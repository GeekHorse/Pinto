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
/*!
	\file
	Dynamic text structure functions.
*/
#undef PINTO_FILE_NUMBER
#define PINTO_FILE_NUMBER 2

/******************************************************************************/
#include "pinto.h"
#include "pintoInternal.h"

/******************************************************************************/
/* How much increase the size of our buffer, via realloc, if we need more */
s32 PINTO_TEXT_SIZE_GROWTH = 1024;

/******************************************************************************/
const char valueToChar[ 64 ] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
	'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
	'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D',
	'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
	'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', ':', ';' };

/******************************************************************************/
/*!
	\brief Initialize a text object.
	\param[out] text_A The new text object.
		Will be allocated. Caller is responsible for freeing by passing the text
		object to pintoTextFree().
	\return PINTO_RC
*/
PINTO_RC pintoTextInit( PintoText **text_A )
{
	/* DATA */
	PINTO_RC rc = PINTO_RC_SUCCESS;

	PintoText *newText = NULL;


	/* CODE */
	ERR_IF( text_A == NULL, PINTO_RC_ERROR_PRECOND );
	ERR_IF( (*text_A) != NULL, PINTO_RC_ERROR_PRECOND );

	PINTO_CALLOC( newText, PintoText, 1 );

	PINTO_CALLOC( newText->string, char, PINTO_TEXT_SIZE_GROWTH );
	newText->allocedSize = PINTO_TEXT_SIZE_GROWTH;

	newText->usedSize = 0;
	newText->index = 0;

	/* give back */
	(*text_A) = newText;
	newText = NULL;


	/* CLEANUP */
	cleanup:

	if ( newText != NULL )
	{
		PINTO_HOOK_FREE( newText->string );
		newText->string = NULL;

		PINTO_HOOK_FREE( newText );
		newText = NULL;
	}

	return rc;
}

/******************************************************************************/
/*!
	\brief Frees a text object.
	\param[in] text_F Text object to be freed. Will be freed with
		PINTO_HOOK_FREE(). On return, text_F will be NULL.
	\return void
*/
void pintoTextFree( PintoText **text_F )
{
	/* CODE */
	if ( text_F == NULL || (*text_F) == NULL )
	{
		return;
	}

	PINTO_HOOK_FREE( (*text_F)->string );
	(*text_F)->string = NULL;

	PINTO_HOOK_FREE( (*text_F) );
	(*text_F) = NULL;

	return;
}

/******************************************************************************/
/*!
	\brief Adds a character to the text object.
	\param[in] text Text object to add to.
	\param[in] ch Character to add.
	\return PINTO_RC

	Will not add a character that isn't used in encoding, which makes the rest
	of the code simpler as we don't have to deal with invalid characters.
*/
PINTO_RC pintoTextAddChar( PintoText *text, char ch )
{
	/* DATA */
	PINTO_RC rc = PINTO_RC_SUCCESS;

	char *reallocedText = NULL;


	/* CODE */
	ERR_IF( text == NULL, PINTO_RC_ERROR_PRECOND );

	/* only add characters that are valid.
	   skipping invalid characters allows clients to wordwrap
	   the text, or add tabs or spaces. */
	if ( ch < '#' || ch > 'z' || ch == '[' || ch == '\\' || ch == ']' || ch == '_' || ch == '`' )
	{
		/* not a valid character, don't add */
		goto cleanup;
	}

	/* No text should ever reach this size, so we put this in to prevent
	   the buffer from growing too big.
	   The fear is a malicious person crafting a small pinto encoding, that will
	   deflate to take up huge amounts of ram ... if you consider more than 16MB to be huge :)
	   That probably won't happen in this function, but we still enforce the constraint */
	ERR_IF( text->usedSize + 1 > ( PINTO_MAX_WIDTH * PINTO_MAX_HEIGHT ), PINTO_RC_ERROR_FORMAT_TOO_LONG );

	/* do we need to resize our buffer? */
	if ( text->usedSize + 1 >= text->allocedSize )
	{
		PINTO_REALLOC( reallocedText, text->string, char, text->allocedSize + PINTO_TEXT_SIZE_GROWTH );

		text->allocedSize += PINTO_TEXT_SIZE_GROWTH;
		text->string = reallocedText;
	}

	/* add */
	text->string[ text->usedSize ] = ch;
	text->usedSize += 1;
	text->string[ text->usedSize ] = '\0';


	/* CLEANUP */
	cleanup:

	return rc;
}

/******************************************************************************/
/*!
	\brief Used to see if we're at the end of the text.
	\param[in] text Text.
	\return s32 True if we're at the end of the text.
*/
s32 pintoTextAtEnd( PintoText *text )
{
	/* CODE */
	PARANOID_ERR_IF( text == NULL );

	return text->index == text->usedSize;
}

/******************************************************************************/
/*!
	\brief Peeks at next character in text.
	\param[in] text Text.
	\param[out] ch Next character in text.
	\return PINTO_RC
*/
PINTO_RC pintoTextPeekChar( PintoText *text, char *ch )
{
	/* DATA */
	PINTO_RC rc = PINTO_RC_SUCCESS;


	/* CODE */
	PARANOID_ERR_IF( text == NULL );
	PARANOID_ERR_IF( ch == NULL );

	ERR_IF( text->index == text->usedSize, PINTO_RC_ERROR_FORMAT_INVALID );

	(*ch) = text->string[ text->index ];


	/* CLEANUP */
	cleanup:

	return rc;
}

/******************************************************************************/
/*!
	\brief Gets the next character in text.
	\param[in] text Text.
	\param[out] ch Next character in text.
	\return PINTO_RC
*/
PINTO_RC pintoTextGetChar( PintoText *text, char *ch )
{
	/* DATA */
	PINTO_RC rc = PINTO_RC_SUCCESS;


	/* CODE */
	PARANOID_ERR_IF( text == NULL );
	PARANOID_ERR_IF( ch == NULL );

	ERR_IF( text->index == text->usedSize, PINTO_RC_ERROR_FORMAT_INVALID );

	(*ch) = text->string[ text->index ];

	text->index += 1;


	/* CLEANUP */
	cleanup:

	return rc;
}

/******************************************************************************/
/*!
	\brief Adds an encoded value to text.
	\param[in] text Text.
	\param[in] value Value to add.
	\return PINTO_RC

	If the value is within 0-63, it is encoded as a single character. See
	valueToChar[] for the mapping.

	If the value is higher, then it's encoding like the following:

	     0 -       63  V
	    64 -      127  #V
	   128 -      191  $V
	   192 -      255  %V
	   256 -      319  'V
	   320 -      383  (V
	   384 -      447  )V
	   448 -      511  *V
	   512 -      575  +V
	   576 -      639  ,V
	   640 -      703  -V
	   704 -      767  .V
	   768 -      831  /V
	   832 -     4095  <VV
	  4096 -   262143  =VVV
	262144 - 16777215  >VVVV

	Values 0-63, 832-4095, 4096-262143, and 262144-16777215 are encoded in
	a straight-forward manner. You decode the value as a base-64 number.
	A     is A
	<AB   is (A * 64) + B
	=ABC  is (A * 64 * 64) + (B * 64) + (C)
	>ABCD is (A * 64 * 64 * 64) + (B * 64 * 64) + (C * 64) + D

	Values 64-831 are encoded differently.
	#A    is 64 + A
	$A    is 128 + A
	%A    is 192 + A
	...and so on.
	This is an enhancement so those values only use 2 characters instead of
	the straight-forward 3 character <AB format.

	Examples:
	Value 0 is encoded as "0"
	Value 10 is encoded as "a"
	Value 35 is encoded as "z"
	Value 36 is encoded as "A"
	Value 61 is encoded as "Z"
	Value 62 is encoded as ":"
	Value 63 is encoded as ";"
	Value 64 is encoded as "#0"
	Value 66 is encoded as "#2"
	Value 127 is encoded as "#;"
	Value 128 is encoded as "$0"
	Value 832 is encoded as "<d0"
	Value 833 is encoded as "<d1"
*/
PINTO_RC pintoTextAddValue( PintoText *text, s32 value )
{
	/* DATA */
	PINTO_RC rc = PINTO_RC_SUCCESS;


	/* CODE */
	PARANOID_ERR_IF( text == NULL );
	PARANOID_ERR_IF( value < 0 );
	PARANOID_ERR_IF( value >= (64 * 64 * 64 * 64) );

	if ( value < 64 )
	{
		rc = pintoTextAddChar( text, valueToChar[ value % 64 ] );
		ERR_IF_PASSTHROUGH;
	}
	else if ( value < (64 * 14) )
	{
		rc = pintoTextAddChar( text, '\"' + ( value / 64 ) );
		ERR_IF_PASSTHROUGH;

		rc = pintoTextAddChar( text, valueToChar[ value % 64 ] );
		ERR_IF_PASSTHROUGH;
	}
	else if ( value < (64 * 64) )
	{
		rc = pintoTextAddChar( text, '<' );
		ERR_IF_PASSTHROUGH;

		rc = pintoTextAddChar( text, valueToChar[ ( (value) / 64 ) % 64 ] );
		ERR_IF_PASSTHROUGH;

		rc = pintoTextAddChar( text, valueToChar[   (value)        % 64 ] );
		ERR_IF_PASSTHROUGH;
	}
	else if ( value < (64 * 64 * 64) ) 
	{
		rc = pintoTextAddChar( text, '=' );
		ERR_IF_PASSTHROUGH;

		rc = pintoTextAddChar( text, valueToChar[ ( (value) / (64 * 64) ) % 64 ] );
		ERR_IF_PASSTHROUGH;

		rc = pintoTextAddChar( text, valueToChar[ ( (value) / (64     ) ) % 64 ] );
		ERR_IF_PASSTHROUGH;

		rc = pintoTextAddChar( text, valueToChar[   (value)               % 64 ] );
		ERR_IF_PASSTHROUGH;
	}
	else
	{
		PARANOID_ERR_IF( value >= (64 * 64 * 64 * 64) );

		rc = pintoTextAddChar( text, '>' );
		ERR_IF_PASSTHROUGH;

		rc = pintoTextAddChar( text, valueToChar[ ( (value) / (64 * 64 * 64) ) % 64 ] );
		ERR_IF_PASSTHROUGH;

		rc = pintoTextAddChar( text, valueToChar[ ( (value) / (64 * 64     ) ) % 64 ] );
		ERR_IF_PASSTHROUGH;

		rc = pintoTextAddChar( text, valueToChar[ ( (value) / (64          ) ) % 64 ] );
		ERR_IF_PASSTHROUGH;

		rc = pintoTextAddChar( text, valueToChar[   (value)                    % 64 ] );
		ERR_IF_PASSTHROUGH;
	}


	/* CLEANUP */
	cleanup:

	return rc;
}

/******************************************************************************/
/*!
	\brief Gets an encoded value from text.
	\param[in] text Text.
	\param[in,out] value The encoded value.
	\return PINTO_RC

	Decodes an encoded value. See pintoTextAddValue() for the encoding.
*/
PINTO_RC pintoTextGetValue( PintoText *text, s32 *value )
{
	/* DATA */
	PINTO_RC rc = PINTO_RC_SUCCESS;

	char ch = 0;


	/* CODE */
	PARANOID_ERR_IF( text == NULL );
	PARANOID_ERR_IF( value == NULL );

	ERR_IF( text->index == text->usedSize, PINTO_RC_ERROR_FORMAT_INVALID );

	(*value) = 0;

	ch = text->string[ text->index ];

	/* No valid character has a lower value than '#', so we don't have to test
	   for "ch >= '#'". Just testing for "ch <= '/'" is enough. */
	if ( ch <= '/' ) 
	{
		PARANOID_ERR_IF( ch < '#' );

		text->index += 1;

		(*value) = ch - '\"';

		rc = pintoTextUpdateValue( text, value );
		ERR_IF_PASSTHROUGH;
	}
	else if ( ch == '<' )
	{
		text->index += 1;

		rc = pintoTextUpdateValue( text, value );
		ERR_IF_PASSTHROUGH;

		rc = pintoTextUpdateValue( text, value );
		ERR_IF_PASSTHROUGH;
	}
	else if ( ch == '=' )
	{
		text->index += 1;

		rc = pintoTextUpdateValue( text, value );
		ERR_IF_PASSTHROUGH;

		rc = pintoTextUpdateValue( text, value );
		ERR_IF_PASSTHROUGH;

		rc = pintoTextUpdateValue( text, value );
		ERR_IF_PASSTHROUGH;
	}
	else if ( ch == '>' )
	{
		text->index += 1;

		rc = pintoTextUpdateValue( text, value );
		ERR_IF_PASSTHROUGH;

		rc = pintoTextUpdateValue( text, value );
		ERR_IF_PASSTHROUGH;

		rc = pintoTextUpdateValue( text, value );
		ERR_IF_PASSTHROUGH;

		rc = pintoTextUpdateValue( text, value );
		ERR_IF_PASSTHROUGH;
	}
	else
	{
		rc = pintoTextUpdateValue( text, value );
		ERR_IF_PASSTHROUGH;
	}


	/* CLEANUP */
	cleanup:

	return rc;
}

/******************************************************************************/
/*!
	\brief Reads next character in text, and uses it to update value.
	\param[in] text Text.
	\param[in,out] value Value to be updated.
	\return PINTO_RC

	Value parameter is first multiplied by 64. The next character in text is
	read and decoded to a number in 0-63, then added to value.

	Basically this function does:
	value = (value * 64) + nextCharacterValue;

	This is used for decoding numbers that were encoded with multiple
	characters.

	See pintoTextGetValue() for more details.
*/
PINTO_RC pintoTextUpdateValue( PintoText *text, s32 *value )
{
	/* DATA */
	PINTO_RC rc = PINTO_RC_SUCCESS;

	char ch = 0;


	/* CODE */
	PARANOID_ERR_IF( text == NULL );
	PARANOID_ERR_IF( value == NULL );

	ERR_IF( text->index == text->usedSize, PINTO_RC_ERROR_FORMAT_INVALID );

	(*value) *= 64;

	ch = text->string[ text->index ];

	if ( ch >= '0' && ch <= '9' )
	{
		(*value) += ( ch - '0' );
	}
	else if ( ch >= 'a' )
	{
		/* No valid character has a higher value
		   than 'z', so we don't have to test for
		   "ch <= 'z'". Just testing for "ch >= 'a'"
		   is enough. */
		PARANOID_ERR_IF( ch > 'z' );

		(*value) += 10 + ( ch - 'a' );
	}
	else if ( ch >= 'A' && ch <= 'Z' )
	{
		(*value) += 36 + ( ch - 'A' );
	}
	else if ( ch == ':' )
	{
		(*value) += 62;
	}
	else if ( ch == ';' )
	{
		(*value) += 63;
	}
	else
	{
		ERR_IF( 1, PINTO_RC_ERROR_FORMAT_INVALID );
	}

	text->index += 1;


	/* CLEANUP */
	cleanup:

	return rc;
}

/******************************************************************************/
/*!
	\brief Frees a text object and gives back old string.
	\param[in] text Text.
	\param[out] string_A Will be string of text. Caller is
		responsible for freeing by passing to PINTO_HOOK_FREE().
	\return void
*/
void pintoTextFreeAndGetString( PintoText **text_F, char **string_A )
{
	/* CODE */
	PARANOID_ERR_IF( text_F == NULL );
	PARANOID_ERR_IF( (*text_F) == NULL );
	PARANOID_ERR_IF( string_A == NULL );
	PARANOID_ERR_IF( (*string_A) != NULL );

	/* giveback */
	PARANOID_ERR_IF( (*text_F)->string == NULL );
	(*string_A) = (*text_F)->string;
	(*text_F)->string = NULL;

	PINTO_HOOK_FREE( (*text_F) );
	(*text_F) = NULL;

	return;
}

/******************************************************************************/
/*!
	\brief Helps the inflate function by duplicating previous characters and
	       adding them to the end of the text.
	\param[in] text Text.
	\param[in] distance Amount of characters from the end to start copying.
	\param[in] length Amount of characters to copy.
	\return PINTO_RC

	Will work even if length is greater than distance.
	For example, if the text was:
	"ABCDE"
	With distance=2 and length=5, then the text would become:
	"ABCDEDEDED"
*/
PINTO_RC pintoTextInflateHelper( PintoText *text, s32 distance, s32 length )
{
	/* DATA */
	PINTO_RC rc = PINTO_RC_SUCCESS;

	char *reallocedText = NULL;

	s32 i = 0;
	s32 j = 0;


	/* CODE */
	PARANOID_ERR_IF( text == NULL );
	PARANOID_ERR_IF( distance < 0 );
	PARANOID_ERR_IF( length < 0 );

	ERR_IF( distance == 0, PINTO_RC_ERROR_FORMAT_INVALID );
	ERR_IF( length == 0, PINTO_RC_ERROR_FORMAT_INVALID );

	/* make sure distance is not too big */
	ERR_IF( text->usedSize - distance < 0, PINTO_RC_ERROR_FORMAT_INVALID );

	/* do we need to resize our buffer? */
	if ( text->usedSize + length >= text->allocedSize )
	{
		/* No text should ever reach this size, so we put this in to prevent
		   the buffer from growing too big.
		   The fear is a malicious person crafting a small pinto encoding, that will
		   deflate to take up huge amounts of ram ... if you consider more than 16MB to be huge :) */
		ERR_IF( text->usedSize + length > ( PINTO_MAX_WIDTH * PINTO_MAX_HEIGHT ), PINTO_RC_ERROR_FORMAT_TOO_LONG );

		/* *** */
		PINTO_REALLOC( reallocedText, text->string, char, text->allocedSize + length );

		text->allocedSize += length;
		text->string = reallocedText;
	}

	/* add */
	j = text->usedSize - distance;

	while ( i < length )
	{
		text->string[ text->usedSize ] = text->string[ j ];
		text->usedSize += 1;
		text->string[ text->usedSize ] = '\0';

		/* next character */
		i += 1;
		j += 1;
	}


	/* CLEANUP */
	cleanup:

	return rc;
}

