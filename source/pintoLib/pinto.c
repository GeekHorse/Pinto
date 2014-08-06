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
	Encoding and decoding functions.
*/
#undef PINTO_FILE_NUMBER
#define PINTO_FILE_NUMBER 1

/******************************************************************************/
#include "pinto.h"
#include "pintoInternal.h"

/******************************************************************************/
extern const char valueToChar[];

/******************************************************************************/
/*!
	\brief Encodes an image into the Pinto format.
	\param[in] image The image to be encoded.
	\param[out] string_A On success, the string that contains the encoded image.
		Will be allocated. The caller is responsible for freeing.
	\return PINTO_RC
*/
PINTO_RC pintoImageEncode( const PintoImage *image, char **string_A )
{
	/* DATA */
	PINTO_RC rc = PINTO_RC_SUCCESS;

	PintoText *newText = NULL;
	PintoText *newTextDeflated = NULL;
	u8 *palette = NULL;

	/* This is the indexed image data.
	   Instead of holding RGB, it holds an index into the palette.
	   The special value -1 is for transparent pixels. */
	char *indexedData = NULL;

	s32 color = 0;
	s32 pixel = 0;

	s32 colorAmount = 0;
	s32 colorFound = 0;

	char needToAddStandardHeader = 1;

	u8 red = 0;
	u8 green = 0;
	u8 blue = 0;

	char rleState = 0;
	s32 rleCount = 0;

	PintoImage *verifyImage = NULL;

	char *givebackString = NULL;


	/* PRECOND */
	FAILURE_POINT;
	ERR_IF( image == NULL, PINTO_RC_ERROR_PRECOND );
	ERR_IF( string_A == NULL, PINTO_RC_ERROR_PRECOND );
	ERR_IF( (*string_A) != NULL, PINTO_RC_ERROR_PRECOND );


	/* CODE */

	/* check size */
	ERR_IF_1( image->width  <= 0, PINTO_RC_ERROR_IMAGE_BAD_SIZE, image->width );
	ERR_IF_1( image->height <= 0, PINTO_RC_ERROR_IMAGE_BAD_SIZE, image->height );

	ERR_IF_1( image->width  > PINTO_MAX_WIDTH,  PINTO_RC_ERROR_IMAGE_BAD_SIZE, image->width );
	ERR_IF_1( image->height > PINTO_MAX_HEIGHT, PINTO_RC_ERROR_IMAGE_BAD_SIZE, image->height );

	/* malloc data, which is the palette-ized version of the image.
	   instead of rgba, it'll hold the index of the color, with -1
	   being a special case of transparent. */
	PINTO_MALLOC( indexedData, char, image->width * image->height );

	/* determine palette */
	PINTO_CALLOC( palette, u8, PINTO_MAX_COLORS * 3 );

	/* foreach pixel */
	for ( pixel = 0; pixel < ( image->width * image->height ); pixel += 1 )
	{
		/* if alpha is 0, fully transparent, ignore */
		if ( image->rgba[ ( pixel * 4 ) + 3 ] == 0 )
		{
			/* mark pixel in data as transparent */
			indexedData[ pixel ] = -1;

			continue;
		}

		/* if alpha is not 255, fully opaque, then it's partially
		   transparent, and not allowed */
		ERR_IF( image->rgba[ ( pixel * 4 ) + 3 ] != 255, PINTO_RC_ERROR_IMAGE_PARTIAL_TRANSPARENCY );

		/* reset colorFound flag */
		colorFound = 0;

		/* get the red, green, and blue of the pixel */
		red   = image->rgba[ ( pixel * 4 )     ];
		green = image->rgba[ ( pixel * 4 ) + 1 ];
		blue  = image->rgba[ ( pixel * 4 ) + 2 ];

		red   = PINTO_8_BITS_TO_6_BITS( red   );
		green = PINTO_8_BITS_TO_6_BITS( green );
		blue  = PINTO_8_BITS_TO_6_BITS( blue  );

		/* is this color already in our palette? */

		/* foreach color */
		for ( color = 0; color < colorAmount; color += 1 )
		{
			/* does the current pixel match this color? */
			if (    red   == palette[ ( color * 3 )     ]
			     && green == palette[ ( color * 3 ) + 1 ]
			     && blue  == palette[ ( color * 3 ) + 2 ]
			   )
			{
				colorFound = 1;
				break;
			}
		}

		/* if color wasn't found, it's new, so add it to our palette */
		if ( colorFound == 0 )
		{
			/* make sure we dont go over PINTO_MAX_COLORS */
			ERR_IF( colorAmount == PINTO_MAX_COLORS, PINTO_RC_ERROR_IMAGE_TOO_MANY_COLORS );

			/* add it to palette */
			palette[ ( colorAmount * 3 )     ] = red;
			palette[ ( colorAmount * 3 ) + 1 ] = green;
			palette[ ( colorAmount * 3 ) + 2 ] = blue;

			colorAmount += 1;
		}

		/* mark this pixel in data */
		indexedData[ pixel ] = color;
	}

	/* we've created out palette, now lets start creating the pinto encoding */

	/* create text */
	rc = pintoTextInit( &newText );
	ERR_IF_PASSTHROUGH;

	/* See if it's a certain image, and we can use a shorter header */
	if (    colorAmount == 1
	     && palette[ 0 ] == 0 && palette[ 1 ] == 0 && palette[ 2 ] == 0
	     && image->width == image->height
	   )
	{
		if ( image->width == 8 )
		{
			rc = pintoTextAddChar( newText, '0' );
			ERR_IF_PASSTHROUGH;

			needToAddStandardHeader = 0;
		}
		if ( image->width == 16 )
		{
			rc = pintoTextAddChar( newText, '1' );
			ERR_IF_PASSTHROUGH;

			needToAddStandardHeader = 0;
		}
		else if ( image->width == 32 )
		{
			rc = pintoTextAddChar( newText, '2' );
			ERR_IF_PASSTHROUGH;

			needToAddStandardHeader = 0;
		}
		else if ( image->width == 64 )
		{
			rc = pintoTextAddChar( newText, '3' );
			ERR_IF_PASSTHROUGH;

			needToAddStandardHeader = 0;
		}
		else if ( image->width == 128 )
		{
			rc = pintoTextAddChar( newText, '4' );
			ERR_IF_PASSTHROUGH;

			needToAddStandardHeader = 0;
		}
		else if ( image->width == 256 )
		{
			rc = pintoTextAddChar( newText, '5' );
			ERR_IF_PASSTHROUGH;

			needToAddStandardHeader = 0;
		}
		else if ( image->width == 512 )
		{
			rc = pintoTextAddChar( newText, '6' );
			ERR_IF_PASSTHROUGH;

			needToAddStandardHeader = 0;
		}
		else if ( image->width == 1024 )
		{
			rc = pintoTextAddChar( newText, '7' );
			ERR_IF_PASSTHROUGH;

			needToAddStandardHeader = 0;
		}
		else if ( image->width == 2048 )
		{
			rc = pintoTextAddChar( newText, '8' );
			ERR_IF_PASSTHROUGH;

			needToAddStandardHeader = 0;
		}
		else if ( image->width == 4096 )
		{
			rc = pintoTextAddChar( newText, '9' );
			ERR_IF_PASSTHROUGH;

			needToAddStandardHeader = 0;
		}
	}

	if ( needToAddStandardHeader )
	{
			/* 'a' means standard header follows */
			rc = pintoTextAddChar( newText, 'a' );
			ERR_IF_PASSTHROUGH;

			/* add width */
			rc = pintoTextAddValue( newText, image->width );
			ERR_IF_PASSTHROUGH;

			/* add height */
			rc = pintoTextAddValue( newText, image->height );
			ERR_IF_PASSTHROUGH;

			/* add colorAmount */
			rc = pintoTextAddValue( newText, colorAmount );
			ERR_IF_PASSTHROUGH;

			/* add colors to palette */
			for ( color = 0; color < ( colorAmount * 3 ); color += 1 )
			{
				/* divide by 4 because pinto's colors are 6-bit */
				rc = pintoTextAddChar( newText, valueToChar[ palette[ color ] / 4 ] );
				ERR_IF_PASSTHROUGH;
			}
	}

	/* add RLE data */

	/* foreach color */
	for ( color = 0; color < colorAmount; color += 1 )
	{
		rleState = 0;
		rleCount = 0;

		/* foreach pixel */
		for ( pixel = 0; pixel < ( image->width * image->height ); pixel += 1 )
		{
			/* if off */
			if ( rleState == 0 )
			{
				/* if same color */
				if ( color == indexedData[ pixel ] )
				{
					/* add rle count */
					rc = pintoTextAddValue( newText, rleCount );
					ERR_IF_PASSTHROUGH;

					/* reset rleState and rleCount */
					rleState = 1;
					rleCount = 1;
				}
				/* else if different color */
				else
				{
					rleCount += 1;
				}
			}
			/* else, on */
			else
			{
				PARANOID_ERR_IF( rleState != 1 );

				/* if same or higher color */
				if ( indexedData[ pixel ] >= color )
				{
					rleCount += 1;
				}
				/* else if lower color */
				else
				{
					/* add rle count */
					rc = pintoTextAddValue( newText, rleCount );
					ERR_IF_PASSTHROUGH;

					rleState = 0;
					rleCount = 1;
				}
			}

		} /* end foreach pixel */

		/* add '^', which signals end of color */
		rc = pintoTextAddChar( newText, '^' );
		ERR_IF_PASSTHROUGH;

	} /* end foreach color */

	/* deflate */
	rc = pintoSimpleDeflate( &newText, &newTextDeflated );
	ERR_IF_PASSTHROUGH;

	/* get string */
	pintoTextFreeAndGetString( &newTextDeflated, &givebackString );

	/* give back */
	(*string_A) = givebackString;
	givebackString = NULL;


	/* CLEANUP */
	cleanup:

	PINTO_HOOK_FREE( palette );
	palette = NULL;

	PINTO_HOOK_FREE( indexedData );
	indexedData = NULL;

	pintoTextFree( &newText );
	pintoTextFree( &newTextDeflated );

	pintoImageFree( &verifyImage );

	PINTO_HOOK_FREE( givebackString );
	givebackString = NULL;

	return rc;
}

/******************************************************************************/
/*!
	\brief Decodes a string into an image.
	\param[in] string The string to be decoded.
	\param[out] image_A On success, the decoded image.
		Will be allocated. The caller is responsible for freeing by passing the
		image to the pintoImageFree() function.
	\return PINTO_RC
*/
PINTO_RC pintoImageDecodeString( const char *string, PintoImage **image_A )
{
	/* DATA */
	PINTO_RC rc = PINTO_RC_SUCCESS;

	PintoText *text = NULL;


	/* PRECOND */
	FAILURE_POINT;
	ERR_IF( string == NULL, PINTO_RC_ERROR_PRECOND );
	ERR_IF( image_A == NULL, PINTO_RC_ERROR_PRECOND );
	ERR_IF( (*image_A) != NULL, PINTO_RC_ERROR_PRECOND );


	/* CODE */
	/* create our text */
	rc = pintoTextInit( &text );
	ERR_IF_PASSTHROUGH;

	/* add string to our pintoText */
	while ( (*string) != '\0' )
	{
		rc = pintoTextAddChar( text, (*string) );
		ERR_IF_PASSTHROUGH;

		string += 1;
	}

	/* call the text version */
	rc = pintoImageDecodeText( &text, image_A );
	ERR_IF_PASSTHROUGH;


	/* CLEANUP */
	cleanup:

	pintoTextFree( &text );

	return rc;
}

/******************************************************************************/
/*!
	\brief Decodes a text into an image.
	\param[in] text_F The text to be decoded. On success, this will be freed.
	\param[out] image_A On success, the decoded image.
		Will be allocated. The caller is responsible for freeing by passing the
		image to pintoImageFree().
	\return PINTO_RC
*/
PINTO_RC pintoImageDecodeText( PintoText **text_F, PintoImage **image_A )
{
	/* DATA */
	PINTO_RC rc = PINTO_RC_SUCCESS;

	PintoText *text = NULL;

	PintoImage *newImage = NULL;

	char header = 0;

	s32 colorIndex = 0;
	s32 i = 0;
	s32 pixelIndex = 0;

	char ch = 0;
	s32 value = 0;

	s32 width = 0;
	s32 height = 0;

	s32 colorAmount = 0;
	u8 *palette = NULL;

	char flagOn = 0;


	/* PRECOND */
	FAILURE_POINT;
	ERR_IF( text_F == NULL, PINTO_RC_ERROR_PRECOND );
	ERR_IF( (*text_F) == NULL, PINTO_RC_ERROR_PRECOND );
	ERR_IF( image_A == NULL, PINTO_RC_ERROR_PRECOND );
	ERR_IF( (*image_A) != NULL, PINTO_RC_ERROR_PRECOND );


	/* CODE */

	/* inflate */
	rc = pintoSimpleInflate( text_F, &text );
	ERR_IF_PASSTHROUGH;

	/* get header */
	rc = pintoTextGetChar( text, &header );
	ERR_IF_PASSTHROUGH;

	/* standard header marker */
	if ( header == 'a' )
	{
		/* get width */
		rc = pintoTextGetValue( text, &width );
		ERR_IF_PASSTHROUGH;

		ERR_IF_1( width <= 0, PINTO_RC_ERROR_FORMAT_INVALID, width );
		ERR_IF_1( width > PINTO_MAX_WIDTH, PINTO_RC_ERROR_FORMAT_INVALID, width );

		/* get height */
		rc = pintoTextGetValue( text, &height );
		ERR_IF_PASSTHROUGH;

		ERR_IF_1( height <= 0, PINTO_RC_ERROR_FORMAT_INVALID, height );
		ERR_IF_1( height > PINTO_MAX_HEIGHT, PINTO_RC_ERROR_FORMAT_INVALID, height );

		/* allocate new image */
		rc = pintoImageInit( width, height, &newImage );
		ERR_IF_PASSTHROUGH;

		/* get number of colors in palette */
		rc = pintoTextGetValue( text, &colorAmount );
		ERR_IF_PASSTHROUGH;

		ERR_IF_1( colorAmount > PINTO_MAX_COLORS, PINTO_RC_ERROR_FORMAT_INVALID, colorAmount );

		if ( colorAmount > 0 )
		{
			/* allocate palette */
			PINTO_CALLOC( palette, u8, colorAmount * 3 );

			/* read in colors */
			for ( colorIndex = 0; colorIndex < ( colorAmount * 3 ); colorIndex += 1 )
			{
				value = 0;

				rc = pintoTextUpdateValue( text, &value );
				ERR_IF_PASSTHROUGH;

				palette[ colorIndex ] = PINTO_6_BITS_TO_8_BITS( value );
			}

		}
	}
	/* smaller header */
	else
	{
		if ( header == '0' )
		{
			width = height = 8;
		}
		else if ( header == '1' )
		{
			width = height = 16;
		}
		else if ( header == '2' )
		{
			width = height = 32;
		}
		else if ( header == '3' )
		{
			width = height = 64;
		}
		else if ( header == '4' )
		{
			width = height = 128;
		}
		else if ( header == '5' )
		{
			width = height = 256;
		}
		else if ( header == '6' )
		{
			width = height = 512;
		}
		else if ( header == '7' )
		{
			width = height = 1024;
		}
		else if ( header == '8' )
		{
			width = height = 2048;
		}
		else if ( header == '9' )
		{
			width = height = 4096;
		}
		else
		{
			ERR_IF_1( 1, PINTO_RC_ERROR_FORMAT_INVALID, header );
		}

		/* allocate new image */
		rc = pintoImageInit( width, height, &newImage );
		ERR_IF_PASSTHROUGH;

		colorAmount = 1;

		/* allocate palette */
		/* since this is calloc, our single color will be black, which is
		   what we want */
		PINTO_CALLOC( palette, u8, colorAmount * 3 );
	}

	/* foreach color */
	for ( colorIndex = 0; colorIndex < colorAmount; colorIndex += 1 )
	{
		pixelIndex = 0;
		flagOn = 0;

		/* rle decoding */
		while ( 1 )
		{
			/* peek at current character to see if it's a '^', which
			   signals end of current color */
			rc = pintoTextPeekChar( text, &ch );
			ERR_IF_PASSTHROUGH;

			if ( ch == '^' )
			{
				/* we just peeked, now we must remove it to move
				   past it in the text */
				rc = pintoTextGetChar( text, &ch );
				PARANOID_ERR_IF( rc != PINTO_RC_SUCCESS );

				break;
			}

			/* get the rle value */
			rc = pintoTextGetValue( text, &value );
			ERR_IF_PASSTHROUGH;

			/* make sure it doesn't go past end of rgba array */
			ERR_IF( pixelIndex + ( value * 4 ) >= ( width * height * 4 ), PINTO_RC_ERROR_FORMAT_INVALID );

			/* if we're in off state, just increment our pixelIndex */
			if ( flagOn == 0 )
			{
				pixelIndex += ( value * 4 );
			}
			/* else we're in the on state, so draw our run */
			else
			{
				for ( i = 0; i < value; i += 1, pixelIndex += 4 )
				{
					newImage->rgba[ pixelIndex     ] = palette[ ( colorIndex * 3 )     ];
					newImage->rgba[ pixelIndex + 1 ] = palette[ ( colorIndex * 3 ) + 1 ];
					newImage->rgba[ pixelIndex + 2 ] = palette[ ( colorIndex * 3 ) + 2 ];
					newImage->rgba[ pixelIndex + 3 ] = 255;
				}
			}

			/* switch states */
			flagOn = ( ! flagOn );

		} /* end rle decoding */

		/* the last rleValue is implicit,
		   if we're in on state */
		if ( flagOn )
		{
			/* draw until end of image */
			for ( ; pixelIndex < ( width * height * 4 ); pixelIndex += 4 )
			{
				newImage->rgba[ pixelIndex     ] = palette[ ( colorIndex * 3 )     ];
				newImage->rgba[ pixelIndex + 1 ] = palette[ ( colorIndex * 3 ) + 1 ];
				newImage->rgba[ pixelIndex + 2 ] = palette[ ( colorIndex * 3 ) + 2 ];
				newImage->rgba[ pixelIndex + 3 ] = 255;
			}
		}

	} /* end foreach color */

	/* must be at end of text */
	ERR_IF( pintoTextAtEnd( text ) == 0, PINTO_RC_ERROR_FORMAT_INVALID );

	/* give back */
	(*image_A) = newImage;
	newImage = NULL;

	pintoTextFree( text_F );


	/* CLEANUP */
	cleanup:

	PINTO_HOOK_FREE( palette );
	palette = NULL;

	pintoTextFree( &text );

	pintoImageFree( &newImage );

	return rc;
}

/******************************************************************************/
/*!
	\brief Allocates a new image that's all transparent.
	\param[in] width The width of the image.
	\param[in] height The height of the image.
	\param[out] image_A On success, the decoded image.
		Will be allocated. The caller is responsible for freeing by passing the
		image to pintoImageFree().
	\return PINTO_RC
*/
PINTO_RC pintoImageInit( s32 width, s32 height, PintoImage **image_A )
{
	/* DATA */
	PINTO_RC rc = PINTO_RC_SUCCESS;

	PintoImage *newImage = NULL;


	/* PRECOND */
	FAILURE_POINT;
	ERR_IF_1( width <= 0 || width > PINTO_MAX_WIDTH, PINTO_RC_ERROR_PRECOND, width );
	ERR_IF_1( height <= 0 || height > PINTO_MAX_HEIGHT, PINTO_RC_ERROR_PRECOND, height );
	ERR_IF( image_A == NULL, PINTO_RC_ERROR_PRECOND );
	ERR_IF( (*image_A) != NULL, PINTO_RC_ERROR_PRECOND );


	/* CODE */
	/* create our newImage */
	PINTO_CALLOC( newImage, PintoImage, 1 );

	/* allocate rgba */
	PINTO_CALLOC( newImage->rgba, u8, width * height * 4 );

	newImage->width = width;
	newImage->height = height;

	/* give back */
	(*image_A) = newImage;
	newImage = NULL;


	/* CLEANUP */
	cleanup:

	PINTO_HOOK_FREE( newImage );
	newImage = NULL;

	return rc;
}

/******************************************************************************/
/*!
	\brief Frees an image.
	\param[in] image_F The image to be freed. Will be freed with
		PINTO_HOOK_FREE(). On return, image_F will be NULL.
	\return void.
*/
void pintoImageFree( PintoImage **image_F )
{
	/* CODE */
	if ( image_F == NULL || (*image_F) == NULL )
	{
		return;
	}

	PINTO_HOOK_FREE( (*image_F)->rgba );
	(*image_F)->rgba = NULL;

	PINTO_HOOK_FREE( (*image_F) );
	(*image_F) = NULL;

	return;
}

/******************************************************************************/
/*!
	\brief Compresses text with a simple version of deflate.
	\param[in] textToDeflate_F text to be compressed. On success, will be freed.
	\param[out] text_A On success, will be compressed text.
		Will be allocated. The caller is responsible for freeing with
		pintoTextFree().
	\return PINTO_RC
*/
PINTO_RC pintoSimpleDeflate( PintoText **textToDeflate_F, PintoText **text_A )
{
	/* DATA */
	PINTO_RC rc = PINTO_RC_SUCCESS;

	PintoText *newText = NULL;
	char *string = NULL;
	s32 stringLength = 0;

	s32 i = 0;

	s32 matchI = 0;
	s32 matchLength = 0;

	s32 bestMatchLength = 0;
	s32 bestMatchDistance = 0;


	/* CODE */
	FAILURE_POINT;
	PARANOID_ERR_IF( textToDeflate_F == NULL );
	PARANOID_ERR_IF( (*textToDeflate_F) == NULL );
	PARANOID_ERR_IF( text_A == NULL );
	PARANOID_ERR_IF( (*text_A) != NULL );

	/* get string length */
	stringLength = (*textToDeflate_F)->usedSize;

	/* get string */
	pintoTextFreeAndGetString( textToDeflate_F, &string );

	/* create our new text */
	rc = pintoTextInit( &newText );
	ERR_IF_PASSTHROUGH;

	/* go through string */
	i = 0;
	/* we go until there's only 4 characters left, since we can only deflate
	   at least 4 characters */
	while ( i <= (stringLength - 4) )
	{
		/* reset our length variables */
		matchLength = 0;
		bestMatchLength = 0;

		/* set matchI */
		matchI = i - 1;

		/* go up to 64 * 64 characters back, finding the best match */
		while ( matchI >= 0 && ( i - matchI ) < (64 * 64) )
		{
			/* do at least 4 characters match? */
			if (    string[ matchI     ] == string[ i     ]
			     && string[ matchI + 1 ] == string[ i + 1 ]
			     && string[ matchI + 2 ] == string[ i + 2 ]
			     && string[ matchI + 3 ] == string[ i + 3 ]
			   )
			{
				/* see how long the match is */
				/* we don't need to worry about going past the end of the string, because
				   before we go past the end of the string we won't match the terminating
				   '\0' and break out anyway */
				/* we dont need to worry about matchLength becoming larger than we can
				   encode, because we only allow PINTO_MAX_WIDTH * PINTO_MAX_HEIGHT
				   characters in the string, so the max we'll have to encode is
				   ( PINTO_MAX_WIDTH * PINTO_MAX_HEIGHT ) - 1
				   which is exactly the max we can encode. (given than the max width
				   and height is 4096 and we use 4 64 values in encoding) */
				matchLength = 4;
				while ( string[ matchI + matchLength ] == string[ i + matchLength ] )
				{
					matchLength += 1;
				}

				/* is this our best match? */
				if ( matchLength > bestMatchLength )
				{
					bestMatchLength = matchLength;
					bestMatchDistance = i - matchI;
				}
			}

			/* decrement matchI
			   try to find match at next spot */
			matchI -= 1;
		}

		/*    if no match*/
		/* or if best match length was 4 and the distance was farther than 64, then
		      the match is 4 characters and the encoding will be 4 characters, so there
		      is no point deflating. */
		if (    bestMatchLength == 0
		     || ( bestMatchLength == 4 && bestMatchDistance >= 64 )
		   )
		{
			rc = pintoTextAddChar( newText, string[ i ] );
			ERR_IF_PASSTHROUGH;

			/* increment i */
			i += 1;
		}
		/* there was a match */
		else
		{
			if ( bestMatchDistance < 64 )
			{
				rc = pintoTextAddChar( newText, '?' );
				ERR_IF_PASSTHROUGH;

				rc = pintoTextAddValue( newText, bestMatchDistance );
				ERR_IF_PASSTHROUGH;

				rc = pintoTextAddValue( newText, bestMatchLength );
				ERR_IF_PASSTHROUGH;
			}
			else
			{
				PARANOID_ERR_IF( bestMatchDistance >= ( 64 * 64 ) );

				rc = pintoTextAddChar( newText, '@' );
				ERR_IF_PASSTHROUGH;

				rc = pintoTextAddChar( newText, valueToChar[ ( (bestMatchDistance) / 64 ) % 64 ] );
				ERR_IF_PASSTHROUGH;

				rc = pintoTextAddChar( newText, valueToChar[   (bestMatchDistance)        % 64 ] );
				ERR_IF_PASSTHROUGH;

				rc = pintoTextAddValue( newText, bestMatchLength );
				ERR_IF_PASSTHROUGH;
			}

			/* increment i */
			i += bestMatchLength;
		}
	}

	/* add rest of string to newText */
	while ( i < stringLength )
	{
		rc = pintoTextAddChar( newText, string[ i ] );
		ERR_IF_PASSTHROUGH;

		/* *** */
		i += 1;
	}

	/* free textToDeflate */
	pintoTextFree( textToDeflate_F );

	/* give back */
	(*text_A) = newText;
	newText = NULL;


	/* CLEANUP */
	cleanup:

	pintoTextFree( &newText );

	PINTO_HOOK_FREE( string );
	string = NULL;

	return rc;
}

/******************************************************************************/
/*!
	\brief Uncompresses text with a simple version of deflate (inflate here).
	\param[in] textToDeflate_F text to be uncompressed. On success, will be freed.
	\param[out] text_A On success, will be uncompressed text.
		Will be allocated. The caller is responsible for freeing with
		pintoTextFree().
	\return PINTO_RC
*/
PINTO_RC pintoSimpleInflate( PintoText **textToInflate_F, PintoText **text_A )
{
	/* DATA */
	PINTO_RC rc = PINTO_RC_SUCCESS;

	PintoText *newText = NULL;

	char ch = 0;

	s32 distance = 0;
	s32 length = 0;


	/* CODE */
	FAILURE_POINT;
	PARANOID_ERR_IF( textToInflate_F == NULL );
	PARANOID_ERR_IF( (*textToInflate_F) == NULL );
	PARANOID_ERR_IF( text_A == NULL );
	PARANOID_ERR_IF( (*text_A) != NULL );

	/* create our new text */
	rc = pintoTextInit( &newText );
	ERR_IF_PASSTHROUGH;

	/* go through string, inflating */
	while ( 1 )
	{
		/* is there another char to get? */
		if ( pintoTextAtEnd( (*textToInflate_F) ) )
		{
			break;
		}

		/* get next char */
		pintoTextGetChar( (*textToInflate_F), &ch );
		/* since we just called pintoTextAtEnd there is guaranteed to be a char to get.
		   so no need to check for error
		*/

		/* '?' is used for distances 1-63 */
		if ( ch == '?' )
		{
			/* get distance */
			distance = 0;

			rc = pintoTextUpdateValue( (*textToInflate_F), &distance );
			ERR_IF_PASSTHROUGH;

			/* get length */
			rc = pintoTextGetValue( (*textToInflate_F), &length );
			ERR_IF_PASSTHROUGH;

			/* inflate */
			rc = pintoTextInflateHelper( newText, distance, length );
			ERR_IF_PASSTHROUGH;
		}
		/* '@' is used for distances 64-4095 */
		else if ( ch == '@' )
		{
			/* get distance */
			distance = 0;

			rc = pintoTextUpdateValue( (*textToInflate_F), &distance );
			ERR_IF_PASSTHROUGH;
			rc = pintoTextUpdateValue( (*textToInflate_F), &distance );
			ERR_IF_PASSTHROUGH;

			/* get length */
			rc = pintoTextGetValue( (*textToInflate_F), &length );
			ERR_IF_PASSTHROUGH;

			/* inflate */
			rc = pintoTextInflateHelper( newText, distance, length );
			ERR_IF_PASSTHROUGH;
		}
		/* no inflate marker, just add char */
		else
		{
			rc = pintoTextAddChar( newText, ch );
			ERR_IF_PASSTHROUGH;
		}
	}

	pintoTextFree( textToInflate_F );

	/* give back */
	(*text_A) = newText;
	newText = NULL;


	/* CLEANUP */
	cleanup:

	pintoTextFree( &newText );

	return rc;
}

/******************************************************************************/
/*!
	\brief Creates a new image half the size.
	\param[in] imageIn The original image.
	\param[out] imageOut_A On success, a new image half the size of the original
		image.
	\return PINTO_RC

	Used to create anti-aliased images.
*/
PINTO_RC pintoImageDownsize( PintoImage *imageIn, PintoImage **imageOut_A )
{
	/* DATA */
	PINTO_RC rc = PINTO_RC_SUCCESS;

	PintoImage *newImage = NULL;

	s32 width = 0;
	s32 height = 0;

	s32 x = 0;
	s32 y = 0;
	s32 newX = 0;
	s32 newY = 0;

	s32 avg = 0;


	/* PRECOND */
	FAILURE_POINT;
	ERR_IF( imageIn == NULL, PINTO_RC_ERROR_PRECOND );
	ERR_IF( imageOut_A == NULL, PINTO_RC_ERROR_PRECOND );
	ERR_IF( (*imageOut_A) != NULL, PINTO_RC_ERROR_PRECOND );


	/* CODE */
	width = imageIn->width;
	height = imageIn->height;

	/* if odd, round down */
	width = width & (~((s32)1));
	height = height & (~((s32)1));

	ERR_IF( width == 0, PINTO_RC_ERROR_IMAGE_TOO_SMALL );
	ERR_IF( height == 0, PINTO_RC_ERROR_IMAGE_TOO_SMALL );

	/* create new image at half size */
	rc = pintoImageInit( width / 2, height / 2, &newImage );
	ERR_IF_PASSTHROUGH;

	/* do the downsize */
	x = 0;
	newX = 0;
	while ( x < width )
	{
		y = 0;
		newY = 0;
		while ( y < height )
		{
			/* red */
			avg = 0;
			avg += imageIn->rgba[ ( ( y + 0 ) * imageIn->width * 4 ) + ( ( x + 0 ) * 4 ) ];
			avg += imageIn->rgba[ ( ( y + 0 ) * imageIn->width * 4 ) + ( ( x + 1 ) * 4 ) ];
			avg += imageIn->rgba[ ( ( y + 1 ) * imageIn->width * 4 ) + ( ( x + 0 ) * 4 ) ];
			avg += imageIn->rgba[ ( ( y + 1 ) * imageIn->width * 4 ) + ( ( x + 1 ) * 4 ) ];
			avg /= 4;
			newImage->rgba[ ( newY * (imageIn->width / 2) * 4 ) + ( newX * 4 ) ] = avg;

			/* green */
			avg = 0;
			avg += imageIn->rgba[ ( ( y + 0 ) * imageIn->width * 4 ) + ( ( x + 0 ) * 4 ) + 1 ];
			avg += imageIn->rgba[ ( ( y + 0 ) * imageIn->width * 4 ) + ( ( x + 1 ) * 4 ) + 1 ];
			avg += imageIn->rgba[ ( ( y + 1 ) * imageIn->width * 4 ) + ( ( x + 0 ) * 4 ) + 1 ];
			avg += imageIn->rgba[ ( ( y + 1 ) * imageIn->width * 4 ) + ( ( x + 1 ) * 4 ) + 1 ];
			avg /= 4;
			newImage->rgba[ ( newY * (width / 2) * 4 ) + ( newX * 4 ) + 1 ] = avg;

			/* blue */
			avg = 0;
			avg += imageIn->rgba[ ( ( y + 0 ) * imageIn->width * 4 ) + ( ( x + 0 ) * 4 ) + 2 ];
			avg += imageIn->rgba[ ( ( y + 0 ) * imageIn->width * 4 ) + ( ( x + 1 ) * 4 ) + 2 ];
			avg += imageIn->rgba[ ( ( y + 1 ) * imageIn->width * 4 ) + ( ( x + 0 ) * 4 ) + 2 ];
			avg += imageIn->rgba[ ( ( y + 1 ) * imageIn->width * 4 ) + ( ( x + 1 ) * 4 ) + 2 ];
			avg /= 4;
			newImage->rgba[ ( newY * (width / 2) * 4 ) + ( newX * 4 ) + 2 ] = avg;

			/* alpha */
			avg = 0;
			avg += imageIn->rgba[ ( ( y + 0 ) * imageIn->width * 4 ) + ( ( x + 0 ) * 4 ) + 3 ];
			avg += imageIn->rgba[ ( ( y + 0 ) * imageIn->width * 4 ) + ( ( x + 1 ) * 4 ) + 3 ];
			avg += imageIn->rgba[ ( ( y + 1 ) * imageIn->width * 4 ) + ( ( x + 0 ) * 4 ) + 3 ];
			avg += imageIn->rgba[ ( ( y + 1 ) * imageIn->width * 4 ) + ( ( x + 1 ) * 4 ) + 3 ];
			avg /= 4;
			newImage->rgba[ ( newY * (width / 2) * 4 ) + ( newX * 4 ) + 3 ] = avg;

			/* increment */
			y += 2;
			newY += 1;
		}

		/* increment */
		x += 2;
		newX += 1;
	}


	/* give back */
	(*imageOut_A) = newImage;
	newImage = NULL;


	/* CLEANUP */
	cleanup:

	pintoImageFree( &newImage );

	return rc;
}

/******************************************************************************/
/*!
	\brief Provides a const char string representation for a PINTO_RC
	\param[in] rc A PINTO_RC value.
	\return A const char representation of the passed in PINTO_RC.
*/
const char *pintoRCToString( PINTO_RC rc )
{
	static const char *rcStrings[] =
	{
		"Success",

		"Precondition Error",
		"Memory Allocation Error",
		"Standard Library Error",
		"Failure Point Error",

		"Image Bad Size Error",
		"Image Too Many Colors Error",
		"Image Partial Transparency Error",
		"Image Too Small Error",
		"Format Invalid Error",
		"Format Too Long Error"
	};

	static const char *rcUnknown = "Unknown Error";

	/* standard errors */
	if ( rc >= 0 && rc <= PINTO_RC_STANDARD_ERRORS_MAX )
	{
		return rcStrings[ rc ];
	}
	/* pinto errors */
	else if ( rc >= PINTO_RC_PINTO_ERRORS_MIN && rc <= PINTO_RC_PINTO_ERRORS_MAX )
	{
		return rcStrings[ PINTO_RC_STANDARD_ERRORS_MAX + rc - PINTO_RC_PINTO_ERRORS_MIN + 1 ];
	}

	return rcUnknown;
}

