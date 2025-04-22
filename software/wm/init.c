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
	for( uint16_t y = 0; y < wm_object_workbench -> height; y++ ) {
		for( uint16_t x = 0; x < wm_object_workbench -> width; x++ ) {
			uint32_t color = 0xFFFFFFFF;
			if( x % 2 ) color = 0xFF000000;
			workbench_pixel[ (y * wm_object_workbench -> width) + x ] = color;
		}
	}

	// object content ready for display
	wm_object_workbench -> descriptor -> flags |= STD_WINDOW_FLAG_fixed_z | STD_WINDOW_FLAG_fixed_xy | STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;

	//----------------------------------------------------------------------

	// create cursor object
	wm_object_cursor = wm_object_create( wm_object_workbench -> width >> STD_SHIFT_2, wm_object_workbench -> height >> STD_SHIFT_2, 16, 32 );

	// mark it as our
	wm_object_cursor -> pid = wm_pid;

	// properties of cursor area content
	uint32_t *cursor_pixel = (uint32_t *) ((uintptr_t) wm_object_cursor -> descriptor + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR ));

	// fill cursor with default color
	for( uint16_t y = 0; y < wm_object_cursor -> height; y++ )
		for( uint16_t x = 0; x < wm_object_cursor -> width; x++ )
			cursor_pixel[ (y * wm_object_cursor -> width) + x ] = STD_COLOR_WHITE;

	// mark window as cursor, so Window Manager will treat it different than others
	wm_object_cursor -> descriptor -> flags |= STD_WINDOW_FLAG_cursor;

	// yep, any object can be a cursor :) but only 1 with highest internal ID will be treated as
	// so before you assign a flag to another object, make sure no other objects have it

	// object content ready for display
	wm_object_cursor -> descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;

	//----------------------------------------------------------------------

	// done
	return EMPTY;
}
