/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_init( void ) {
	// get our PID number
	wm_pid = std_pid();

	// obtain information about kernel framebuffer
	std_framebuffer( &framebuffer );

	// framebuffer locked?
	if( framebuffer.pid != wm_pid ) { log( "WM: Framebuffer is already in use by process with ID %u\nExit.", wm_pid ); while( TRUE ); }

	//----------------------------------------------------------------------

	// prepare space for an array of objects
	wm_object_base_address = (struct WM_STRUCTURE_OBJECT *) malloc( TRUE );

	// prepare space for an array of object list
	wm_list_base_address = (struct WM_STRUCTURE_OBJECT **) malloc( TRUE );

	// prepare space for an list of zones
	wm_zone_base_address = (struct WM_STRUCTURE_ZONE *) malloc( TRUE );

	//----------------------------------------------------------------------

	// create cache space

	// as local object
	wm_object_cache.width		= framebuffer.width_pixel;
	wm_object_cache.height		= framebuffer.height_pixel;
	wm_object_cache.descriptor	= (struct WM_STRUCTURE_DESCRIPTOR *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( (wm_object_cache.width * wm_object_cache.height * STD_VIDEO_DEPTH_byte) + sizeof( struct WM_STRUCTURE_DESCRIPTOR ) ) >> STD_SHIFT_PAGE );

	// leave cache untouched, first objects synchronization will fill it up

	//----------------------------------------------------------------------

	// create workbench object
	wm_object_workbench = wm_object_create( 0, 0, wm_object_cache.width, wm_object_cache.height );

	// fill workbench with default gradient like Xorg :)
	uint32_t *workbench_pixel = (uint32_t *) ((uintptr_t) wm_object_workbench -> descriptor + sizeof( struct WM_STRUCTURE_DESCRIPTOR ));
	for( uint16_t y = 0; y < wm_object_workbench -> height; y++ ) {
		// define colors
		uint32_t colors[ 2 ];

		// regarded of line
		if( y % 2 ) { colors[ 0 ] = STD_COLOR_BLACK_light; colors[ 1 ] = STD_COLOR_WHITE; }
		else { colors[ 0 ] = STD_COLOR_WHITE; colors[ 1 ] = STD_COLOR_BLACK_light; }

		for( uint16_t x = 0; x < wm_object_workbench -> width; x++ ) {
			// select color
			uint32_t color; if( x % 2 ) color = colors[ 0 ]; else color = colors[ 1 ];

			// draw pixel
			workbench_pixel[ (y * wm_object_workbench -> width) + x ] = color;
		}
	}

	// object created
	wm_object_workbench -> descriptor -> flags |= WM_OBJECT_FLAG_visible | WM_OBJECT_FLAG_fixed_xy | WM_OBJECT_FLAG_fixed_z | WM_OBJECT_FLAG_flush;
}