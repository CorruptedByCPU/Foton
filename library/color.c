/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint32_t lib_color_blend( uint32_t background, uint32_t foreground ) {
	// little scary :)
	uint32_t a = (foreground & 0xFF000000) >> 24;
	return ((((((255 - a) * (background & 0x00FF00FF)) + (a * (foreground & 0x00FF00FF))) >> 8) & 0x00FF00FF) | ((((255 - a) * ((background & (0xFF000000 | 0x0000FF00)) >> 8)) + (a * (0x01000000 | ((foreground & 0x0000FF00) >> 8)))) & (0xFF000000 | 0x0000FF00)));
}