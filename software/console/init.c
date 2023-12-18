/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void console_init( void ) {
	// get our PID number
	console_pid = std_pid();

	// obtain information about kernel framebuffer
	std_framebuffer( (struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER *) &kernel_framebuffer );

	// framebuffer belongs to us?
	if( console_pid == kernel_framebuffer.pid ) {
		// look at me, i'm the captain now
		console_the_master_of_puppets = TRUE;

		// initialize terminal library
		console_terminal.width			= kernel_framebuffer.width_pixel;
		console_terminal.height			= kernel_framebuffer.height_pixel;
		console_terminal.base_address		= (uint32_t *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( kernel_framebuffer.width_pixel * kernel_framebuffer.height_pixel * STD_VIDEO_DEPTH_byte ) >> STD_SHIFT_PAGE );
		console_terminal.scanline_pixel		= kernel_framebuffer.width_pixel;
		console_terminal.alpha			= EMPTY;	// 0 - not transparent, 255 - fully transparent
		console_terminal.color_foreground	= lib_color( 255 );
		console_terminal.color_background	= lib_color( 232 );
		lib_terminal( (struct LIB_TERMINAL_STRUCTURE *) &console_terminal );
	} else {
		// initialize interface library
		console_interface.properties = (uint8_t *) &file_interface_start;
		lib_interface( (struct LIB_INTERFACE_STRUCTURE *) &console_interface );

		// initialize terminal library
		console_terminal.width			= console_interface.width - (LIB_INTERFACE_BORDER_pixel << STD_SHIFT_2);
		console_terminal.height			= console_interface.height - (LIB_INTERFACE_HEADER_HEIGHT_pixel + 1);
		console_terminal.base_address		= (uint32_t *) ((uintptr_t) console_interface.descriptor + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR ) + (((LIB_INTERFACE_HEADER_HEIGHT_pixel * console_interface.width) + LIB_INTERFACE_BORDER_pixel) << STD_VIDEO_DEPTH_shift));
		console_terminal.scanline_pixel		= console_interface.width;
		console_terminal.alpha			= 0x20;	// 32 of 255, background slightly transparent
		console_terminal.color_foreground	= lib_color( 255 );
		console_terminal.color_background	= lib_color( 232 );
		lib_terminal( (struct LIB_TERMINAL_STRUCTURE *) &console_terminal );

		// update window content on screen
		console_interface.descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;
	}

	// default meta properties of stream
	console_stream_meta.width = console_terminal.width_char;
	console_stream_meta.height = console_terminal.height_char;

	// set default meta data of input stream
	std_stream_set( (uint8_t *) &console_stream_meta, STD_STREAM_IN );

	// run Shell program
	console_pid_of_shell = std_exec( (uint8_t *) "shell", 5, STD_STREAM_FLOW_out_to_parent_in );

	// prepare cache for incomming stream
	console_stream_in = (uint8_t *) malloc( STD_STREAM_SIZE_page << STD_SHIFT_PAGE );
}