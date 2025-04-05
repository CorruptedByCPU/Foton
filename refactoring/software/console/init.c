/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void console_init( void ) {
	// get our PID number
	console_pid = std_pid();

	// obtain information about kernel framebuffer
	std_framebuffer( (struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER *) &kernel_framebuffer );

	// alloc area for interface properties
	console_interface = (struct LIB_INTERFACE_STRUCTURE *) malloc( sizeof( struct LIB_INTERFACE_STRUCTURE ) );

	// alloc area for terminal properties
	console_terminal = (struct LIB_TERMINAL_STRUCTURE *) malloc( sizeof( struct LIB_TERMINAL_STRUCTURE ) );

	// framebuffer belongs to us?
	if( console_pid == kernel_framebuffer.pid ) {
		// look at me, i'm the captain now
		console_the_master_of_puppets = TRUE;

		// initialize terminal library
		console_terminal -> width		= kernel_framebuffer.width_pixel;
		console_terminal -> height		= kernel_framebuffer.height_pixel;
		console_terminal -> base_address	= (uint32_t *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( kernel_framebuffer.width_pixel * kernel_framebuffer.height_pixel * STD_VIDEO_DEPTH_byte ) >> STD_SHIFT_PAGE );
		console_terminal -> scanline_pixel	= kernel_framebuffer.width_pixel;
		console_terminal -> alpha		= EMPTY;	// 0 - not transparent, 255 - fully transparent
		console_terminal -> color_foreground	= lib_color( 255 );
		console_terminal -> color_background	= lib_color( 232 );
		lib_terminal( console_terminal );
	} else {
		// for future window resize, set limits
		console_interface -> min_width = 1 + (LIB_FONT_WIDTH_pixel << STD_SHIFT_16) + 1;
		console_interface -> min_height = LIB_INTERFACE_HEADER_HEIGHT_pixel + (LIB_FONT_HEIGHT_pixel << STD_SHIFT_2) + 1;

		// initialize interface library
		console_interface -> properties = (uint8_t *) &file_interface_start;
		console_interface -> background_color = lib_color( 232 );
		if( ! lib_interface( console_interface ) ) { log( "Cannot create window.\n" ); exit(); }

		// allow window to be resiable
		console_interface -> descriptor -> flags |= STD_WINDOW_FLAG_resizable;

		// find control element of type: close
		struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *control = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *) lib_interface_element_by_id( console_interface, 0 );

		// assign executable function to element
		control -> event = (void *) close;

		// initialize terminal library
		console_terminal -> width		= console_interface -> width - (LIB_INTERFACE_BORDER_pixel << STD_SHIFT_2);
		console_terminal -> height		= console_interface -> height - (LIB_INTERFACE_HEADER_HEIGHT_pixel + 1);
		console_terminal -> base_address	= (uint32_t *) ((uintptr_t) console_interface -> descriptor + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR ) + (((LIB_INTERFACE_HEADER_HEIGHT_pixel * console_interface -> width) + LIB_INTERFACE_BORDER_pixel) << STD_VIDEO_DEPTH_shift));
		console_terminal -> scanline_pixel	= console_interface -> width;
		console_terminal -> alpha		= 32;
		console_terminal -> color_foreground	= lib_color( 255 );
		console_terminal -> color_background	= lib_color( 232 );
		lib_terminal( console_terminal );

		// update window content on screen
		console_interface -> descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;
	}

	// default meta properties of stream
	console_stream_meta.width = console_terminal -> width_char;
	console_stream_meta.height = console_terminal -> height_char;

	// set default meta data of input stream
	std_stream_set( (uint8_t *) &console_stream_meta, STD_STREAM_IN );

	// prepare cache for incomming stream
	console_stream_in = (uint8_t *) malloc( STD_STREAM_SIZE_page << STD_SHIFT_PAGE );
}