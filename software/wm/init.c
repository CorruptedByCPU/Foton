/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_init( void ) {
	// get our PID number
	wm_pid = std_pid();

	// obtain information about kernel framebuffer
	std_framebuffer( &framebuffer );

	// framebuffer locked?
	if( framebuffer.owner_pid != wm_pid ) { log( "WM: Framebuffer is already in use by process with ID %u\nExit.", wm_pid ); while( TRUE ); }

	//----------------------------------------------------------------------

	// prepare space for an array of objects
	wm_object_base_address = (struct WM_STRUCTURE_OBJECT *) malloc( TRUE );

	// prepare space for an array of object list
	wm_list_base_address = (struct WM_STRUCTURE_OBJECT **) malloc( TRUE );

	// prepare space for an list of zones
	wm_zone_base_address = (struct WM_STRUCTURE_ZONE *) malloc( TRUE );

	//----------------------------------------------------------------------

	// create framebuffer space

	// as local object
	wm_object_framebuffer.width		= framebuffer.width_pixel;
	wm_object_framebuffer.height		= framebuffer.height_pixel;
	wm_object_framebuffer.descriptor	= (struct WM_STRUCTURE_DESCRIPTOR *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( ((wm_object_framebuffer.width * wm_object_framebuffer.height) << STD_VIDEO_DEPTH_shift) + sizeof( struct WM_STRUCTURE_DESCRIPTOR ) ) >> STD_SHIFT_PAGE );

	//----------------------------------------------------------------------

	// create workbench object
	wm_object_workbench = wm_object_create( 0, 0, framebuffer.width_pixel, framebuffer.height_pixel );

	// fill workbench with default gradient
	uint32_t *workbench_pixel = (uint32_t *) ((uintptr_t) wm_object_workbench -> descriptor + sizeof( struct WM_STRUCTURE_DESCRIPTOR ));
	for( uint16_t y = 0; y < wm_object_workbench -> height; y++ )
		for( uint16_t x = 0; x < wm_object_workbench -> width; x++ )
			workbench_pixel[ (y * wm_object_workbench -> width) + x ] = STD_COLOR_BLACK;

	// object created
	wm_object_workbench -> descriptor -> flags |= WM_OBJECT_FLAG_visible | WM_OBJECT_FLAG_fixed_xy | WM_OBJECT_FLAG_fixed_z | WM_OBJECT_FLAG_flush;

	//----------------------------------------------------------------------

	// create cursor object
	wm_object_cursor = wm_object_create( 0, 0, 32, 32 );

	// fill cursor with default template
	uint32_t *cursor_pixel = (uint32_t *) ((uintptr_t) wm_object_cursor -> descriptor + sizeof( struct WM_STRUCTURE_DESCRIPTOR ));
	uint32_t *cursor_template = (uint32_t *) &wm_cursor_default;
	for( uint16_t y = 0; y < 19; y++ )
		for( uint16_t x = 0; x < 12; x++ )
			cursor_pixel[ (y * wm_object_cursor -> width) + x ] = cursor_template[ (y * 12) + x ] | STD_COLOR_mask;

	// object created
	wm_object_cursor -> descriptor -> flags |= WM_OBJECT_FLAG_visible | WM_OBJECT_FLAG_flush;
}