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
#include <string.h> /* strcmp */
#include <errno.h>  /* errno */

#include "pinto.h"
#include "pintoInternal.h"

#include "pintoTestCommon.h"

/******************************************************************************/
int testMisc()
{
	/* DATA */
	int rc = 0;

	const char *string = NULL;

	PintoText *text = NULL;

	s32 value = 0;
	s32 valueVerify = 0;
	s32 i = 0;
	s32 j = 0;

	PintoImage *image1 = NULL;
	PintoImage *image2 = NULL;


	/* CODE */
	/* **************************************** */
	printf( "Testing misc...\n" ); fflush( stdout );

	/* these should not crash or have memory issues */
	pintoImageFree( NULL );
	pintoTextFree( NULL );

	/* make sure we haven't forgotten any text for our RCs */
	string = pintoRCToString( PINTO_RC_SUCCESS );
	TEST_ERR_IF( strcmp( string, "Success" ) != 0 );

	string = pintoRCToString( PINTO_RC_STANDARD_ERRORS_MAX );
	TEST_ERR_IF( strcmp( string, "Unknown Error" ) == 0 );

	string = pintoRCToString( PINTO_RC_STANDARD_ERRORS_MAX + 1 );
	TEST_ERR_IF( strcmp( string, "Unknown Error" ) != 0 );

	string = pintoRCToString( PINTO_RC_PINTO_ERRORS_MIN - 1 );
	TEST_ERR_IF( strcmp( string, "Unknown Error" ) != 0 );

	string = pintoRCToString( PINTO_RC_PINTO_ERRORS_MIN );
	TEST_ERR_IF( strcmp( string, "Unknown Error" ) == 0 );

	string = pintoRCToString( PINTO_RC_PINTO_ERRORS_MAX );
	TEST_ERR_IF( strcmp( string, "Unknown Error" ) == 0 );

	string = pintoRCToString( PINTO_RC_PINTO_ERRORS_MAX + 1 );
	TEST_ERR_IF( strcmp( string, "Unknown Error" ) != 0 );

	string = pintoRCToString( -1 );
	TEST_ERR_IF( strcmp( string, "Unknown Error" ) != 0 );

	string = pintoRCToString( -2 );
	TEST_ERR_IF( strcmp( string, "Unknown Error" ) != 0 );

	/* run our pintoTest through all values */
	printf( "[                                                                ]\r[" );
	fflush( stdout );

	for ( i = 0; i < 64; i += 1 )
	{
		TEST_ERR_IF( pintoTextInit( &text ) != PINTO_RC_SUCCESS );

		for ( j = 0; j < ( 64 * 64 * 64 ); j += 1 )
		{
			TEST_ERR_IF( pintoTextAddValue( text, value + j ) != PINTO_RC_SUCCESS );
		}
		for ( j = 0; j < ( 64 * 64 * 64 ); j += 1 )
		{
			TEST_ERR_IF( pintoTextGetValue( text, &valueVerify ) != PINTO_RC_SUCCESS );
			if ( valueVerify != ( value + j ) )
			{
				printf( "%d != %d\n", valueVerify, ( value + j ) ); fflush( stdout );
				TEST_ERR_IF( 1 );
			}
		}

		pintoTextFree( &text );

		printf( "." ); fflush( stdout );

		value += ( 64 * 64 * 64 );
	}

	printf( "\n" );

	/* test bad format if we add PINTO_MAX_WIDTH * PINTO_MAX_HEIGHT characters */
	printf( "[                                                                ]\r[" );
	fflush( stdout );

	TEST_ERR_IF( pintoTextInit( &text ) != PINTO_RC_SUCCESS );
	for ( i = 0; i < PINTO_MAX_WIDTH; i += 1 )
	{
		for ( j = 0; j < PINTO_MAX_HEIGHT; j += 1 )
		{
			TEST_ERR_IF( pintoTextAddChar( text, 'x' ) != PINTO_RC_SUCCESS );
		}
		if ( i % 64 == 0 ) { printf( "." ); fflush( stdout ); }
	}
	printf ( "\n" );

	TEST_ERR_IF( pintoTextAddChar( text, 'x' ) != PINTO_RC_ERROR_FORMAT_TOO_LONG );

	/* double-check */
	TEST_ERR_IF( text->string[ ( PINTO_MAX_WIDTH * PINTO_MAX_HEIGHT ) - 1 ] != 'x' );
	TEST_ERR_IF( text->string[ ( PINTO_MAX_WIDTH * PINTO_MAX_HEIGHT ) ] != '\0' );
	
	pintoTextFree( &text );

	/* check "image too small" in downsize function */
	TEST_ERR_IF( pintoImageInit( 1, 32, &image1 ) != PINTO_RC_SUCCESS );
	TEST_ERR_IF( pintoImageDownsize( image1, &image2 ) != PINTO_RC_ERROR_IMAGE_TOO_SMALL );
	pintoImageFree( &image1 );

	TEST_ERR_IF( pintoImageInit( 32, 1, &image1 ) != PINTO_RC_SUCCESS );
	TEST_ERR_IF( pintoImageDownsize( image1, &image2 ) != PINTO_RC_ERROR_IMAGE_TOO_SMALL );
	pintoImageFree( &image1 );

	/* check log function */
	/*    rc success */
	pintoHookLog( 1, 2, 3, 0, 10, 11, 12 );
	/*    with errno */
	errno = 1;
	pintoHookLog( 1, 2, 3, 1, 10, 11, 12 );
	pintoHookLog( 1, 2, 3, 0, 10, 11, 12 );
	errno = 0;

	printf( "\n" );


	/* CLEANUP */
	cleanup:

	pintoTextFree( &text );

	return rc;
}

/******************************************************************************/
void testImageAddRun( PintoImage *image, s32 startIndex, s32 length, u8 red, u8 green, u8 blue )
{
	/* CODE */
	startIndex *= 4;

	while ( length > 0 )
	{
		PARANOID_ERR_IF( startIndex + 3 >= (image->width * image->height * 4) );

		image->rgba[ startIndex     ] = red;
		image->rgba[ startIndex + 1 ] = green;
		image->rgba[ startIndex + 2 ] = blue;
		image->rgba[ startIndex + 3 ] = 255;

		/* *** */
		length -= 1;
		startIndex += 4;
	}

	return;
}

