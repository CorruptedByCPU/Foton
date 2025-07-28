/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_FONT
	#define	LIB_FONT

	#include	"./font/config.h"

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
