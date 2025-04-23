/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint64_t wm_init( void ) {
	// retrieve our process ID
	wm_pid = std_pid();

	// retrieve properties of kernel framebuffer
	std_framebuffer( (struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER *) &wm_framebuffer );

	// kernel framebuffer locked by someone else?
	if( wm_pid != wm_framebuffer.pid ) return STD_ERROR_locked;	// yep

	// assign area for an array of objects
	wm_object_base_address = (struct WM_STRUCTURE_OBJECT *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct WM_STRUCTURE_OBJECT ) * WM_OBJECT_LIMIT ) >> STD_SHIFT_PAGE );

	// assign area for a list of objects
	wm_list_base_address = (struct WM_STRUCTURE_OBJECT **) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct WM_STRUCTURE_OBJECT * ) * (WM_LIST_LIMIT + TRUE) ) >> STD_SHIFT_PAGE );	// blank entry at end list

	// assign area for a list of zones
	wm_zone_base_address = (struct WM_STRUCTURE_ZONE *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct WM_STRUCTURE_ZONE ) * WM_ZONE_LIMIT ) >> STD_SHIFT_PAGE );

	//----------------------------------------------------------------------

	// create cache area
	wm_object_cache.width		= wm_framebuffer.width_pixel;
	wm_object_cache.height		= wm_framebuffer.height_pixel;
	wm_object_cache.descriptor	= (struct STD_STRUCTURE_WINDOW_DESCRIPTOR *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( (wm_object_cache.width * wm_object_cache.height * STD_VIDEO_DEPTH_byte) + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR ) ) >> STD_SHIFT_PAGE );

	// leave cache untouched, first object synchronization will fill it up

	//----------------------------------------------------------------------

	// create workbench object
	wm_object_workbench = wm_object_create( 0, 0, wm_object_cache.width, wm_object_cache.height );

	// mark it as our
	wm_object_workbench -> pid = wm_pid;

	// properties of workbench area content
	uint32_t *workbench_pixel = (uint32_t *) ((uintptr_t) wm_object_workbench -> descriptor + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR ));

	// fill workbench with default color
	for( uint16_t y = 0; y < wm_object_workbench -> height; y++ )
		for( uint16_t x = 0; x < wm_object_workbench -> width; x++ )
			workbench_pixel[ (y * wm_object_workbench -> width) + x ] = STD_COLOR_BLACK;

	// object content ready for display
	wm_object_workbench -> descriptor -> flags |= STD_WINDOW_FLAG_fixed_z | STD_WINDOW_FLAG_fixed_xy | STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;

	//----------------------------------------------------------------------

	// done
	return EMPTY;
}
