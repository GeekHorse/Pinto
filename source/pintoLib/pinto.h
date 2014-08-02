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
#ifndef pinto_H
#define pinto_H

/******************************************************************************/
#include <stdlib.h> /* for size_t, in memory hook functions */

/******************************************************************************/
#define PINTO_NAME "Pinto"

#define PINTO_COPYRIGHT "Copyright (C) 2012-2014 Jeremiah Martell"

#define PINTO_VERSION_STRING "1.0.03-wip"
#define PINTO_VERSION 10030
#define PINTO_VERSION_MAJOR       ( PINTO_VERSION / 10000 )
#define PINTO_VERSION_MINOR       ( ( PINTO_VERSION / 1000 ) % 10 )
#define PINTO_VERSION_SUBMINOR    ( ( PINTO_VERSION / 10 ) % 100 )
#define PINTO_VERSION_FINAL       ( PINTO_VERSION % 10 )

/******************************************************************************/
/* This defines s32 as a signed 32-bit integer */
#ifndef s32
#define s32 int
#endif
/* This defines u8 as an unsigned 8-bit integer */
#ifndef u8
#define u8  unsigned char
#endif

/******************************************************************************/
#define PINTO_RC s32

/******************************************************************************/
#define PINTO_LIBRARY_NUMBER 1000

/******************************************************************************/
/* standard rc values */
#define PINTO_RC_SUCCESS 0

#define PINTO_RC_ERROR_PRECOND                  1
#define PINTO_RC_ERROR_MEMORY_ALLOCATION_FAILED 2
#define PINTO_RC_ERROR_STANDARD_LIBRARY_ERROR   3

/* This must be kept in sync with the above defines */
#define PINTO_RC_STANDARD_ERRORS_MAX            3

/* Pinto specific rc values */
#define PINTO_RC_ERROR_IMAGE_BAD_SIZE             1001
#define PINTO_RC_ERROR_IMAGE_TOO_MANY_COLORS      1002
#define PINTO_RC_ERROR_IMAGE_PARTIAL_TRANSPARENCY 1003
#define PINTO_RC_ERROR_IMAGE_TOO_SMALL            1004
#define PINTO_RC_ERROR_FORMAT_INVALID             1005
#define PINTO_RC_ERROR_FORMAT_TOO_LONG            1006

/* These must be kept in sync with the above defines */
#define PINTO_RC_PINTO_ERRORS_MIN                 1001
#define PINTO_RC_PINTO_ERRORS_MAX                 1006


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
/*! Image structure */
typedef struct
{
	/*! Width */
	s32 width;
	/*! Height */
	s32 height;
	/*! Red, Green, Blue, and Alpha data. Size is (width * height * 4) */
	u8 *rgba;
} PintoImage;

typedef struct PintoText_STRUCT PintoText;

/******************************************************************************/
/* pintoHooks.c */
/* These function pointers will be used by Pinto. You can use them to plug
   Pinto into your own memory management system. */
extern void *(*pintoHookMalloc)( size_t size );
extern void *(*pintoHookCalloc)( size_t nmemb, size_t size );
extern void *(*pintoHookRealloc)( void *ptr, size_t size );
extern void (*pintoHookFree)( void *ptr );

/* This function pointer is used by Pinto to log errors. Not really useful for
   end-users of the library, but useful for developers. The default function
   prints to stderr, but you can change this to plug Pinto into your own
   logging system. */
extern void (*pintoHookLog)( s32 library, s32 file, s32 line, s32 rc, s32 a, s32 b, s32 c );

/******************************************************************************/
/* pinto.c */
PINTO_RC pintoImageEncode( const PintoImage *image, char **string_A );

PINTO_RC pintoImageDecodeString( const char *string, PintoImage **image_A );
PINTO_RC pintoImageDecodeText( PintoText **text_F, PintoImage **image_A );

PINTO_RC pintoImageInit( s32 width, s32 height, PintoImage **image_A );
void pintoImageFree( PintoImage **image_F );

PINTO_RC pintoImageDownsize( PintoImage *imageIn, PintoImage **imageOut_A );

const char *pintoRCToString( PINTO_RC rc );

/******************************************************************************/
/* pintoText.c */
PINTO_RC pintoTextInit( PintoText **text_A );
void pintoTextFree( PintoText **text_F );

PINTO_RC pintoTextAddChar( PintoText *text, char ch );

/******************************************************************************/
#endif

