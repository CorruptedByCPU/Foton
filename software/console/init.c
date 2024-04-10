/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void console_init( void ) {
	// get our PID number
	console_pid = std_pid();

	// obtain information about kernel framebuffer
	std_framebuffer( (struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER *) &kernel_framebuffer );

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
		console_interface -> min_width = (LIB_INTERFACE_SHADOW_length << STD_SHIFT_2) + (LIB_FONT_WIDTH_pixel << STD_SHIFT_16);
		console_interface -> min_height = LIB_INTERFACE_HEADER_HEIGHT_pixel + 1 + (LIB_FONT_HEIGHT_pixel << STD_SHIFT_2);

		// initialize interface library
		console_interface -> properties = (uint8_t *) &file_interface_start;
		lib_interface( console_interface );

		// find control element of type: close
		struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *control = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *) lib_interface_element_by_id( console_interface, 0 );

		// assign executable function to element
		control -> event = (void *) close;

		// initialize terminal library
		console_terminal -> width		= console_interface -> width - ((LIB_INTERFACE_BORDER_pixel + console_interface -> descriptor -> offset) << STD_SHIFT_2);
		console_terminal -> height		= console_interface -> height - (LIB_INTERFACE_HEADER_HEIGHT_pixel + 1 + (console_interface -> descriptor -> offset << STD_SHIFT_2));
		console_terminal -> base_address	= (uint32_t *) ((uintptr_t) console_interface -> descriptor + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR ) + ((((LIB_INTERFACE_HEADER_HEIGHT_pixel + console_interface -> descriptor -> offset) * console_interface -> width) + LIB_INTERFACE_BORDER_pixel + console_interface -> descriptor -> offset) << STD_VIDEO_DEPTH_shift));
		console_terminal -> scanline_pixel	= console_interface -> width;
		console_terminal -> alpha		= 0x20;	// 32 of 255, background slightly transparent
		console_terminal -> color_foreground	= lib_color( 255 );
		console_terminal -> color_background	= lib_color( 232 );
		lib_terminal( console_terminal );

		// update window content on screen
		console_interface -> descriptor -> flags |= STD_WINDOW_FLAG_resizable | STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;
	}

	// default meta properties of stream
	console_stream_meta.width = console_terminal -> width_char;
	console_stream_meta.height = console_terminal -> height_char;

	// set default meta data of input stream
	std_stream_set( (uint8_t *) &console_stream_meta, STD_STREAM_IN );

	// prepare cache for incomming stream
	console_stream_in = (uint8_t *) malloc( STD_STREAM_SIZE_page << STD_SHIFT_PAGE );
}