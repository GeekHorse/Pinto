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
#ifndef pinto_H
#define pinto_H

/******************************************************************************/
#include <stdlib.h> /* for size_t, in memory hook functions */

/******************************************************************************/
#define PINTO_NAME "Pinto"

#define PINTO_COPYRIGHT "Copyright (C) 2012-2013 Jeremiah Martell"

#define PINTO_VERSION_STRING "1.0"
#define PINTO_VERSION 1000
#define PINTO_VERSION_MAJOR  ( PINTO_VERSION / 1000 )
#define PINTO_VERSION_MINOR  ( ( PINTO_VERSION / 100 ) % 10 )
#define PINTO_VERSION_BUGFIX ( PINTO_VERSION % 100 )

/******************************************************************************/
typedef enum
{
	PINTO_RC_SUCCESS = 0,

	PINTO_RC_ERROR_PRECOND = 1,
	PINTO_RC_ERROR_MEMORY_ALLOCATION_FAILED = 2,
	PINTO_RC_ERROR_STANDARD_LIBRARY_ERROR = 3,

	PINTO_RC_ERROR_IMAGE_BAD_SIZE = 4,
	PINTO_RC_ERROR_IMAGE_TOO_MANY_COLORS = 5,
	PINTO_RC_ERROR_IMAGE_PARTIAL_TRANSPARENCY = 6,
	PINTO_RC_ERROR_FORMAT_INVALID = 7,
	PINTO_RC_ERROR_FORMAT_TOO_LONG = 8,

	/* This must be the same value as the previous enum */
	PINTO_RC_MAX = 8,

	/* Never used, but forces the enum type signed, so we
	   can be more defensive in pintoRCToString */
	PINTO_RC_ERROR_UNKNOWN = -1
} PINTO_RC;

/******************************************************************************/
/* NOTE: You can not simply modify these if you want pintoLib to handle more
         colors, or larger images. These values are derived from the design of
         the format and can not be changed without careful consideration and
         modification of the encoding and decoding algorithm and format.
*/
#define PINTO_MAX_COLORS 63
#define PINTO_MAX_WIDTH  4096
#define PINTO_MAX_HEIGHT 4096

/******************************************************************************/
/* This defines s32 as a signed 32-bit integer */
#define s32 int

/******************************************************************************/
/*! Image structure */
typedef struct
{
	/*! Width */
	s32 width;
	/*! Height */
	s32 height;
	/*! Red, Green, Blue, and Alpha data. Size is (width * height * 4) */
	unsigned char *rgba;
} PintoImage;

typedef struct PintoText_STRUCT PintoText;

/******************************************************************************/
/* pintoHooks.c */
extern void *(*pintoHookMalloc)( size_t size );
extern void *(*pintoHookCalloc)( size_t nmemb, size_t size );
extern void *(*pintoHookRealloc)( void *ptr, size_t size );
extern void (*pintoHookFree)( void *ptr );

/******************************************************************************/
/* pinto.c */
PINTO_RC pintoImageEncode( const PintoImage *image, char **string_A );

PINTO_RC pintoImageDecodeString( const char *string, PintoImage **image_A );
PINTO_RC pintoImageDecodeText( PintoText **text_F, PintoImage **image_A );

PINTO_RC pintoImageInit( s32 width, s32 height, PintoImage **image_A );
void pintoImageFree( PintoImage **image_F );

const char *pintoRCToString( PINTO_RC rc );

/******************************************************************************/
/* pintoText.c */
PINTO_RC pintoTextInit( PintoText **text_A );
void pintoTextFree( PintoText **text_F );

PINTO_RC pintoTextAddChar( PintoText *text, char ch );

/******************************************************************************/
#endif

