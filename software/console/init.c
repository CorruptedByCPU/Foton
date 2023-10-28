/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void console_init( void ) {
	// initialize interface library
	console_interface.properties = (uint8_t *) &file_interface_start;
	lib_interface( (struct LIB_INTERFACE_STRUCTURE *) &console_interface );

	// initialize terminal library
	console_terminal.width			= console_interface.width - (LIB_INTERFACE_BORDER_pixel << STD_SHIFT_2);
	console_terminal.height			= console_interface.height - (LIB_INTERFACE_HEADER_HEIGHT_pixel + 1);
	console_terminal.base_address		= (uint32_t *) ((uintptr_t) console_interface.descriptor + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR ) + (((LIB_INTERFACE_HEADER_HEIGHT_pixel * console_interface.width) + LIB_INTERFACE_BORDER_pixel) << STD_VIDEO_DEPTH_shift));
	console_terminal.scanline_pixel		= console_interface.width;
	console_terminal.color_foreground	= STD_COLOR_WHITE;
	console_terminal.color_background	= STD_COLOR_BLACK;
	lib_terminal( (struct LIB_TERMINAL_STRUCTURE *) &console_terminal );

	// update window content on screen
	console_interface.descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;

	// run Shell program
	console_pid_of_shell = std_exec( (uint8_t *) "shell", 5, STD_STREAM_FLOW_out_to_parent_in );

	// prepare cache for incomming stream
	console_stream_in = (uint8_t *) malloc( STD_STREAM_SIZE_page << STD_SHIFT_PAGE );

}