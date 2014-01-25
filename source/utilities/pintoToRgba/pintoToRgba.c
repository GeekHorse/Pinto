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
#include <stdlib.h> /* malloc, free */
#include <stdio.h>  /* fread, printf, fprintf, fopen, fclose */
#include <string.h> /* strcmp, strerror */
#include <errno.h>  /* errno */

#include "pinto.h"

/******************************************************************************/
#define APP_ERR_IF( x ) if ( (x) ) { rc = -__LINE__; goto cleanup; }

/******************************************************************************/
#define FLAG_PRINT_USAGE         0
#define FLAG_PRINT_SIZE          1
#define FLAG_PRINT_RGBA          2
#define FLAG_PRINT_RGBA_DOWNSIZE 3

/******************************************************************************/
int main( int argc, char **argv )
{
	/* DATA */
	int rc = 0;

	char flag = FLAG_PRINT_USAGE;

	PintoText *text = NULL;

	FILE *fp = NULL;

	char ch = 0;

	PintoImage *image = NULL;
	PintoImage *imageDownsize = NULL;

	PINTO_RC pintoRC = PINTO_RC_SUCCESS;


	/* CODE */
	if ( argc == 3 )
	{
		if ( strcmp( argv[ 1 ], "getSize" ) == 0 )
		{
			flag = FLAG_PRINT_SIZE;
		}
		else if ( strcmp( argv[ 1 ], "rgba" ) == 0 )
		{
			flag = FLAG_PRINT_RGBA;
		}
		else if ( strcmp( argv[ 1 ], "rgbaDownsize" ) == 0 )
		{
			flag = FLAG_PRINT_RGBA_DOWNSIZE;
		}
	}

	if ( flag == FLAG_PRINT_USAGE )
	{
		fprintf( stderr, "pintoToRgba %s\n", PINTO_VERSION_STRING );
		fprintf( stderr, "Copyright (C) 2013-2014 Jeremiah Martell\n" );
		fprintf( stderr, "http://GeekHorse.com\n" );
		fprintf( stderr, "\n" );
		fprintf( stderr, "pintoToRgba will tell you the size of a pinto image, or output the image's\n" );
		fprintf( stderr, "rgba data to stdout. It can also create anti-aliased images by downsizing\n" );
		fprintf( stderr, "the pinto image by half.\n" );
		fprintf( stderr, "It helps convert a pinto image into another image format.\n" );
		fprintf( stderr, "NOTE: \"convert\" mentioned below comes from the free ImageMagick software\n" );
		fprintf( stderr, "suite, which can be found at: http://www.imagemagick.org\n" );
		fprintf( stderr, "\n" );
		fprintf( stderr, "USAGE:\n" );
		fprintf( stderr, "    pintoToRgba getSize in.pinto\n" );
		fprintf( stderr, "    pintoToRgba rgba in.pinto\n" );
		fprintf( stderr, "    pintoToRgba rgbaDownsize in.pinto\n" );
		fprintf( stderr, "\n" );
		fprintf( stderr, "Example 1: To get the size of a pinto image:\n" );
		fprintf( stderr, "    pintoToRgba getSize in.pinto\n" );
		fprintf( stderr, "Example 2: To generate a png image from a pinto image:\n" );
		fprintf( stderr, "    pintoToRgba rgba in.pinto | convert -depth 8 -size WIDTHxHEIGHT rgba:- out.png\n" );
		fprintf( stderr, "Example 3: To generate an anti-aliased png image from a pinto image:\n" );
		fprintf( stderr, "    pintoToRgba rgbaDownsize in.pinto | convert -depth 8 -size HALFWIDTHxHALFHEIGHT rgba:- out.png\n" );
		fprintf( stderr, "\n" );
		APP_ERR_IF( 1 );
	}

	/* setup text */
	pintoRC = pintoTextInit( &text );
	if ( pintoRC != PINTO_RC_SUCCESS )
	{
		fprintf( stderr, "ERROR: text init failed! (%s)\n", pintoRCToString( pintoRC ) );
		APP_ERR_IF( 1 );
	}

	/* open file */
	fp = fopen( argv[ 2 ], "rb" );
	if ( fp == NULL )
	{
		fprintf( stderr, "ERROR: Could not open file \"%s\": %s\n", argv[ 2 ], strerror( errno ) );
		APP_ERR_IF( 1 );
	}
	
	/* read pinto data from stdin */
	while ( fread( &ch, 1, 1, fp ) == 1 )
	{
		pintoRC = pintoTextAddChar( text, ch );
		if ( pintoRC != PINTO_RC_SUCCESS )
		{
			fprintf( stderr, "ERROR: text add char failed! (%s)\n", pintoRCToString( pintoRC ) );
			APP_ERR_IF( 1 );
		}
	}

	/* decode to image */
	pintoRC = pintoImageDecodeText( &text, &image );
	if ( pintoRC != PINTO_RC_SUCCESS )
	{
		fprintf( stderr, "ERROR: pinto decode failed! (%s)\n", pintoRCToString( pintoRC ) );
		APP_ERR_IF( 1 );
	}

	if ( flag == FLAG_PRINT_SIZE )
	{
		printf( "%dx%d\n", image->width, image->height );
	}
	else if ( flag == FLAG_PRINT_RGBA )
	{
		/* output to stdout */
		fwrite( image->rgba, 1, image->width * image->height * 4, stdout );
	}
	else /* FLAG_PRINT_RGBA_DOWNSIZE */
	{
		/* downsize */
		pintoRC	= pintoImageDownsize( image, &imageDownsize );
		if ( pintoRC != PINTO_RC_SUCCESS )
		{
			fprintf( stderr, "ERROR: pinto image downsize failed! (%s)\n", pintoRCToString( pintoRC ) );
			APP_ERR_IF( 1 );
		}
		
		/* output to stdout */
		fwrite( imageDownsize->rgba, 1, imageDownsize->width * imageDownsize->height * 4, stdout );
	}


	/* CLEANUP */
	cleanup:

	pintoImageFree( &image );
	pintoImageFree( &imageDownsize );

	if ( fp != NULL )
	{
		fclose( fp );
		fp = NULL;
	}

	free( text );

	return rc;
}

