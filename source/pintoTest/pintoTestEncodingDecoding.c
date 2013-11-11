/*
Copyright (C) 2012-2013 Jeremiah Martell
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

#include "pinto.h"
#include "pintoInternal.h"

#include "pintoTestCommon.h"

/******************************************************************************/
static int _testGoodImages();
static int _testGoodFormats();
static int _testBadImages();
static int _testBadFormats();
static int _testGoodDeflate();
static int _testBadDeflate();
static int _testRandomImages(
	char *description,
	int numberOfImages,
	int maxNumberOfColors,
	int blackOnly,
	int simpleSizes,
	int square,
	int maxSize );
static int _testImageVerify( PintoImage *image );

/******************************************************************************/
int testEncodingDecoding()
{
	/* DATA */
	int rc = 0;


	/* CODE */
	printf( "Testing encoding/decoding...\n" ); fflush( stdout );

	TEST_ERR_IF( _testGoodImages() != 0 );
	TEST_ERR_IF( _testGoodFormats() != 0 );
	TEST_ERR_IF( _testGoodDeflate() != 0 );
	TEST_ERR_IF( _testBadImages() != 0 );
	TEST_ERR_IF( _testBadFormats() != 0 );
	TEST_ERR_IF( _testBadDeflate() != 0 );
	TEST_ERR_IF( _testRandomImages( "Testing 100 random \"simple\" square images...", 100, 1, 1, 1, 1, PINTO_MAX_WIDTH ) != 0 );
	TEST_ERR_IF( _testRandomImages( "Testing 100 random \"simple\" non-square images...", 100, 1, 1, 1, 0, PINTO_MAX_WIDTH ) != 0 );
	TEST_ERR_IF( _testRandomImages( "Testing 100000 random small images...", 100000, 3, 0, 0, 0, 32 ) != 0 );
	TEST_ERR_IF( _testRandomImages( "Testing 100 random worst case images...", 100, PINTO_MAX_COLORS, 0, 0, 0, PINTO_MAX_WIDTH ) != 0 );


	/* CLEANUP */
	cleanup:

	return rc;
}

/******************************************************************************/
static int _testGoodImages()
{
	/* DATA */
	int rc = 0;

	PintoImage *image = NULL;

	char *encoding = NULL;
	PintoImage *imageOut = NULL;


	/* CODE */
	printf( "  Testing good images...\n" ); fflush( stdout );

	/* 1x1 transparent image */
	TEST_ERR_IF( pintoImageInit( 1, 1, &image ) != PINTO_RC_SUCCESS );
	TEST_ERR_IF( _testImageVerify( image ) != 0 );
	pintoImageFree( &image );
	printf( "." ); fflush( stdout );
	
	/* 1x1 white image */
	TEST_ERR_IF( pintoImageInit( 1, 1, &image ) != PINTO_RC_SUCCESS );
	testImageAddRun( image, 0, 1, 255, 255, 255 );
	TEST_ERR_IF( _testImageVerify( image ) != 0 );
	pintoImageFree( &image );
	printf( "." ); fflush( stdout );

	/* 1x1 black image */
	TEST_ERR_IF( pintoImageInit( 1, 1, &image ) != PINTO_RC_SUCCESS );
	testImageAddRun( image, 0, 1, 0, 0, 0 );
	TEST_ERR_IF( _testImageVerify( image ) != 0 );
	pintoImageFree( &image );
	printf( "." ); fflush( stdout );

	/* 32x32 transparent image */
	TEST_ERR_IF( pintoImageInit( 32, 32, &image ) != PINTO_RC_SUCCESS );
	TEST_ERR_IF( _testImageVerify( image ) != 0 );
	pintoImageFree( &image );
	printf( "." ); fflush( stdout );

	/* 32x32 white image */
	TEST_ERR_IF( pintoImageInit( 32, 32, &image ) != PINTO_RC_SUCCESS );
	testImageAddRun( image, 0, 32 * 32, 255, 255, 255 );
	TEST_ERR_IF( _testImageVerify( image ) != 0 );
	pintoImageFree( &image );
	printf( "." ); fflush( stdout );

	/* MAXxMAX transparent */
	TEST_ERR_IF( pintoImageInit( PINTO_MAX_WIDTH, PINTO_MAX_HEIGHT, &image ) != PINTO_RC_SUCCESS );
	TEST_ERR_IF( _testImageVerify( image ) != 0 );
	pintoImageFree( &image );
	printf( "." ); fflush( stdout );

	/* MAXxMAX white */
	TEST_ERR_IF( pintoImageInit( PINTO_MAX_WIDTH, PINTO_MAX_HEIGHT, &image ) != PINTO_RC_SUCCESS );
	testImageAddRun( image, 0, PINTO_MAX_WIDTH * PINTO_MAX_HEIGHT, 255, 255, 255 );
	TEST_ERR_IF( _testImageVerify( image ) != 0 );
	pintoImageFree( &image );
	printf( "." ); fflush( stdout );

	/* MAXxMAX first pixel transparent, remaining pixels white */
	TEST_ERR_IF( pintoImageInit( PINTO_MAX_WIDTH, PINTO_MAX_HEIGHT, &image ) != PINTO_RC_SUCCESS );
	testImageAddRun( image, 1, ( PINTO_MAX_WIDTH * PINTO_MAX_HEIGHT ) - 1, 255, 255, 255 );
	TEST_ERR_IF( _testImageVerify( image ) != 0 );
	pintoImageFree( &image );
	printf( "." ); fflush( stdout );

	/* MAXxMAX last pixel transparent, remaining pixels white */
	TEST_ERR_IF( pintoImageInit( PINTO_MAX_WIDTH, PINTO_MAX_HEIGHT, &image ) != PINTO_RC_SUCCESS );
	testImageAddRun( image, 0, ( PINTO_MAX_WIDTH * PINTO_MAX_HEIGHT ) - 1, 255, 255, 255 );
	TEST_ERR_IF( _testImageVerify( image ) != 0 );
	pintoImageFree( &image );
	printf( "." ); fflush( stdout );

	/* MAXxMAX first pixel black, remaining pixels white */
	TEST_ERR_IF( pintoImageInit( PINTO_MAX_WIDTH, PINTO_MAX_HEIGHT, &image ) != PINTO_RC_SUCCESS );
	testImageAddRun( image, 0, 1, 0, 0, 0 );
	testImageAddRun( image, 1, ( PINTO_MAX_WIDTH * PINTO_MAX_HEIGHT ) - 1, 255, 255, 255 );
	TEST_ERR_IF( _testImageVerify( image ) != 0 );
	pintoImageFree( &image );
	printf( "." ); fflush( stdout );

	/* MAXxMAX last pixel black, remaining pixels white */
	TEST_ERR_IF( pintoImageInit( PINTO_MAX_WIDTH, PINTO_MAX_HEIGHT, &image ) != PINTO_RC_SUCCESS );
	testImageAddRun( image, 0, ( PINTO_MAX_WIDTH * PINTO_MAX_HEIGHT ) - 1, 255, 255, 255 );
	testImageAddRun( image, ( PINTO_MAX_WIDTH * PINTO_MAX_HEIGHT ) - 1, 1, 0, 0, 0 );
	TEST_ERR_IF( _testImageVerify( image ) != 0 );
	pintoImageFree( &image );
	printf( "." ); fflush( stdout );

	/* Manually check the PINTO_8_BITS_TO_6_BITS */
	TEST_ERR_IF( pintoImageInit( 1, 14, &image ) != PINTO_RC_SUCCESS );
	/* red */
	image->rgba[ 0 * 4 + 0 ] = 0x01;
	image->rgba[ 0 * 4 + 3 ] = 255;

	image->rgba[ 1 * 4 + 0 ] = 0x40;
	image->rgba[ 1 * 4 + 3 ] = 255;

	image->rgba[ 2 * 4 + 0 ] = 0x80;
	image->rgba[ 2 * 4 + 3 ] = 255;

	image->rgba[ 3 * 4 + 0 ] = 0xC0;
	image->rgba[ 3 * 4 + 3 ] = 255;

	/* green */
	image->rgba[ 4 * 4 + 1 ] = 0x01;
	image->rgba[ 4 * 4 + 3 ] = 255;

	image->rgba[ 5 * 4 + 1 ] = 0x40;
	image->rgba[ 5 * 4 + 3 ] = 255;

	image->rgba[ 6 * 4 + 1 ] = 0x80;
	image->rgba[ 6 * 4 + 3 ] = 255;

	image->rgba[ 7 * 4 + 1 ] = 0xC0;
	image->rgba[ 7 * 4 + 3 ] = 255;

	/* blue */
	image->rgba[ 8 * 4 + 2 ] = 0x01;
	image->rgba[ 8 * 4 + 3 ] = 255;

	image->rgba[ 9 * 4 + 2 ] = 0x40;
	image->rgba[ 9 * 4 + 3 ] = 255;

	image->rgba[ 10 * 4 + 2 ] = 0x80;
	image->rgba[ 10 * 4 + 3 ] = 255;

	image->rgba[ 11 * 4 + 2 ] = 0xC0;
	image->rgba[ 11 * 4 + 3 ] = 255;

	/* black */
	image->rgba[ 12 * 4 + 3 ] = 255;

	/* white */
	image->rgba[ 13 * 4 + 0 ] = 255;
	image->rgba[ 13 * 4 + 1 ] = 255;
	image->rgba[ 13 * 4 + 2 ] = 255;
	image->rgba[ 13 * 4 + 3 ] = 255;

	/* encode image */
	TEST_ERR_IF( pintoImageEncode( image, &encoding ) != PINTO_RC_SUCCESS );

	/* decode what we just encoded to imageOut */
	TEST_ERR_IF( pintoImageDecodeString( encoding, &imageOut ) != PINTO_RC_SUCCESS );

	/* verify pixels */
	TEST_ERR_IF( imageOut->rgba[  0 * 4 + 0 ] != 0x00 );
	TEST_ERR_IF( imageOut->rgba[  1 * 4 + 0 ] != 0x41 );
	TEST_ERR_IF( imageOut->rgba[  2 * 4 + 0 ] != 0x82 );
	TEST_ERR_IF( imageOut->rgba[  3 * 4 + 0 ] != 0xC3 );
	TEST_ERR_IF( imageOut->rgba[  4 * 4 + 1 ] != 0x00 );
	TEST_ERR_IF( imageOut->rgba[  5 * 4 + 1 ] != 0x41 );
	TEST_ERR_IF( imageOut->rgba[  6 * 4 + 1 ] != 0x82 );
	TEST_ERR_IF( imageOut->rgba[  7 * 4 + 1 ] != 0xC3 );
	TEST_ERR_IF( imageOut->rgba[  8 * 4 + 2 ] != 0x00 );
	TEST_ERR_IF( imageOut->rgba[  9 * 4 + 2 ] != 0x41 );
	TEST_ERR_IF( imageOut->rgba[ 10 * 4 + 2 ] != 0x82 );
	TEST_ERR_IF( imageOut->rgba[ 11 * 4 + 2 ] != 0xC3 );

	TEST_ERR_IF( imageOut->rgba[ 12 * 4 + 0 ] != 0x00 );
	TEST_ERR_IF( imageOut->rgba[ 12 * 4 + 1 ] != 0x00 );
	TEST_ERR_IF( imageOut->rgba[ 12 * 4 + 2 ] != 0x00 );
	TEST_ERR_IF( imageOut->rgba[ 12 * 4 + 3 ] != 0xFF );

	TEST_ERR_IF( imageOut->rgba[ 13 * 4 + 0 ] != 0xFF );
	TEST_ERR_IF( imageOut->rgba[ 13 * 4 + 1 ] != 0xFF );
	TEST_ERR_IF( imageOut->rgba[ 13 * 4 + 2 ] != 0xFF );
	TEST_ERR_IF( imageOut->rgba[ 13 * 4 + 3 ] != 0xFF );

	printf( "." ); fflush( stdout );
	

	printf( "\n" );


	/* CLEANUP */
	cleanup:

	pintoImageFree( &image );

	pintoHookFree( encoding );
	encoding = NULL;

	pintoImageFree( &imageOut );

	return rc;
}


/******************************************************************************/
static int _testGoodFormats()
{
	/* DATA */
	int rc = 0;

	PintoImage *image = NULL;

	s32 i = 0;
	char *formats[] =
	{
		"a441000?64^",
		"   \n\
\t_a4~4` |\\  !\"1[0~]0{0?_64^}  ",

		NULL
	};


	/* CODE */
	printf( "  Testing good formats...\n" ); fflush( stdout );

	while ( formats[ i ] != NULL )
	{
		pintoImageFree( &image );
		TEST_ERR_IF( pintoImageDecodeString( formats[ i ], &image ) != PINTO_RC_SUCCESS );

		printf( "." ); fflush( stdout );
		i += 1;
	}

	printf( "\n" );


	/* CLEANUP */
	cleanup:

	pintoImageFree( &image );

	return rc;
}

/******************************************************************************/
static int _testBadFormats()
{
	/* DATA */
	int rc = 0;

	PintoImage *image = NULL;

	s32 i = 0;
	char *formats[] =
	{
		/* bad header type */
		"Z0410000121812^",

		/* width == 0 */
		"a0410000121812^",
		/* bad width */
		"a^410000121812^",

		/* height == 0 */
		"a4010000121812^",
		/* bad height */
		"a4^10000121812^",

		/* bad number of colors */
		"a44^0000121812^",
		"a44#00000121812^", /* number of colors 64 */

		/* bad color values */
		"a441^000121812^",
		"a4410^00121812^",
		"a44100^0121812^",

		/* bad end rle values */
		"a4410000121813^",
		"a4410000121814^",

		/* bad '^' */
		"a4410000121812#",


		/* bad semi-double values */
		"a441000##",

		/* bad double values */
		"a441000<^",

		/* bad triple values */
		"a441000=^",
		"a441000=1^",

		/* bad quad values */
		"a441000>^",
		"a441000>1^",
		"a441000>11^",

		/* bad inflate distances */
		/* 0 */
		"a441000?04",
		/* 1 character before the beginning of the string */
		"a441000?84",
		/* non-value */
		"a441000?^",

		/* 0 */
		"a441000@004",
		/* 1 character before the beginning of the string */
		"a441000@084",
		/* non-value */
		"a441000@^",
		"a441000@0^",

		/* bad inflate lengths */
		/* 0 */
		"a441000?60",
		/* non-value */
		"a441000?6^",

		/* 0 */
		"a441000@060",
		/* non-value */
		"a441000@06^",


		/* extra characters at end */
		"a4410000121812^0",

		/* truncated */
		"",
		"a",
		"a4",
		"a44",
		"a441",
		"a4410",
		"a44100",
		"a441000",
		"a4410000",
		"a44100001",
		"a441000012",
		"a4410000121",
		"a44100001218",
		"a441000012181",
		"a4410000121812",

		NULL
	};


	/* CODE */
	printf( "  Testing bad formats...\n" ); fflush( stdout );

	while ( formats[ i ] != NULL )
	{
		if ( pintoImageDecodeString( formats[ i ], &image ) != PINTO_RC_ERROR_FORMAT_INVALID )
		{
			printf( "ERROR: this bad format succeeded!\n%s\n", formats[ i ] );
			TEST_ERR_IF( 1 );
		}

		printf( "." ); fflush( stdout );
		i += 1;
	}

	printf( "\n" );


	/* CLEANUP */
	cleanup:

	pintoImageFree( &image );

	return rc;
}

/******************************************************************************/
static int _testBadImages()
{
	/* DATA */
	int rc = 0;

	char *encoding = NULL;

	PintoImage badStackImage = { 1, 1, (unsigned char *)"\0\0\0\0" };
	PintoImage *badHeapImage = NULL;

	s32 i = 0;


	/* CODE */
	printf( "  Testing bad images...\n" ); fflush( stdout );

	/* test image width of 0 */
	badStackImage.width = 0;
	TEST_ERR_IF( pintoImageEncode( &badStackImage, &encoding ) != PINTO_RC_ERROR_IMAGE_BAD_SIZE );
	printf( "." ); fflush( stdout );
	badStackImage.width = 1;

	/* test image hieght of 0 */
	badStackImage.height = 0;
	TEST_ERR_IF( pintoImageEncode( &badStackImage, &encoding ) != PINTO_RC_ERROR_IMAGE_BAD_SIZE );
	printf( "." ); fflush( stdout );
	badStackImage.height = 1;

	/* test image width of PINTO_MAX_WIDTH + 1 */
	badStackImage.width = PINTO_MAX_WIDTH + 1;
	TEST_ERR_IF( pintoImageEncode( &badStackImage, &encoding ) != PINTO_RC_ERROR_IMAGE_BAD_SIZE );
	printf( "." ); fflush( stdout );
	badStackImage.width = 1;

	/* test image height of PINTO_MAX_HEIGHT + 1 */
	badStackImage.height = PINTO_MAX_HEIGHT + 1;
	TEST_ERR_IF( pintoImageEncode( &badStackImage, &encoding ) != PINTO_RC_ERROR_IMAGE_BAD_SIZE );
	printf( "." ); fflush( stdout );
	badStackImage.height = 1;

	/* test partial transparency at first pixel in image */
	TEST_ERR_IF( pintoImageInit( 32, 32, &badHeapImage ) != PINTO_RC_SUCCESS );
	badHeapImage->rgba[ 3 ] = 1;
	TEST_ERR_IF( pintoImageEncode( badHeapImage, &encoding ) != PINTO_RC_ERROR_IMAGE_PARTIAL_TRANSPARENCY );
	pintoImageFree( &badHeapImage );
	printf( "." ); fflush( stdout );

	/* test partial transparency at last pixel in image */
	TEST_ERR_IF( pintoImageInit( 32, 32, &badHeapImage ) != PINTO_RC_SUCCESS );
	badHeapImage->rgba[ ( 32 * 32 * 4 ) - 1 ] = 1;
	TEST_ERR_IF( pintoImageEncode( badHeapImage, &encoding ) != PINTO_RC_ERROR_IMAGE_PARTIAL_TRANSPARENCY );
	pintoImageFree( &badHeapImage );
	printf( "." ); fflush( stdout );

	/* test too many (64) colors */
	TEST_ERR_IF( pintoImageInit( 8, 8, &badHeapImage ) != PINTO_RC_SUCCESS );
	for ( i = 0; i < 64; i += 1 )
	{
		/* 
			this is confusing.
			- index of i * 4 because we're going to set the red value of each color, and
			  each color's rgba 4-byte part is at i * 4.
			- value of i * 4 to scale our 0-63 values to 0-252.
		*/
		badHeapImage->rgba[ i * 4 ] = PINTO_8_BITS_TO_6_BITS( i * 4 ); /* set red value */
		badHeapImage->rgba[ ( i * 4 ) + 3 ] = 255; /* make opaque */
	}
	TEST_ERR_IF( pintoImageEncode( badHeapImage, &encoding ) != PINTO_RC_ERROR_IMAGE_TOO_MANY_COLORS );
	pintoImageFree( &badHeapImage );
	printf( "." ); fflush( stdout );

	printf( "\n" );


	/* CLEANUP */
	cleanup:

	return rc;
}

/******************************************************************************/
static int _testGoodDeflate()
{
	/* DATA */
	int rc = 0;

	s32 test = 0;
	s32 i = 0;

	PintoText *textIn = NULL;
	PintoText *textOut = NULL;
	char *stringOut = NULL;

	struct inOut {
		char *in;
		char *out;
	}
	testCases[] = {
		{
			"",
			""
		},
		{
			"A",
			"A"
		},
		{
			"AB",
			"AB"
		},
		{
			"ABC",
			"ABC"
		},
		{
			"ABCD",
			"ABCD"
		},
		{
			"ABCDE",
			"ABCDE"
		},
		{
			"AA",
			"AA"
		},
		{
			"AAAA",
			"AAAA"
		},
		{
			"AAAAA",
			"A?14"
		},
		{
			"ABCDABCD",
			"ABCD?44"
		},
		{
			"ABCDx002ABCD",
			"ABCDx002?84"
		},
		{
			"ABCDx002x003x004x005x006x007x008x009x010x011x012x013x014x015x01ABCD",
			"ABCDx002x003x004x005x006x007x008x009x010x011x012x013x014x015x01?;4"
		},
		/* This shouldn't deflate because the encoding is 4 characters and the match
		   is 4 characters, so there would be no savings */
		{
			"ABCDx002x003x004x005x006x007x008x009x010x011x012x013x014x015x016ABCD",
			"ABCDx002x003x004x005x006x007x008x009x010x011x012x013x014x015x016ABCD"
		},
		{
			"ABCDx002x003x004x005x006x007x008x009x010x011x012x013x014x015x016xABCD",
			"ABCDx002x003x004x005x006x007x008x009x010x011x012x013x014x015x016xABCD"
		},
		{
			"ABCDE002x003x004x005x006x007x008x009x010x011x012x013x014x015x016ABCDE",
			"ABCDE002x003x004x005x006x007x008x009x010x011x012x013x014x015x016@105"
		},
		{
			"ABCDE002x003x004x005x006x007x008x009x010x011x012x013x014x015x016xABCDE",
			"ABCDE002x003x004x005x006x007x008x009x010x011x012x013x014x015x016x@115"
		},
		{
		/*	          1         2         3         4         5         6     */
		/*	 1234567890123456789012345678901234567890123456789012345678901234 */
			"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
			"A?1;" /* distance 1, length 63 */
		},
		{
			"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
			"A?1#0" /* distance 1, length 64 */
		},
		{
			"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
			"A?1#1" /* distance 1, length 65 */
		},
		{
			"1212121212121212",
			"12?2e"
		},
		{
			"1212121212121212121212121212121212121212121212121212121212121212",
			"12?2:"
		},
		{
			"12121212121212121212121212121212121212121212121212121212121212121",
			"12?2;"
		},
		{
			"121212121212121212121212121212121212121212121212121212121212121212",
			"12?2#0"
		},
		{
			"1212121212121212121212121212121212121212121212121212121212121212121",
			"12?2#1"
		},
		{
			"A001x002x003x004x005x006x007x008x009x010x011x012x013x014x015x01A001x002x003x004x005x006x007x008x009x010x011x012x013x014x015x01",
			"A001x002x003x004x005x006x007x008x009x010x011x012x013x014x015x01?;;",
		},
		{
			"A001x002x003x004x005x006x007x008x009x010x011x012x013x014x015x016A001x002x003x004x005x006x007x008x009x010x011x012x013x014x015x016",
			"A001x002x003x004x005x006x007x008x009x010x011x012x013x014x015x016@10#0",
		},
		{
			"A001x002x003x004x005x006x007x008x009x010x011x012x013x014x015x016xA001x002x003x004x005x006x007x008x009x010x011x012x013x014x015x016x",
			"A001x002x003x004x005x006x007x008x009x010x011x012x013x014x015x016x@11#1",
		},
		/* sentinel */
		{
			NULL,
			NULL
		}
	};


	/* CODE */
	printf( "  Testing good deflate...\n" ); fflush( stdout );

	while ( testCases[ test ].in != NULL )
	{
		/* create textIn */
		pintoTextFree( &textIn );
		TEST_ERR_IF( pintoTextInit( &textIn ) != PINTO_RC_SUCCESS );

		for ( i = 0; testCases[ test ].in[ i ] != '\0'; i += 1 )
		{
			TEST_ERR_IF( pintoTextAddChar( textIn, testCases[ test ].in[ i ] ) != PINTO_RC_SUCCESS );
		}

		/* deflate */
		TEST_ERR_IF( pintoSimpleDeflate( &textIn, &textOut ) != PINTO_RC_SUCCESS );

		/* test against expected result */
		if ( strcmp( textOut->string, testCases[ test ].out ) != 0 )
		{
			TEST_ERR_IF( 1 );
		}

		/* inflate */
		TEST_ERR_IF( pintoSimpleInflate( &textOut, &textIn ) != PINTO_RC_SUCCESS );

		/* test against original string */
		if ( strcmp( textIn->string, testCases[ test ].in ) != 0 )
		{
			TEST_ERR_IF( 1 );
		}

		/* next test */
		test += 1;

		printf( "." ); fflush( stdout );
	}

	/* Some test cases where we programatically create the input, because
	   we need long strings that would be harder to type out. */

	/* create textIn */
	pintoTextFree( &textIn );
	TEST_ERR_IF( pintoTextInit( &textIn ) != PINTO_RC_SUCCESS );

	TEST_ERR_IF( pintoTextAddChar( textIn, '1' ) != PINTO_RC_SUCCESS );
	TEST_ERR_IF( pintoTextAddChar( textIn, '2' ) != PINTO_RC_SUCCESS );
	TEST_ERR_IF( pintoTextAddChar( textIn, '3' ) != PINTO_RC_SUCCESS );
	TEST_ERR_IF( pintoTextAddChar( textIn, '4' ) != PINTO_RC_SUCCESS );
	TEST_ERR_IF( pintoTextAddChar( textIn, '5' ) != PINTO_RC_SUCCESS );

	for ( i = 0; i < 4090; i += 1 )
	{
		TEST_ERR_IF( pintoTextAddChar( textIn, 'A' ) != PINTO_RC_SUCCESS );
	}

	TEST_ERR_IF( pintoTextAddChar( textIn, '1' ) != PINTO_RC_SUCCESS );
	TEST_ERR_IF( pintoTextAddChar( textIn, '2' ) != PINTO_RC_SUCCESS );
	TEST_ERR_IF( pintoTextAddChar( textIn, '3' ) != PINTO_RC_SUCCESS );
	TEST_ERR_IF( pintoTextAddChar( textIn, '4' ) != PINTO_RC_SUCCESS );
	TEST_ERR_IF( pintoTextAddChar( textIn, '5' ) != PINTO_RC_SUCCESS );

	/* deflate */
	TEST_ERR_IF( pintoSimpleDeflate( &textIn, &textOut ) != PINTO_RC_SUCCESS );

	/* test against expected result */
	if ( strcmp( textOut->string, "12345A?1<;V@;;5" ) != 0 )
	{
		TEST_ERR_IF( 1 );
	}

	/* inflate */
	TEST_ERR_IF( pintoSimpleInflate( &textOut, &textIn ) != PINTO_RC_SUCCESS );

	TEST_ERR_IF( textIn->string[ 0 ] != '1' );
	TEST_ERR_IF( textIn->string[ 1 ] != '2' );
	TEST_ERR_IF( textIn->string[ 2 ] != '3' );
	TEST_ERR_IF( textIn->string[ 3 ] != '4' );
	TEST_ERR_IF( textIn->string[ 4 ] != '5' );

	/* test against original string */
	for ( i = 0; i < 4090; i += 1 )
	{
		TEST_ERR_IF( textIn->string[ i + 5 ] != 'A' );
	}

	TEST_ERR_IF( textIn->string[ 4095 ] != '1' );
	TEST_ERR_IF( textIn->string[ 4096 ] != '2' );
	TEST_ERR_IF( textIn->string[ 4097 ] != '3' );
	TEST_ERR_IF( textIn->string[ 4098 ] != '4' );
	TEST_ERR_IF( textIn->string[ 4099 ] != '5' );

	TEST_ERR_IF( textIn->string[ 4100 ] != '\0' );


	/* create textIn */
	pintoTextFree( &textIn );
	TEST_ERR_IF( pintoTextInit( &textIn ) != PINTO_RC_SUCCESS );

	for ( i = 0; i < (PINTO_MAX_WIDTH * PINTO_MAX_HEIGHT); i += 1 )
	{
		TEST_ERR_IF( pintoTextAddChar( textIn, 'A' ) != PINTO_RC_SUCCESS );
	}

	/* deflate */
	TEST_ERR_IF( pintoSimpleDeflate( &textIn, &textOut ) != PINTO_RC_SUCCESS );

	/* test against expected result */
	if ( strcmp( textOut->string, "A?1>;;;;" ) != 0 )
	{
		TEST_ERR_IF( 1 );
	}

	/* inflate */
	TEST_ERR_IF( pintoSimpleInflate( &textOut, &textIn ) != PINTO_RC_SUCCESS );

	/* test against original string */
	for ( i = 0; i < (PINTO_MAX_WIDTH * PINTO_MAX_HEIGHT); i += 1 )
	{
		TEST_ERR_IF( textIn->string[ i ] != 'A' );
	}
	TEST_ERR_IF( textIn->string[ i ] != '\0' );

	printf( "\n" );


	/* CLEANUP */
	cleanup:

	pintoTextFree( &textIn );
	pintoTextFree( &textOut );

	pintoHookFree( stringOut );
	stringOut = NULL;

	return rc;
}

/******************************************************************************/
static int _testBadDeflate()
{
	/* DATA */
	int rc = 0;

	s32 i = 0;

	PintoText *textIn = NULL;
	PintoText *textOut = NULL;
	const char *stringIn = NULL;
	char *stringOut = NULL;

	/* CODE */
	printf( "  Testing bad deflate...\n" ); fflush( stdout );

	/* *** */
	pintoTextFree( &textIn );
	TEST_ERR_IF( pintoTextInit( &textIn ) != PINTO_RC_SUCCESS );

	stringIn = "a?1>;;;;";
	for ( i = 0; stringIn[ i ] != '\0'; i += 1 )
	{
		TEST_ERR_IF( pintoTextAddChar( textIn, stringIn[ i ] ) != PINTO_RC_SUCCESS );
	}

	/* inflate */
	TEST_ERR_IF( pintoSimpleInflate( &textIn, &textOut ) != PINTO_RC_SUCCESS );

	pintoTextFree( &textOut );

	/* *** */
	pintoTextFree( &textIn );
	TEST_ERR_IF( pintoTextInit( &textIn ) != PINTO_RC_SUCCESS );

	stringIn = "aa?1>;;;;";
	for ( i = 0; stringIn[ i ] != '\0'; i += 1 )
	{
		TEST_ERR_IF( pintoTextAddChar( textIn, stringIn[ i ] ) != PINTO_RC_SUCCESS );
	}

	/* inflate */
	TEST_ERR_IF( pintoSimpleInflate( &textIn, &textOut ) != PINTO_RC_ERROR_FORMAT_TOO_LONG );

	printf( "." );

	printf( "\n" );


	/* CLEANUP */
	cleanup:

	pintoTextFree( &textIn );
	pintoTextFree( &textOut );

	pintoHookFree( stringOut );
	stringOut = NULL;

	return rc;
}

/******************************************************************************/
#define MARK \
	image->rgba[ ( y1 * image->width * 4 ) + ( x1 * 4 )     ] = red; \
	image->rgba[ ( y1 * image->width * 4 ) + ( x1 * 4 ) + 1 ] = green; \
	image->rgba[ ( y1 * image->width * 4 ) + ( x1 * 4 ) + 2 ] = blue; \
	image->rgba[ ( y1 * image->width * 4 ) + ( x1 * 4 ) + 3 ] = 255;

static int _testRandomImages(
	char *description,
	int numberOfImages,
	int maxNumberOfColors,
	int blackOnly,
	int simpleSizes,
	int square,
	int maxSize )
{
	/* DATA */
	int rc = 0;

	PintoImage *image = NULL;
	s32 width = 0;
	s32 height = 0;

	s32 i = 0;
	s32 j = 0;
	s32 k = 0;

	s32 colorAmount = 0;

	unsigned char red = 0;
	unsigned char green = 0;
	unsigned char blue = 0;

	s32 marksAmount = 0;

	s32 markKind = 0;

	s32 x1 = 0;
	s32 y1 = 0;
	s32 x2 = 0;
	s32 y2 = 0;

	s32 temp = 0;

	char *string = NULL;


	/* CODE */
	printf( "  %s\n", description ); fflush( stdout );

	for ( i = 0; i < numberOfImages; i += 1 )
	{
		/* generate random size */
		if ( simpleSizes )
		{
			width = ( (s32) 1 ) << ( ( rand() % 10 ) + 3 );
			height = ( (s32) 1 ) << ( ( rand() % 10 ) + 3 );
		}
		else
		{
			width  = ( rand() % maxSize ) + 1;
			height = ( rand() % maxSize ) + 1;
		}

		if ( square )
		{
			height = width;
		}

		/* initialize image */
		TEST_ERR_IF( pintoImageInit( width, height, &image ) != PINTO_RC_SUCCESS );

		/* how many colors do we want? */
		colorAmount = ( rand() % maxNumberOfColors ) + 1;

		/* foreach color */
		for ( j = 0; j < colorAmount; j += 1 )
		{
			/* generate color */
			/* we don't care if it ends up being the same color as a previous
			   color */
			if ( blackOnly == 1 )
			{
				red   = 0;
				green = 0;
				blue  = 0;
			}
			else
			{
				red   = rand() % 256;
				green = rand() % 256;
				blue  = rand() % 256;
			}

			/* how many marks are we going to make */
			marksAmount = ( rand() % 100 ) + 1;

			/* foreach mark */
			for ( k = 0; k < marksAmount; k += 1 )
			{
				/* what kind of mark are we going to make? */
				markKind = rand() % 4;

				/* single dot */
				if ( markKind == 0 )
				{
					x1 = ( rand() % image->width );
					y1 = ( rand() % image->height );

					MARK;
				}
				/* vertical line */
				else if ( markKind == 1 )
				{
					x1 = ( rand() % image->width );
					y1 = ( rand() % image->height );
					y2 = ( rand() % image->height );

					if ( y2 < y1 )
					{
						/* swap y2 and y1 */
						y2 = temp; y2 = y1; y1 = temp;
					}

					for ( ; y1 < y2; y1 += 1 )
					{
						MARK;
					}
				}
				/* horizontal line */
				else if ( markKind == 1 )
				{
					x1 = ( rand() % image->width );
					x2 = ( rand() % image->width );
					y1 = ( rand() % image->height );

					if ( x2 < x1 )
					{
						/* swap x2 and x1 */
						x2 = temp; x2 = x1; x1 = temp;
					}

					for ( ; x1 < x2; x1 += 1 )
					{
						MARK;
					}
				}
				/* rectangle */
				else if ( markKind == 1 )
				{
					x1 = ( rand() % image->width );
					x2 = ( rand() % image->width );
					y1 = ( rand() % image->height );
					y2 = ( rand() % image->height );

					if ( x2 < x1 )
					{
						/* swap x2 and x1 */
						x2 = temp; x2 = x1; x1 = temp;
					}

					if ( y2 < y1 )
					{
						/* swap y2 and y1 */
						y2 = temp; y2 = y1; y1 = temp;
					}

					for ( ; x1 < x2; x1 += 1 )
					{
						temp = y1;
						for ( ; y1 < y2; y1 += 1 )
						{
							MARK;
						}
						y1 = temp;
					}
				}
			}
		}

		/* verify image */
		TEST_ERR_IF( _testImageVerify( image ) != 0 );
 
		/* free */
		pintoHookFree( string );
		string = NULL;

		pintoImageFree( &image );

		/* *** */
		if ( i + 1 <= 100 )
		{
			printf( "\r%3d", i + 1 ); fflush( stdout );
		}
		else if ( ( i + 1 ) % 100 == 0 )
		{
			printf( "\r%3d", i + 1 ); fflush( stdout );
		}
	}


	printf( "\n" ); fflush( stdout );


	/* CLEANUP */
	cleanup:

	return rc;
}

/******************************************************************************/
static int _testImageVerify( PintoImage *image )
{
	/* DATA */
	int rc = 0;

	char *encoding = NULL;

	PintoImage *imageOut = NULL;

	s32 i = 0;

	unsigned char red = 0;
	unsigned char green = 0;
	unsigned char blue = 0;


	/* CODE */

	/* encode imageIn */
	TEST_ERR_IF( pintoImageEncode( image, &encoding ) != PINTO_RC_SUCCESS );

	/* decode what we just encoded to imageOut */
	TEST_ERR_IF( pintoImageDecodeString( encoding, &imageOut ) != PINTO_RC_SUCCESS );

	/* verify widths and heights */
	TEST_ERR_IF( image->width != imageOut->width );
	TEST_ERR_IF( image->height != imageOut->height );

	/* verify image data.
	   NOTE: can't just use memcmp because if the image has 0 transparency, it may
	         still have values for it's red, green, or blue. On decoding, we make
	         all fully transparent pixels have 0 for red, green, and blue */
	for ( i = 0; i < (image->width * image->height * 4); i += 4 )
	{
		if ( image->rgba[ i + 3 ] == 0 )
		{
			TEST_ERR_IF( imageOut->rgba[ i + 3 ] != 0 );

			TEST_ERR_IF( imageOut->rgba[ i     ] != 0 );
			TEST_ERR_IF( imageOut->rgba[ i + 1 ] != 0 );
			TEST_ERR_IF( imageOut->rgba[ i + 2 ] != 0 );
		}
		else
		{
			/* make sure it's fully opaque */
			TEST_ERR_IF( imageOut->rgba[ i + 3 ] != 255 );

			/* compare red, green, and blue */
			red   = image->rgba[ i     ];
			green = image->rgba[ i + 1 ];
			blue  = image->rgba[ i + 2 ];

			red   = PINTO_8_BITS_TO_6_BITS( red   );
			green = PINTO_8_BITS_TO_6_BITS( green );
			blue  = PINTO_8_BITS_TO_6_BITS( blue  );

			TEST_ERR_IF( imageOut->rgba[ i     ] != red );
			TEST_ERR_IF( imageOut->rgba[ i + 1 ] != green );
			TEST_ERR_IF( imageOut->rgba[ i + 2 ] != blue );
		}
	}


	/* CLEANUP */
	cleanup:

	pintoHookFree( encoding );
	encoding = NULL;

	pintoImageFree( &imageOut );

	return rc;
}

