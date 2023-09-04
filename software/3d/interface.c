/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void interface( void ) {
	// terminal properties
	struct LIB_TERMINAL_STRUCTURE terminal;

	// update terminal properties
	terminal.width			= framebuffer.width_pixel;
	terminal.height			= framebuffer.height_pixel;
	terminal.base_address		= framebuffer.base_address;
	terminal.scanline_pixel		= framebuffer.pitch_byte >> STD_VIDEO_DEPTH_shift;
	terminal.color_foreground	= STD_COLOR_WHITE;
	terminal.color_background	= STD_COLOR_BLACK;

	// initialize terminal
	lib_terminal( &terminal );

	// disable cursor
	lib_terminal_cursor_disable( &terminal );

	while( TRUE ) {
		// unit of time, passed?
		if( std_uptime() > fpu_last ) {
			// select new time unit
			fpu_last += 1;

			// sum all FPU parsed
			fpu_average += fpu;
			fpu_count++;

			// reset terminal cursor
			terminal.cursor_x = 0;
			terminal.cursor_y = 0;
			lib_terminal_cursor_set( &terminal );

			// show amount of FPU
			lib_terminal_printf( &terminal, (uint8_t *) "FPu: %u (%u) ", fpu_average / fpu_count, fpu );

			// restart
			fpu = EMPTY;
		}
	}
}