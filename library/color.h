/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_COLOR
	#define	LIB_COLOR

	// return one of 256 predefinied colors
	uint32_t lib_color( uint8_t index );

	// returns mixed colors
	uint32_t lib_color_blend( uint32_t background, uint32_t foreground );

	// convert HSV values to RGB
	uint32_t lib_color_from_hsv( double h, double s, double v );
#endif
