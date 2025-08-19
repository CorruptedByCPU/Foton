/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_terminal( void ) {
	// convert framebuffer dimension to height and width in characters
	kernel -> terminal.width_char		= kernel -> framebuffer_width_pixel / LIB_FONT_WIDTH_pixel;	// default font width for monospace
	kernel -> terminal.height_char		= kernel -> framebuffer_height_pixel / LIB_FONT_HEIGHT_pixel;

	// terminal scanline in height of character
	kernel -> terminal.scanline_pixel	= (kernel -> framebuffer_pitch_byte >> STD_VIDEO_DEPTH_shift) * LIB_FONT_HEIGHT_pixel;

	// cursor default position
	kernel -> terminal.cursor_x			= EMPTY;
	kernel -> terminal.cursor_y			= EMPTY;

	// default cursor position inside memory
	kernel -> terminal.pointer			= kernel -> terminal.pixel;

	// visualization
	kernel -> terminal.color_background	= kernel_terminal_color( KERNEL_TERMINAL_COLOR_BACKGROUND );
	kernel -> terminal.color_foreground	= kernel_terminal_color( KERNEL_TERMINAL_COLOR_FOREGROUND );

	// check for cases we do not support
	if( kernel -> framebuffer_pitch_byte % STD_SIZE_DWORD_byte || kernel -> framebuffer_bpp != STD_VIDEO_DEPTH_bit ) {
		// show RED line at top of screen
		for( uint64_t x = 0; x < kernel -> framebuffer_width_pixel; x++ ) kernel -> framebuffer_base_address[ x ] = STD_COLOR_RED;

		// hodor
		while( TRUE );
	}

	// clean terminal
	kernel_terminal_clean();
}
