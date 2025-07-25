/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void init( void ) {
	// get our PID number
	int64_t d3_pid = std_pid();

	// obtain information about kernel framebuffer
	std_framebuffer( (struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER *) &kernel_framebuffer );

	// alloc area for interface properties
	d3_interface = (struct LIB_INTERFACE_STRUCTURE *) malloc( sizeof( struct LIB_INTERFACE_STRUCTURE ) );

	// framebuffer belongs to us?
	if( d3_pid == kernel_framebuffer.pid ) {
		// look at me, i'm the captain now
		the_master_of_puppets = TRUE;

		// initialize RGL library
		rgl = lib_rgl( kernel_framebuffer.width_pixel, kernel_framebuffer.height_pixel, kernel_framebuffer.width_pixel, kernel_framebuffer.base_address );
	} else {
		// for future window resize, set limits
		d3_interface -> min_width = 1 + 32 + 1;
		d3_interface -> min_height = LIB_INTERFACE_HEADER_HEIGHT_pixel + 32 + 1;

		// initialize interface library
		d3_interface -> properties = (uint8_t *) &file_interface_start;
		if( ! lib_interface( d3_interface ) ) { log( "Cannot create window.\n" ); exit(); }

		// allow window to be resiable
		d3_interface -> descriptor -> flags |= LIB_WINDOW_FLAG_resizable;

		// find control element of type: close
		struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *control = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *) lib_interface_element_by_id( d3_interface, 0 );

		// assign executable function to element
		control -> event = close;

		// initialize RGL library
		rgl = lib_rgl( d3_interface -> width - (LIB_INTERFACE_BORDER_pixel << STD_SHIFT_2), d3_interface -> height - (LIB_INTERFACE_HEADER_HEIGHT_pixel + LIB_INTERFACE_BORDER_pixel), d3_interface -> width, (uint32_t *) ((uintptr_t) d3_interface -> descriptor + sizeof( struct LIB_WINDOW_STRUCTURE ) + (((LIB_INTERFACE_HEADER_HEIGHT_pixel * d3_interface -> width) + LIB_INTERFACE_BORDER_pixel) << STD_VIDEO_DEPTH_shift)) );

		// window content ready for display
		d3_interface -> descriptor -> flags |= LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_flush;
	}
}
