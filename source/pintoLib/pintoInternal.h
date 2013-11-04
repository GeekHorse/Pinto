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
#ifndef pintoInternal_H
#define pintoInternal_H

/******************************************************************************/
#include <stdio.h> /* for printf in ERR */
#include <stdlib.h> /* for NULL */

/******************************************************************************/
#if ( PRINT_ERR == 1 )
#define ERR_IF( cond, error_to_return ) if ( (cond) ) { fprintf( stderr, "ERR: %s %d\n", __FILE__, __LINE__ ); fflush( stderr ); rc = error_to_return; goto cleanup; }
#define ERR_IF_PASSTHROUGH if ( rc != PINTO_RC_SUCCESS ) { fprintf( stderr, "ERR: %s %d\n", __FILE__, __LINE__ ); fflush( stderr ); goto cleanup; }
#else
#define ERR_IF( cond, error_to_return ) if ( (cond) ) { rc = error_to_return; goto cleanup; }
#define ERR_IF_PASSTHROUGH if ( rc != PINTO_RC_SUCCESS ) { goto cleanup; }
#endif

/******************************************************************************/
#ifndef TEST_PRECOND
#define TEST_PRECOND 1
#endif

#if ( TEST_PRECOND == 1 )

#if ( PRINT_ERR == 1 )
#define PRECOND_ERR_IF( cond ) if ( (cond) ) { fprintf( stderr, "PRECOND_ERR: %s %d\n", __FILE__, __LINE__ ); fflush( stderr ); return PINTO_RC_ERROR_PRECOND; }
#else
#define PRECOND_ERR_IF( cond ) if ( (cond) ) { return PINTO_RC_ERROR_PRECOND; }
#endif

#else

#define PRECOND_ERR_IF( cond )

#endif

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
	POINTER = ( POINTER_TYPE * ) pintoHookMalloc( sizeof( POINTER_TYPE ) * (SIZE) ); \
	ERR_IF( POINTER == NULL, PINTO_RC_ERROR_MEMORY_ALLOCATION_FAILED );

/******************************************************************************/
#define PINTO_CALLOC( POINTER, POINTER_TYPE, SIZE ) \
	POINTER = ( POINTER_TYPE * ) pintoHookCalloc( SIZE, sizeof( POINTER_TYPE ) ); \
	ERR_IF( POINTER == NULL, PINTO_RC_ERROR_MEMORY_ALLOCATION_FAILED );

/******************************************************************************/
#define PINTO_REALLOC( POINTER2, POINTER1, POINTER_TYPE, SIZE ) \
	POINTER2 = ( POINTER_TYPE * ) pintoHookRealloc( POINTER1, sizeof( POINTER_TYPE ) * (SIZE) ); \
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
#endif

