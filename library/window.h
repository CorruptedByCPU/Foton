/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef LIB_WINDOW
	#define	LIB_WINDOW

	#define	LIB_WINDOW_BORDER_pixel				1
	#define	LIB_WINDOW_BORDER_COLOR_default			0xFF282828
	#define	LIB_WINDOW_BORDER_COLOR_default_shadow		0xFF202020
	#define	LIB_WINDOW_BORDER_COLOR_inactive		LIB_WINDOW_BORDER_COLOR_default_shadow
	#define	LIB_WINDOW_BORDER_COLOR_inactive_shadow		0xFF181818
	
	struct STD_STRUCTURE_WINDOW_DESCRIPTOR *lib_window( int16_t x, int16_t y, uint16_t width, uint16_t height );

	struct STD_STRUCTURE_WINDOW_DESCRIPTOR *lib_window_event( struct STD_STRUCTURE_WINDOW_DESCRIPTOR *descriptor );

	void lib_window_name( struct STD_STRUCTURE_WINDOW_DESCRIPTOR *descriptor, uint8_t *name );
#endif
