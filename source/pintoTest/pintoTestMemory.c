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
#define PINTO_FILE_NUMBER 500

/******************************************************************************/
#include "pinto.h"
#include "pintoInternal.h"

#include "pintoTestCommon.h"

/******************************************************************************/
/* which malloc to fail on */
static s32 failOnMallocCount = 0;
static s32 currentMallocCount = 0;

void *pintoHookMalloc( size_t size )
{
	currentMallocCount += 1;
	if ( currentMallocCount == failOnMallocCount )
	{
		return NULL;
	}

	return malloc( size );
}

void *pintoHookCalloc( size_t nmemb, size_t size )
{
	currentMallocCount += 1;
	if ( currentMallocCount == failOnMallocCount )
	{
		return NULL;
	}

	return calloc( nmemb, size );
}

void *pintoHookRealloc( void *ptr, size_t size )
{
	currentMallocCount += 1;
	if ( currentMallocCount == failOnMallocCount )
	{
		return NULL;
	}

	return realloc( ptr, size );
}

void pintoHookFree( void *ptr )
{
	free( ptr );
}

/******************************************************************************/
#ifdef DEBUG
static PINTO_RC testFailedMallocs1( s32 test );
static PINTO_RC testFailedMallocs2( s32 test );
static PINTO_RC testFailedMallocs3( s32 test );
static PINTO_RC testFailedMallocs4( s32 test );

typedef struct
{
	PINTO_RC (*func)( s32 test );
	s32 numberOfTests;
} FailedFunc;

static FailedFunc failedFuncs[] =
{
	{ testFailedMallocs1, 1 },
	{ testFailedMallocs2, 1 },
	{ testFailedMallocs3, 2 },
	{ testFailedMallocs4, 13 },
	{ NULL, 0 }
};
#endif

/******************************************************************************/
int testMemory()
{
	/* DATA */
	int rc = 0;

	int **iArray = NULL;

#ifdef DEBUG
	s32 i = 0;
	s32 j = 0;
	s32 flagAtLeastOneFailed = 0;
	char *spinner = "-\\|/";
	u8 spinnerI = 0;
#endif

	s32 OLD_PINTO_TEXT_SIZE_GROWTH = 0;


	/* CODE */
	/* **************************************** */
	printf( "Testing memory management...\n" ); fflush( stdout );

	/* **************************************** */
	/* testing bad mallocs */
	printf( "  Testing bad mallocs...\n" ); fflush( stdout );

#ifndef DEBUG

	printf( "\n\n\n" );
	printf( "--- SKIPPING FAILED MALLOC TESTS! ------\n" );
	printf( "You need to build a debug version of Pinto to test failed mallocs.\n" );
	printf( "----------------------------------------\n" );
	printf( "\n\n\n" );

#else

	OLD_PINTO_TEXT_SIZE_GROWTH = PINTO_TEXT_SIZE_GROWTH;
	PINTO_TEXT_SIZE_GROWTH = 1;

	i = 0;
	while ( failedFuncs[ i ].func != NULL )
	{
		j = 0;
		while ( j < failedFuncs[ i ].numberOfTests )
		{
			failOnMallocCount = 0;
			spinnerI = 0;
			flagAtLeastOneFailed = 0;

			do
			{
				spinnerI += 1;
				printf( "\r%d: %d of %d %c", i, j, failedFuncs[ i ].numberOfTests - 1, spinner[ spinnerI % 4 ]  ); fflush( stdout );

				currentMallocCount = 0;
				failOnMallocCount += 1;

				rc = failedFuncs[ i ].func( j );

				if ( rc == PINTO_RC_ERROR_MEMORY_ALLOCATION_FAILED )
				{
					flagAtLeastOneFailed = 1;
				}
			}
			while ( rc == PINTO_RC_ERROR_MEMORY_ALLOCATION_FAILED );

			if ( rc != PINTO_RC_SUCCESS )
			{
				printf( "\nERROR: Expecting PINTO_RC_SUCCESS but got %s.\n", pintoRCToString( rc ) );
				TEST_ERR_IF( 1 );
			}

			if ( flagAtLeastOneFailed == 0 )
			{
				printf( "There were no memory failures. There should have been at least 1.\n" );
				TEST_ERR_IF( 1 );
			}

			j += 1;
		}

		printf( "\r%d            \n", i ); fflush( stdout );

		i += 1;
	}

	failOnMallocCount = 0;
#endif

	/* *** */
	PINTO_TEXT_SIZE_GROWTH = OLD_PINTO_TEXT_SIZE_GROWTH;

	/* **************************************** */
	/* test that calloc sets pointers to NULL */
	printf( "  Testing calloc...\n" ); fflush( stdout );
	iArray = (int **) PINTO_HOOK_CALLOC( 10, sizeof( int * ) );
	TEST_ERR_IF( iArray == NULL );
	TEST_ERR_IF( iArray[ 5 ] != NULL );
	PINTO_HOOK_FREE( iArray );


	/* CLEANUP */
	cleanup:

	return rc;
}

#ifdef DEBUG
/******************************************************************************/
static PINTO_RC testFailedMallocs1( s32 test )
{
	/* DATA */
	PINTO_RC rc = PINTO_RC_SUCCESS;

	PintoImage *image1 = NULL;
	PintoImage *image1Downsized = NULL;
	PintoImage *image2 = NULL;
	char *encoding = NULL;


	/* CODE */
	(void)test;

	/* 32x32 white image */
	rc = pintoImageInit( 32, 32, &image1 );
	ERR_IF_PASSTHROUGH;

	testImageAddRun( image1, 0, 2, 255, 255, 255 );
	testImageAddRun( image1, 4, 2, 255, 255, 255 );
	testImageAddRun( image1, 8, 2, 255, 255, 255 );
	testImageAddRun( image1, 12, 2, 255, 255, 255 );
	testImageAddRun( image1, 16, 2, 255, 255, 255 );
	testImageAddRun( image1, 20, 2, 255, 255, 255 );
	testImageAddRun( image1, 24, 2, 255, 255, 255 );

	/* downsize */
	rc = pintoImageDownsize( image1, &image1Downsized );
	ERR_IF_PASSTHROUGH;

	/* encode */
	rc = pintoImageEncode( image1, &encoding );
	ERR_IF_PASSTHROUGH;

	/* decode */
	rc = pintoImageDecodeString( encoding, &image2 );
	ERR_IF_PASSTHROUGH;


	/* CLEANUP */
	cleanup:

	PINTO_HOOK_FREE( encoding );
	pintoImageFree( &image1 );
	pintoImageFree( &image2 );
	pintoImageFree( &image1Downsized );

	return rc;
}

/******************************************************************************/
static PINTO_RC testFailedMallocs2( s32 test )
{
	/* DATA */
	PINTO_RC rc = PINTO_RC_SUCCESS;

	PintoText *text = NULL;


	/* CODE */
	(void)test;

	rc = pintoTextInit( &text );
	ERR_IF_PASSTHROUGH;

	rc = pintoTextAddValue( text, 0 );
	ERR_IF_PASSTHROUGH;
	rc = pintoTextAddValue( text, 1 );
	ERR_IF_PASSTHROUGH;
	rc = pintoTextAddValue( text, 63 );
	ERR_IF_PASSTHROUGH;
	rc = pintoTextAddValue( text, 64 );
	ERR_IF_PASSTHROUGH;
	rc = pintoTextAddValue( text, ( 64 * 2 ) - 1 );
	ERR_IF_PASSTHROUGH;
	rc = pintoTextAddValue( text, 64 * 2 );
	ERR_IF_PASSTHROUGH;
	rc = pintoTextAddValue( text, ( 64 * 16 ) - 1 );
	ERR_IF_PASSTHROUGH;
	rc = pintoTextAddValue( text, 64 * 16 );
	ERR_IF_PASSTHROUGH;
	rc = pintoTextAddValue( text, ( 64 * 17 ) - 1 );
	ERR_IF_PASSTHROUGH;
	rc = pintoTextAddValue( text, 64 * 17 );
	ERR_IF_PASSTHROUGH;
	rc = pintoTextAddValue( text, ( 64 * 64 ) - 1 );
	ERR_IF_PASSTHROUGH;
	rc = pintoTextAddValue( text, 64 * 64 );
	ERR_IF_PASSTHROUGH;
	rc = pintoTextAddValue( text, ( 64 * 64 * 64 ) - 1 );
	ERR_IF_PASSTHROUGH;
	rc = pintoTextAddValue( text, 64 * 64 * 64 );
	ERR_IF_PASSTHROUGH;
	rc = pintoTextAddValue( text, ( 64 * 64 * 64 * 64 ) - 1 );
	ERR_IF_PASSTHROUGH;


	/* CLEANUP */
	cleanup:

	pintoTextFree( &text );

	return rc;
}

/******************************************************************************/
static PINTO_RC testFailedMallocs3( s32 test )
{
	/* DATA */
	PINTO_RC rc = PINTO_RC_SUCCESS;

	s32 i = 0;

	PintoText *textIn = NULL;
	PintoText *textOut = NULL;

	char *in[] =
	{
		"AAAAA",
		"ABCDE002x003x004x005x006x007x008x009x010x011x012x013x014x015x016ABCDE"
	};


	/* CODE */
	/* create textIn */
	rc = pintoTextInit( &textIn );
	ERR_IF_PASSTHROUGH;

	for ( i = 0; in[ test ][ i ] != '\0'; i += 1 )
	{
		rc = pintoTextAddChar( textIn, in[ test ][ i ] );
		ERR_IF_PASSTHROUGH;
	}

	/* deflate */
	rc = pintoSimpleDeflate( &textIn, &textOut );
	ERR_IF_PASSTHROUGH;

	/* inflate */
	rc = pintoSimpleInflate( &textOut, &textIn );
	ERR_IF_PASSTHROUGH;


	/* CLEANUP */
	cleanup:

	pintoTextFree( &textIn );
	pintoTextFree( &textOut );

	return rc;
}

/******************************************************************************/
static PINTO_RC testFailedMallocs4( s32 test )
{
	/* DATA */
	PINTO_RC rc = PINTO_RC_SUCCESS;

	PintoImage *image1 = NULL;
	PintoImage *image2 = NULL;
	char *encoding = NULL;


	/* CODE */
	(void)test;

	/* image */
	rc = pintoImageInit( ( (s32) 1 ) << test, ( (s32) 1 ) << test, &image1 );
	ERR_IF_PASSTHROUGH;

	/* mark 1st pixel as black */
	image1->rgba[ 0 ] = 0;
	image1->rgba[ 1 ] = 0;
	image1->rgba[ 2 ] = 0;
	image1->rgba[ 3 ] = 255;

	/* encode */
	rc = pintoImageEncode( image1, &encoding );
	ERR_IF_PASSTHROUGH;

	/* decode */
	rc = pintoImageDecodeString( encoding, &image2 );
	ERR_IF_PASSTHROUGH;


	/* CLEANUP */
	cleanup:

	PINTO_HOOK_FREE( encoding );
	pintoImageFree( &image1 );
	pintoImageFree( &image2 );

	return rc;
}
#endif

