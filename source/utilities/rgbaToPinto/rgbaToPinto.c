/*
Copyright (C) 2013 Jeremiah Martell
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
#include <stdlib.h> /* atoi, malloc, free */
#include <stdio.h>  /* fread */

#include "pinto.h"

/******************************************************************************/
#define APP_ERR_IF( x ) if ( (x) ) { rc = -__LINE__; goto cleanup; }

/******************************************************************************/
int main( int argc, char **argv )
{
	/* DATA */
	int rc = 0;

	s32 width = 0;
	s32 height = 0;

	PintoImage *image = NULL;
	char *string = NULL;

	PINTO_RC pintoRC = PINTO_RC_SUCCESS;


	/* CODE */
	if ( argc != 3 )
	{
		printf( "rgbaToPinto\n" );
		printf( "Copyright (C) 2013 Jeremiah Martell\n" );
		printf( "http://GeekHorse.com\n" );
		printf( "\n" );
		printf( "rgbaToPinto takes rgba data from stdin and outputs pinto data to stdout.\n" );
		printf( "It helps convert another image format into a pinto image.\n" );
		printf( "NOTE: \"identify\" and \"convert\" mentioned below come from the free\n" );
		printf( "ImageMagick software suite, which can be found at:\n" );
		printf( "http://www.imagemagick.org\n" );
		printf( "\n" );
		printf( "USAGE:\n" );
		printf( "    rgbaToPinto width height\n" );
		printf( "\n" );
		printf( "Example 1: To get the size of a png image:\n" );
		printf( "    identify in.png\n" );
		printf( "Example 2: To generate a pinto image from a png image:\n" );
		printf( "    convert -colors 63 +dither in.png rgba:- | rgbaToPinto WIDTH HEIGHT > out.pinto.\n" );
		printf( "\n" );
		APP_ERR_IF( 1 );
	}

	/* get width */
	width = strtol( argv[ 1 ], NULL, 10 );

	if ( width <= 0 )
	{
		fprintf( stderr, "ERROR: Width was <= 0!\n" );
		APP_ERR_IF( 1 );
	}

	if ( width > PINTO_MAX_WIDTH )
	{
		fprintf( stderr, "ERROR: Width was > %d!\n", PINTO_MAX_WIDTH );
		APP_ERR_IF( 1 );
	}

	/* get height */
	height = strtol( argv[ 2 ], NULL, 10 );

	if ( height <= 0 )
	{
		fprintf( stderr, "ERROR: Height was <= 0!\n" );
		APP_ERR_IF( 1 );
	}

	if ( height > PINTO_MAX_HEIGHT )
	{
		fprintf( stderr, "ERROR: Height was > %d!\n", PINTO_MAX_HEIGHT );
		APP_ERR_IF( 1 );
	}

	/* allocate image */
	pintoRC = pintoImageInit( width, height, &image );
	if ( pintoRC != PINTO_RC_SUCCESS )
	{
		fprintf( stderr, "ERROR: Image initialization failed! (%s)\n", pintoRCToString( pintoRC ) );
		APP_ERR_IF( 1 );
	}

	/* read rgba data from stdin */
	if ( fread( image->rgba, 1, width * height * 4, stdin ) != (size_t)(width * height * 4) )
	{
		fprintf( stderr, "ERROR: Reading from stdin failed!\n" );
		APP_ERR_IF( 1 );
	}
	
	/* convert to pinto */
	pintoRC = pintoImageEncode( image, &string );
	if ( pintoRC != PINTO_RC_SUCCESS )
	{
		fprintf( stderr, "ERROR: rgba to pinto failed! (%s)\n", pintoRCToString( pintoRC ) );
		APP_ERR_IF( 1 );
	}

	/* output to stdout */
	printf( "%s", string );


	/* CLEANUP */
	cleanup:

	free( string );
	string = NULL;

	pintoImageFree( &image );

	return rc;
}

