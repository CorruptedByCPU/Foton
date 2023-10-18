/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void console_init( void ) {
	// initialize window interface
	interface.properties = (uint8_t *) &file_interface_start;
	lib_interface( &interface );

// terminal.width			= interface.width - 2;
// terminal.height			= interface.height - (LIB_INTERFACE_HEADER_HEIGHT_pixel + 1);
// terminal.base_address		= (uint32_t *) ((uintptr_t) interface.descriptor + sizeof( struct WM_STRUCTURE_DESCRIPTOR ));
// terminal.scanline_pixel		= interface.width;
// terminal.color_foreground	= STD_COLOR_WHITE;
// terminal.color_background	= 0xFF101010;
// lib_terminal( &terminal );
// lib_terminal_printf( &terminal, (uint8_t *) "\n  Window created by Console.\n  " );
// interface.descriptor -> flags |= WM_OBJECT_FLAG_visible | WM_OBJECT_FLAG_flush;
}