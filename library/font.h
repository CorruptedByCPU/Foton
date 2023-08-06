/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_FONT
	#define	LIB_FONT

        // display management
	#define LIB_FONT_ALIGN_left             	0b00000001
	#define LIB_FONT_ALIGN_center           	0b00000010
	#define LIB_FONT_ALIGN_right            	0b00000100
	#define LIB_FONT_ALIGN_justify          	0b00001000

	// available font styles
	#define LIB_FONT_FAMILY_ROBOTO_MONO     	0
	#define LIB_FONT_FAMILY_ROBOTO          	1

	// matrix dimension
	#define LIB_FONT_MATRIX_width_pixel		1481
	#define LIB_FONT_MATRIX_height_pixel		16

	// how deep into matrix is located font style characters
	#define LIB_FONT_FAMILY_ROBOTO_MONO_offset	0
	#define LIB_FONT_FAMILY_ROBOTO_offset		760

	// monospace font doesn't use kerning, it has default width in pixels
	#define LIB_FONT_FAMILY_ROBOTO_MONO_KERNING	8

	// those are defaults for any terminal applications
	#define LIB_FONT_WIDTH_pixel			LIB_FONT_FAMILY_ROBOTO_MONO_KERNING
	#define LIB_FONT_HEIGHT_pixel			LIB_FONT_MATRIX_height_pixel

	// all functions return length of string in pixels

	// main function of printing strings
	uint64_t lib_font( uint8_t font, uint8_t *string, uint64_t length, uint32_t color, uint32_t *pixel, uint64_t scanline_pixel, uint8_t align );

	// convert value to string and print it
	uint64_t lib_font_value( uint8_t font, uint64_t value, uint8_t base, uint32_t color, uint32_t *pixel, uint64_t scanline_pixel, uint8_t align );

	// length of string in pixels
	uint64_t lib_font_length_string( uint8_t font, uint8_t *string, uint64_t length );
#endif
