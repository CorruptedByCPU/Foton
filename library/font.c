/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#ifndef	LIB_FONT
		#include	"./font.h"
	#endif
	//----------------------------------------------------------------------
	#include	"./font/data.c"
	//----------------------------------------------------------------------

// return position of character inside matrix array
static uint64_t lib_font_offset( uint8_t font, uint8_t character ) {
	// characted at init position
	uint64_t offset = EMPTY;

	// for every character as index inside ASCII table
	for( uint64_t i = 0; i < character; i++ ) {
		// calculate character offset inside matrix table
		if( font == LIB_FONT_FAMILY_ROBOTO_MONO ) offset += LIB_FONT_FAMILY_ROBOTO_MONO_KERNING;
		if( font == LIB_FONT_FAMILY_ROBOTO ) offset += lib_font_family_roboto_kerning[ i ];
	}

	// give back offset value
	return offset;
}

// return character width in pixels
uint8_t lib_font_length_char( uint8_t font, uint8_t character ) {
	// if font style is not monospace
	if( font == LIB_FONT_FAMILY_ROBOTO ) return lib_font_family_roboto_kerning[ character - STD_ASCII_SPACE ];

	// else
	return LIB_FONT_FAMILY_ROBOTO_MONO_KERNING;
}

// print character on screen
static uint8_t lib_font_char( uint8_t font, uint64_t scanline_pixel, uint32_t *pixel, uint8_t character, uint32_t color, uint8_t flag ) {
	// properties of font matrix
	uint8_t *matrix = (uint8_t *) lib_font_matrix;

	// move pointer of matrix array to selected character
	if( font == LIB_FONT_FAMILY_ROBOTO ) matrix += LIB_FONT_FAMILY_ROBOTO_offset + lib_font_offset( font, character );
	else matrix += LIB_FONT_FAMILY_ROBOTO_MONO_offset + lib_font_offset( font, character );

	// for every pixel from character matrix
	for( uint8_t y = 0; y < LIB_FONT_MATRIX_height_pixel; y++ )
		for( uint16_t x = 0; x < lib_font_length_char( font, character + STD_ASCII_SPACE ); x++ )
			// show him if is visible
			if( matrix[ (y * LIB_FONT_MATRIX_width_pixel) + x ] ) {
				// simple bold function :>
				uint8_t bold = TRUE; if( flag & LIB_FONT_FLAG_WEIGHT_bold ) bold = LIB_FONT_BOLD_level;

				// put pixels
				while( bold-- ) pixel[ (y * scanline_pixel) + x ] = lib_font_color( pixel[ (y * scanline_pixel) + x ], (color & 0x00FFFFFF) | matrix[ (y * LIB_FONT_MATRIX_width_pixel) + x ] << 24 );
			}

	// return to string function width of displayed character in pixels
	return lib_font_length_char( font, character + STD_ASCII_SPACE );
}

uint32_t lib_font_color( uint32_t background, uint32_t foreground ) {
	return ((((((255 - ((foreground & 0xFF000000) >> 24)) * (background & 0x00FF00FF)) + (((foreground & 0xFF000000) >> 24) * (foreground & 0x00FF00FF))) >> 8) & 0x00FF00FF) | ((((255 - ((foreground & 0xFF000000) >> 24)) * ((background & (0xFF000000 | 0x0000FF00)) >> 8)) + (((foreground & 0xFF000000) >> 24) * (0x01000000 | ((foreground & 0x0000FF00) >> 8)))) & (0xFF000000 | 0x0000FF00)));
}

// calculate string width in pixels
uint64_t lib_font_length_string( uint8_t font, uint8_t *string, uint64_t length ) {
	// for non-monospace font style
	if( font == LIB_FONT_FAMILY_ROBOTO ) {
		// string init width
		uint64_t width = EMPTY;

		// for every character inside string
		for( uint64_t i = 0; i < length; i++ ) {
			// apply matrix offset of character if is printable
			if( string[ i ] > 0x1F && string[ i ] < 0x7F ) width += lib_font_family_roboto_kerning[ string[ i ] - STD_ASCII_SPACE ];
			else return EMPTY;	// or no support if there is at least 1 character not printable
		}

		// return calculated string width in pixels
		return width;
	} else
		// monospace font style is much simpler ;)
		return length * LIB_FONT_FAMILY_ROBOTO_MONO_KERNING;
}

// main function of library
uint64_t lib_font( uint8_t font, uint8_t *string, uint64_t length, uint32_t color, uint32_t *pixel, uint64_t scanline_pixel, uint8_t flag ) {
	// string length in pixels
	uint64_t string_pixel = lib_font_length_string( font, string, length );

	// no support for empty strings or if there is at least 1 character not printable
	if( ! string_pixel ) return EMPTY;

	// move pointer of destination according of alignment
	if( flag & LIB_FONT_FLAG_ALIGN_center ) pixel -= string_pixel >> STD_SHIFT_2;
	if( flag & LIB_FONT_FLAG_ALIGN_right ) pixel -= string_pixel;

	// print all characters from string
	for( uint64_t i = 0; i < length; i++ ) pixel += lib_font_char( font, scanline_pixel, pixel, string[ i ] - STD_ASCII_SPACE, color, flag );

	// return length of string in pixels
	return string_pixel;
}

uint64_t lib_font_value( uint8_t font, uint64_t value, uint8_t base, uint32_t color, uint32_t *pixel, uint64_t scanline_pixel, uint8_t flag ) {
	// base of value not supported?
	if( base < 2 || base > 36 ) return EMPTY;	// yes

	// temporary space for ASCII value
	uint8_t string[ 64 ] = { 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30 }; uint8_t index = 64;

	// parse every digit from value
	while( value ) {
		// store first digit from value
		string[ --index ] = (value % base) | 0x30;

		// if digit is alfanumerical
		if( string[ index ] > 0x39 ) string[ index ] += 0x07;	// ASCII "A" index

		// remove digit from value
		value /= base;
	}

	// if value was 0
	if( index == 64 ) index--;

	// show value
	return lib_font( font, (uint8_t *) &string[ index ], 64 - index, color, pixel, scanline_pixel, flag );
}
