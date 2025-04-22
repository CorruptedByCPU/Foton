/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_release( void ) {
	// check every object inside array
	for( uint64_t i = INIT; i < wm_list_limit; i++ ) {
		// object loose connection with process or marked for release?
		if( std_pid_check( wm_list_base_address[ i ] -> pid ) && ! (wm_list_base_address[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_release) ) continue;	// no

		// mark object as invisible
		wm_list_base_address[ i ] -> descriptor -> flags &= ~STD_WINDOW_FLAG_visible;

		// redraw area of object
		wm_zone_insert( (struct WM_STRUCTURE_ZONE *) wm_list_base_address[ i ], FALSE );

		// preserve properties of object
		struct WM_STRUCTURE_OBJECT *release = wm_list_base_address[ i ];

		// find object on object list
		for( uint16_t j = i; j < wm_list_limit; j++ )
			// next object object to current position
			wm_list_base_address[ j ] = wm_list_base_address[ j + 1 ];

		// clear last entry
		wm_list_base_address[ --wm_list_limit ] = EMPTY;

		// select new active object
		wm_object_current();

		// release old object area
		std_memory_release( (uintptr_t) release -> descriptor, MACRO_PAGE_ALIGN_UP( release -> size_byte ) >> STD_SHIFT_PAGE );

		// release old entry in object table
		release -> descriptor = EMPTY;

		// redisplay cursor
		wm_object_cursor -> descriptor -> flags |= STD_WINDOW_FLAG_flush;
	}
}
