/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void console_init( void ) {
	// initialize interface library
	interface.properties = (uint8_t *) &file_interface_start;
	lib_interface( &interface );

	// initialize terminal library
	terminal.width			= interface.width - (LIB_INTERFACE_BORDER_pixel << STD_SHIFT_2);
	terminal.height			= interface.height - (LIB_INTERFACE_HEADER_HEIGHT_pixel + 1);
	terminal.base_address		= (uint32_t *) ((uintptr_t) interface.descriptor + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR ) + (((LIB_INTERFACE_HEADER_HEIGHT_pixel * interface.width) + LIB_INTERFACE_BORDER_pixel) << STD_VIDEO_DEPTH_shift));
	terminal.scanline_pixel		= interface.width;
	terminal.color_foreground	= STD_COLOR_WHITE;
	terminal.color_background	= STD_COLOR_BLACK_light;
	lib_terminal( &terminal );

	// show welcome
	lib_terminal_printf( &terminal, (uint8_t *) "Window created by Console.\n" );

	// update window content on screen
	interface.descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;
}