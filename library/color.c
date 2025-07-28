/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#ifndef	LIB_COLOR
		#include	"./color.h"
	#endif
	#include	"./math.h"
	//======================================================================

uint32_t lib_color_palette[ 16 ] = {
	STD_COLOR_BLACK,
	STD_COLOR_RED,
	STD_COLOR_GREEN,
	STD_COLOR_BROWN,
	STD_COLOR_BLUE,
	STD_COLOR_MAGENTA,
	STD_COLOR_CYAN,
	STD_COLOR_GRAY_LIGHT,
	STD_COLOR_GRAY,
	STD_COLOR_RED_LIGHT,
	STD_COLOR_GREEN_LIGHT,
	STD_COLOR_YELLOW,
	STD_COLOR_BLUE_LIGHT,
	STD_COLOR_MAGENTA_LIGHT,
	STD_COLOR_CYAN_LIGHT,
	STD_COLOR_WHITE
};

uint8_t lib_color_interval[ 6 ] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };

uint32_t lib_color( uint8_t index ) {
	// initialize colors
	uint8_t red, green, blue;

	// select palette
	if( index < 16 )
		// predefinied color
		return lib_color_palette[ index ];
	else if( index < 232 ) {
		// calculate colors
		red	= lib_color_interval[ (index - 16) / 36 ];
		green	= lib_color_interval[ ((index - 16) % 36) / 6 ];
		blue	= lib_color_interval[ ((index - 16) % 36) % 6 ];
	} else
		// one of gray scale
		red = green = blue = ((index - 232) * 10) + 8;

	// convert to hexadecimal value
	return STD_COLOR_mask | red << 16 | green << 8 | blue;
}

uint32_t lib_color_blend( uint32_t background, uint32_t foreground ) {
	// little scary :)
	uint32_t a = (foreground & 0xFF000000) >> 24;
	return ((((((255 - a) * (background & 0x00FF00FF)) + (a * (foreground & 0x00FF00FF))) >> 8) & 0x00FF00FF) | ((((255 - a) * ((background & (0xFF000000 | 0x0000FF00)) >> 8)) + (a * (0x01000000 | ((foreground & 0x0000FF00) >> 8)))) & (0xFF000000 | 0x0000FF00)));
}

double fmod(double x, double y) {
    return x - (double)((long long)(x / y)) * y;
}

uint32_t lib_color_hsv( double h, double s, double b ) {
	// keep in range
    	while( h < 0.0f ) h += 360.0f;
	while( h >= 360.0f ) h -= 360.0f;

	// calculate chroma
    	double c = b * s;

	// color
	uint8_t i = (uint64_t) (h / 60.0f);

 	// interpolate sector color from normalized Hue
	double f = (h / 60.0f) - i;

	// color samples
    	double s0 = b - c;
    	double s1 = b - c * f;
    	double s2 = b - c * (1.0f - f);

	// fold color
	double tr, tg, tb;
	switch( i ) {
		case 0: { tr = b; tg = s2; tb = s0; break; }
		case 1: { tr = s1; tg = b; tb = s0; break; }
		case 2: { tr = s0; tg = b; tb = s2; break; }
		case 3: { tr = s0; tg = s1; tb = b; break; }
		case 4: { tr = s2; tg = s0; tb = b; break; }
		case 5: { tr = b; tg = s0; tb = s1; break; }
	}

	// ready
	return 0xFF000000 | ((uint8_t) (tr * 255.0f)) << 16 | ((uint8_t) (tg * 255.0f)) << 8 | (uint8_t) (tb * 255.0f);
}
