/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void interface( void ) {
	// terminal properties
	struct LIB_TERMINAL_STRUCTURE terminal;

	// update terminal properties
	terminal.width			= descriptor -> width;
	terminal.height			= descriptor -> height;
	terminal.base_address		= (uint32_t *) ((uintptr_t) descriptor + sizeof( struct WM_STRUCTURE_DESCRIPTOR ));
	terminal.scanline_pixel		= descriptor -> width;
	terminal.color_foreground	= STD_COLOR_WHITE;
	terminal.color_background	= STD_COLOR_BLACK_light;

	// initialize terminal
	lib_terminal( &terminal );

	// disable cursor
	lib_terminal_cursor_disable( &terminal );

	while( TRUE ) {
		// unit of time, passed?
		if( std_uptime() > fps_last ) {
			// select new time unit
			fps_last += 1;

			// sum all FPU parsed
			fps_average += fps;
			fps_count++;

			// reset terminal cursor
			terminal.cursor_x = 0;
			terminal.cursor_y = 0;
			lib_terminal_cursor_set( &terminal );

			// show amount of FPU
			lib_terminal_printf( &terminal, (uint8_t *) "Window (%ux%u), FPS: %u (%u) ", D3_WIDTH_pixel, D3_HEIGHT_pixel, fps_average / fps_count, fps );

			// restart
			fps = EMPTY;
		}
	}
}