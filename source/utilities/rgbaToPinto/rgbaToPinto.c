/*
Copyright (C) 2013-2014 Jeremiah Martell
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
#include <stdlib.h> /* strtol, malloc, free */
#include <stdio.h>  /* fread, printf, fprintf */

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

	s32 i = 0;

	char *string = NULL;

	PINTO_RC pintoRC = PINTO_RC_SUCCESS;


	/* CODE */
	if ( argc != 3 && argc != 4 )
	{
		fprintf( stderr, "rgbaToPinto %s\n", PINTO_VERSION_STRING );
		fprintf( stderr, "Copyright (C) 2013-2014 Jeremiah Martell\n" );
		fprintf( stderr, "http://GeekHorse.com\n" );
		fprintf( stderr, "\n" );
		fprintf( stderr, "rgbaToPinto takes rgba data from stdin and outputs pinto data to stdout.\n" );
		fprintf( stderr, "It helps convert another image format into a pinto image.\n" );
		fprintf( stderr, "NOTE: \"identify\" and \"convert\" mentioned below come from the free\n" );
		fprintf( stderr, "ImageMagick software suite, which can be found at:\n" );
		fprintf( stderr, "http://www.imagemagick.org\n" );
		fprintf( stderr, "\n" );
		fprintf( stderr, "USAGE:\n" );
		fprintf( stderr, "    rgbaToPinto width height\n" );
		fprintf( stderr, "    rgbaToPinto width height blackOnly\n" );
		fprintf( stderr, "\n" );
		fprintf( stderr, "Example 1: To get the size of a png image:\n" );
		fprintf( stderr, "    identify in.png\n" );
		fprintf( stderr, "Example 2: To generate a pinto image from a png image:\n" );
		fprintf( stderr, "    convert -colors 63 +dither in.png rgba:- | rgbaToPinto WIDTH HEIGHT > out.pinto.\n" );
		fprintf( stderr, "Example 3: To generate a pinto image from the black pixels in a png image:\n" );
		fprintf( stderr, "           Useful when drawing black on white, but you want the white pixels\n" );
		fprintf( stderr, "           to be transparent. Which could help generating black bitmap fonts.\n" );
		fprintf( stderr, "    convert in.png rgba:- | rgbaToPinto WIDTH HEIGHT 1 > out.pinto.\n" );
		fprintf( stderr, "\n" );
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

	/* blackOnly? */
	if ( argc == 4 )
	{
		while ( i < (width * height * 4) )
		{
			if ( image->rgba[ i + 3 ] != 0 )
			{
				if (    image->rgba[ i     ] != 0
				     || image->rgba[ i + 1 ] != 0
				     || image->rgba[ i + 2 ] != 0
				   )
				{
					image->rgba[ i + 3 ] = 0;
				}
			}

			/* increment */
			i += 4;
		}
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

