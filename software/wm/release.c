/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_release( void ) {
	// check every object on list
	for( uint64_t i = 0; i < wm -> list_limit; i++ ) {
		// object loose connection with parent? or set for release?
		if( std_pid_exist( wm -> list[ i ] -> pid ) && ! (wm -> list[ i ] -> descriptor -> flags & LIB_WINDOW_FLAG_release) ) continue;	// no

		// redraw area of object
		wm_zone_insert( (struct WM_STRUCTURE_ZONE *) wm -> list[ i ], FALSE );

		// remove entry from panel list
		wm_panel_remove( wm -> list[ i ] );

		// release object area
		std_memory_release( (uintptr_t) wm -> list[ i ] -> descriptor, MACRO_PAGE_ALIGN_UP( wm -> list[ i ] -> limit ) >> STD_SHIFT_PAGE );

		// release old entry in object table
		wm -> list[ i ] -> descriptor = EMPTY;

		// remove object from object list
		for( uint16_t j = i; j < wm -> list_limit; j++ ) wm -> list[ j ] = wm -> list[ j + 1 ]; wm -> list[ --wm -> list_limit ] = EMPTY;

		// redisplay cursor
		wm -> cursor -> descriptor -> flags |= LIB_WINDOW_FLAG_flush;

		//--------------------------------------------------------------

		// select new active object
		wm_object_activate();

		// refresh panel content
		wm -> panel_semaphore = TRUE;
	}
}
