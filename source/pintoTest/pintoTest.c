/*
Copyright (C) 2012-2014 Jeremiah Martell
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    - Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    - Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    - Neither the name of Jeremiah Martell nor the name of GeekHorse nor the
      name of Pinto nor the names of its contributors may be used to endorse
      or promote products derived from this software without specific prior
      written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/******************************************************************************/
#include "pinto.h"

#include "pintoTestCommon.h"

#include <stdio.h> /* printf(), fprintf(), fflush() */ 
#include <string.h> /* strcmp() */
#include <time.h> /* time(), for seeding random number generator */

/******************************************************************************/
static int _getArgValue( int argc, char **argv, char *key, char **value );

/******************************************************************************/
int main( int argc, char **argv )
{
	/* DATA */
	int rc = 0;

	char *argValue = 0;

	int seed = 1;

	char flagTestAll = 0;
	char flagTestPreconditions = 0;
	char flagTestMisc = 0;
	char flagTestMemory = 0;
	char flagTestEncodingDecoding = 0;

	char flagTestAnySet = 0;

	time_t timeStart = 0;
	time_t timeEnd = 0;


	/* CODE */
	printf( "Testing:\n" );
	printf( "     %s %s\n", PINTO_NAME, PINTO_VERSION_STRING );
	printf( "     %s\n", PINTO_COPYRIGHT );
	printf( "\n" );

	/* **************************************** */
	rc = _getArgValue( argc, argv, "-s", &argValue );
	if ( rc == 0 )
	{
		seed = strtol( argValue, NULL, 10 );
	}
	else
	{
		seed = time( NULL );
	}

	/* **************************************** */
	rc = _getArgValue( argc, argv, "-t", &argValue );
	if ( rc == 0 )
	{
		if ( strcmp( argValue, "all" ) == 0 )
		{
			flagTestAll = 1;
			flagTestAnySet = 1;
		}
		else if ( strcmp( argValue, "pre" ) == 0 )
		{
			flagTestPreconditions = 1;
			flagTestAnySet = 1;
		}
		else if ( strcmp( argValue, "misc" ) == 0 )
		{
			flagTestMisc = 1;
			flagTestAnySet = 1;
		}
		else if ( strcmp( argValue, "memory" ) == 0 )
		{
			flagTestMemory = 1;
			flagTestAnySet = 1;
		}
		else if ( strcmp( argValue, "image" ) == 0 )
		{
			flagTestEncodingDecoding = 1;
			flagTestAnySet = 1;
		}
		else
		{
			fprintf( stderr, "UNKNOWN TEST TO RUN: \"%s\"\n", argValue );
			TEST_ERR_IF( 1 );
		}
	}

	if ( flagTestAnySet == 0 )
	{
		fprintf( stderr, "Usage: pintoTest [options]\n" );
		fprintf( stderr, "  -s <NUMBER>    Seed for random number generator\n" );
		fprintf( stderr, "  -t <TEST>      Test to run\n" );
		fprintf( stderr, "                 Possible tests:\n" );
		fprintf( stderr, "                   all = all tests\n" );
		fprintf( stderr, "                   pre = preconditions\n" );
		fprintf( stderr, "                   misc = misc\n" );
		fprintf( stderr, "                   memory = memory\n" );
		fprintf( stderr, "                   image = encoding/decoding\n" );
		fprintf( stderr, "\n" );

		return -1;
	}

	/* **************************************** */
	printf( "Using seed: %d\n", seed ); fflush( stdout );
	srand( seed );

	/* **************************************** */
	timeStart = time( NULL );

	/* **************************************** */
	TEST_ERR_IF( sizeof( s32 ) != 4 );
	TEST_ERR_IF( sizeof( u8 ) != 1 );

	/* **************************************** */
	if ( flagTestAll || flagTestPreconditions )
	{
		TEST_ERR_IF( testPreconditions() != 0 );
	}

	if ( flagTestAll || flagTestMisc )
	{
		TEST_ERR_IF( testMisc() != 0 );
	}

	if ( flagTestAll || flagTestEncodingDecoding )
	{
		TEST_ERR_IF( testEncodingDecoding() != 0 );
	}

	if ( flagTestAll || flagTestMemory )
	{
		TEST_ERR_IF( testMemory() != 0 );
	}

	/* **************************************** */
	timeEnd = time( NULL );

	printf( "\nTests took %ld:%02ld:%02ld to complete\n",
		( timeEnd - timeStart ) / (60 * 60),
		( ( timeEnd - timeStart ) / 60 ) % 60,
		( timeEnd - timeStart ) % 60 );

	/* **************************************** */
	/* success! */
	printf( "SUCCESS!\n\n" );

	return 0;


	/* CLEANUP */
	cleanup:

	printf( "FAILED AT %d\n\n", rc );

	return rc;
}

/******************************************************************************/
static int _getArgValue( int argc, char **argv, char *key, char **value )
{
	/* DATA */
	int i = 0;


	/* CODE */
	while ( i < argc )
	{
		if ( strcmp( key, argv[ i ] ) == 0 )
		{
			i += 1;
			if ( i >= argc )
			{
				printf( "ERROR: Command line argument \"%s\" had no value.\n", key );
				return -1;
			}

			(*value) = argv[ i ];
			return 0;
		}

		i += 1;
	}

	return -2;
}

