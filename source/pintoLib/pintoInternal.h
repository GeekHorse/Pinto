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
#ifndef pintoInternal_H
#define pintoInternal_H

/******************************************************************************/
#include <stdio.h> /* for printf, fprintf, fflush */
#include <stdlib.h> /* for NULL */

#include "pinto.h"

/******************************************************************************/
#define ERR_IF( cond, error_to_return ) \
	if ( (cond) ) \
	{ \
		PINTO_HOOK_LOG( PINTO_LIBRARY_NUMBER, PINTO_FILE_NUMBER, __LINE__, error_to_return, 0, 0, 0 ); \
		rc = error_to_return; \
		goto cleanup; \
	}

#define ERR_IF_1( cond, error_to_return, a ) \
	if ( (cond) ) \
	{ \
		PINTO_HOOK_LOG( PINTO_LIBRARY_NUMBER, PINTO_FILE_NUMBER, __LINE__, error_to_return, a, 0, 0 ); \
		rc = error_to_return; \
		goto cleanup; \
	}

#define ERR_IF_2( cond, error_to_return, a, b ) \
	if ( (cond) ) \
	{ \
		PINTO_HOOK_LOG( PINTO_LIBRARY_NUMBER, PINTO_FILE_NUMBER, __LINE__, error_to_return, a, b, 0 ); \
		rc = error_to_return; \
		goto cleanup; \
	}

#define ERR_IF_3( cond, error_to_return, a, b, c ) \
	if ( (cond) ) \
	{ \
		PINTO_HOOK_LOG( PINTO_LIBRARY_NUMBER, PINTO_FILE_NUMBER, __LINE__, error_to_return, a, b, c ); \
		rc = error_to_return; \
		goto cleanup; \
	}

#define ERR_IF_PASSTHROUGH \
	ERR_IF( rc != PINTO_RC_SUCCESS, rc );

/******************************************************************************/
#ifdef BE_PARANOID
#define PARANOID_ERR_IF( cond ) if ( (cond) ) { fprintf( stderr, "PARANOID ERROR! %s %d\n", __FILE__, __LINE__ ); fflush( stderr ); exit(-1); }
#else
#define PARANOID_ERR_IF( cond )
#endif

/******************************************************************************/
/* only for debugging */
#define dline printf( "dline:" __FILE__ ":%d\n", __LINE__ ); fflush( stdout );

/******************************************************************************/
#define PINTO_MALLOC( POINTER, POINTER_TYPE, SIZE ) \
	POINTER = ( POINTER_TYPE * ) PINTO_HOOK_MALLOC( sizeof( POINTER_TYPE ) * (SIZE) ); \
	ERR_IF( POINTER == NULL, PINTO_RC_ERROR_MEMORY_ALLOCATION_FAILED );

/******************************************************************************/
#define PINTO_CALLOC( POINTER, POINTER_TYPE, SIZE ) \
	POINTER = ( POINTER_TYPE * ) PINTO_HOOK_CALLOC( SIZE, sizeof( POINTER_TYPE ) ); \
	ERR_IF( POINTER == NULL, PINTO_RC_ERROR_MEMORY_ALLOCATION_FAILED );

/******************************************************************************/
#define PINTO_REALLOC( POINTER2, POINTER1, POINTER_TYPE, SIZE ) \
	POINTER2 = ( POINTER_TYPE * ) PINTO_HOOK_REALLOC( POINTER1, sizeof( POINTER_TYPE ) * (SIZE) ); \
	ERR_IF( POINTER2 == NULL, PINTO_RC_ERROR_MEMORY_ALLOCATION_FAILED );



/******************************************************************************/
/* Pinto's colors are reduced from 8-bits to 6-bits.
   However, we don't want the 2 least significant bits to always be 0, because
   then we couldn't have a pixel that's fully white.
   So we make the 2 least significant bits the same as the 2 most significant
   bits. This way we can have fully white and fully black, and the pixels in
   the middle have their 2 least significant bits scaled appropriately too.
*/
#define PINTO_8_BITS_TO_6_BITS( x ) ( ( (x) & 0xFC ) | ( (x) >> 6 ) )

#define PINTO_6_BITS_TO_8_BITS( x ) ( ( (x) << 2   ) | ( (x) >> 4 ) )

/******************************************************************************/
extern s32 PINTO_TEXT_SIZE_GROWTH;

/******************************************************************************/
/*! Text object */
struct PintoText_STRUCT
{
	/*! Internal string. */
	char *string;
	/*! Allocated size of string */
	s32 allocedSize;
	/*! How many characters we've added to the string. Does not include the '\0'
	    at the end of the string */
	s32 usedSize;
	/*! Index in string where we will read next */
	s32 index;
};

/******************************************************************************/
s32 pintoTextAtEnd( PintoText *text );
PINTO_RC pintoTextPeekChar( PintoText *text, char *ch );
PINTO_RC pintoTextGetChar( PintoText *text, char *ch );

PINTO_RC pintoTextAddValue( PintoText *text, s32 value );

PINTO_RC pintoTextGetValue( PintoText *text, s32 *value );
PINTO_RC pintoTextUpdateValue( PintoText *text, s32 *value );

PINTO_RC pintoTextInflateHelper( PintoText *text, s32 distance, s32 length );

void pintoTextFreeAndGetString( PintoText **text_F, char **string_A );

PINTO_RC pintoSimpleDeflate( PintoText **textToDeflate_F, PintoText **text_A );
PINTO_RC pintoSimpleInflate( PintoText **textToInflate_F, PintoText **text_A );

/******************************************************************************/
#ifdef DEBUG

	extern void *pintoHookMalloc( size_t size );
	#ifndef PINTO_HOOK_MALLOC
	#define PINTO_HOOK_MALLOC pintoHookMalloc
	#endif

	extern void *pintoHookCalloc( size_t nmemb, size_t size );
	#ifndef PINTO_HOOK_CALLOC
	#define PINTO_HOOK_CALLOC pintoHookCalloc
	#endif

	extern void *pintoHookRealloc( void *ptr, size_t size );
	#ifndef PINTO_HOOK_REALLOC
	#define PINTO_HOOK_REALLOC pintoHookRealloc
	#endif

	extern void pintoHookFree( void *ptr );
	#ifndef PINTO_HOOK_FREE
	#define PINTO_HOOK_FREE pintoHookFree
	#endif

#else

	#ifndef PINTO_HOOK_MALLOC
	#define PINTO_HOOK_MALLOC malloc
	#endif

	#ifndef PINTO_HOOK_CALLOC
	#define PINTO_HOOK_CALLOC calloc
	#endif

	#ifndef PINTO_HOOK_REALLOC
	#define PINTO_HOOK_REALLOC realloc
	#endif

	#ifndef PINTO_HOOK_FREE
	#define PINTO_HOOK_FREE free
	#endif

#endif

/******************************************************************************/
#ifdef PINTO_ENABLE_LOGGING

	extern void pintoHookLog( s32 library, s32 file, s32 line, s32 rc, s32 a, s32 b, s32 c );
	#ifndef PINTO_HOOK_LOG
	#define PINTO_HOOK_LOG( p, f, l, r, a, b, c ) pintoHookLog( p, f, l, r, a, b, c )
	#endif

#else

	#ifndef PINTO_HOOK_LOG
	#define PINTO_HOOK_LOG( p, f, l, r, a, b, c )
	#endif

#endif

/******************************************************************************/
#endif

