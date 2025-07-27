/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_FONT
	#define	LIB_FONT

        // display management
	#define LIB_FONT_FLAG_ALIGN_left             	0x00
	#define LIB_FONT_FLAG_ALIGN_center           	0x01
	#define LIB_FONT_FLAG_ALIGN_right            	0x02
	#define LIB_FONT_FLAG_ALIGN_justify          	0x04
	#define	LIB_FONT_FLAG_WEIGHT_bold		0x08

	#define	LIB_FONT_BOLD_level			2

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

	#define	LIB_FONT_TAB_length			2

	// all functions return length of string in pixels

	// main function of printing strings
	uint64_t lib_font( uint8_t font, uint8_t *string, uint64_t length, uint32_t color, uint32_t *pixel, uint64_t scanline_pixel, uint8_t flag );

	uint32_t lib_font_color( uint32_t background, uint32_t foreground );

	uint8_t lib_font_length_char( uint8_t font, uint8_t character );

	// convert value to string and print it
	uint64_t lib_font_value( uint8_t font, uint64_t value, uint8_t base, uint32_t color, uint32_t *pixel, uint64_t scanline_pixel, uint8_t flag );

	// length of string in pixels
	uint64_t lib_font_length_string( uint8_t font, uint8_t *string, uint64_t length );
#endif
