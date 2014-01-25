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
#include "pintoInternal.h"

#include "pintoTestCommon.h"

/******************************************************************************/
int testPreconditions()
{
	/* DATA */
	int rc = 0;

	char *string1 = NULL;
	char *string2 = "";

	PintoImage *image1 = NULL;
	PintoImage image;
	PintoImage *image2 = &image;

	PintoText *text1 = NULL;
	PintoText *text2 = NULL;


	/* CODE */
	/* **************************************** */
	printf( "Testing preconditions...\n" ); fflush( stdout );

	TEST_ERR_IF( pintoTextInit( &text1 ) != PINTO_RC_SUCCESS );

	TEST_ERR_IF( pintoImageInit( -1, 10, &image1 ) != PINTO_RC_ERROR_PRECOND );
	TEST_ERR_IF( pintoImageInit(  0, 10, &image1 ) != PINTO_RC_ERROR_PRECOND );
	TEST_ERR_IF( pintoImageInit( PINTO_MAX_WIDTH + 1, 10, &image1 ) != PINTO_RC_ERROR_PRECOND );
	TEST_ERR_IF( pintoImageInit( 10, -1, &image1 ) != PINTO_RC_ERROR_PRECOND );
	TEST_ERR_IF( pintoImageInit( 10,  0, &image1 ) != PINTO_RC_ERROR_PRECOND );
	TEST_ERR_IF( pintoImageInit( 10, PINTO_MAX_HEIGHT + 1, &image1 ) != PINTO_RC_ERROR_PRECOND );
	TEST_ERR_IF( pintoImageInit( 10, 10, NULL ) != PINTO_RC_ERROR_PRECOND );
	TEST_ERR_IF( pintoImageInit( 10, 10, &image2 ) != PINTO_RC_ERROR_PRECOND );

	TEST_ERR_IF( pintoImageEncode( NULL, &string1 ) != PINTO_RC_ERROR_PRECOND );
	TEST_ERR_IF( pintoImageEncode( image2, NULL ) != PINTO_RC_ERROR_PRECOND );
	TEST_ERR_IF( pintoImageEncode( image2, &string2 ) != PINTO_RC_ERROR_PRECOND );

	TEST_ERR_IF( pintoImageDecodeString( NULL, &image1 ) != PINTO_RC_ERROR_PRECOND );
	TEST_ERR_IF( pintoImageDecodeString( string2, NULL ) != PINTO_RC_ERROR_PRECOND );
	TEST_ERR_IF( pintoImageDecodeString( string2, &image2 ) != PINTO_RC_ERROR_PRECOND );

	TEST_ERR_IF( pintoImageDecodeText( NULL, &image1 ) != PINTO_RC_ERROR_PRECOND );
	TEST_ERR_IF( pintoImageDecodeText( &text2, &image1 ) != PINTO_RC_ERROR_PRECOND );
	TEST_ERR_IF( pintoImageDecodeText( &text1, NULL ) != PINTO_RC_ERROR_PRECOND );
	TEST_ERR_IF( pintoImageDecodeText( &text1, &image2 ) != PINTO_RC_ERROR_PRECOND );

	TEST_ERR_IF( pintoTextInit( NULL ) != PINTO_RC_ERROR_PRECOND );
	TEST_ERR_IF( pintoTextInit( &text1 ) != PINTO_RC_ERROR_PRECOND );

	TEST_ERR_IF( pintoTextAddChar( NULL, 'a' ) != PINTO_RC_ERROR_PRECOND );

	TEST_ERR_IF( pintoImageDownsize( NULL, &image1 ) != PINTO_RC_ERROR_PRECOND );
	TEST_ERR_IF( pintoImageDownsize( image2, NULL ) != PINTO_RC_ERROR_PRECOND );
	TEST_ERR_IF( pintoImageDownsize( image2, &image2 ) != PINTO_RC_ERROR_PRECOND );

	/* CLEANUP */
	cleanup:

	pintoTextFree( &text1 );

	return rc;
}

