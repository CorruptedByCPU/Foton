/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_release( void ) {
	// block access to object array
	MACRO_LOCK( wm_list_semaphore );

	// check every object inside array
	for( uint64_t i = 0; i < wm_list_limit; i++ ) {
		// object loose connection with process or marked for release?
		if( wm_list_base_address[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_release || ! std_pid_check( wm_list_base_address[ i ] -> pid ) ) {
			// block access to object array
			MACRO_LOCK( wm_object_semaphore );

			// block access to object array
			MACRO_LOCK( wm_taskbar_semaphore );

			// mark object as invisible
			wm_list_base_address[ i ] -> descriptor -> flags &= ~STD_WINDOW_FLAG_visible;

			// redraw area of object
			wm_zone_insert( (struct WM_STRUCTURE_ZONE *) wm_list_base_address[ i ], FALSE );

			// redisplay cursor
			wm_object_cursor -> descriptor -> flags |= STD_WINDOW_FLAG_flush;

			// preserve properties of object
			struct WM_STRUCTURE_OBJECT *release = wm_list_base_address[ i ];

			// find object on object list
			for( uint16_t j = i; j < wm_list_limit; j++ )
				// next object object to current position
				wm_list_base_address[ j ] = wm_list_base_address[ j + 1 ];

			// clear last entry
			wm_list_base_address[ --wm_list_limit ] = EMPTY;

			// find object on taskbar list
			for( uint16_t j = 0; j < wm_taskbar_limit; j++ ) {
				// object located?
				if( wm_taskbar_base_address[ j ] != release ) continue;	// no

				// remove object from list (and move all other in place)
				for( uint16_t k = j; k < wm_taskbar_limit; k++ )
					// move next object to current position
					wm_taskbar_base_address[ k ] = wm_taskbar_base_address[ k + 1 ];

				// clear last entry
				wm_taskbar_base_address[ --wm_taskbar_limit ] = EMPTY;

				// done
				break;
			}

			// select new active object
			wm_object_active_new();

			// update taskbar status
			wm_taskbar_modified = TRUE;

			// release old object area
			std_memory_release( (uintptr_t) release -> descriptor, MACRO_PAGE_ALIGN_UP( release -> size_byte ) >> STD_SHIFT_PAGE );

			// release old entry in object table
			release -> descriptor = EMPTY;

			// release access to object array
			MACRO_UNLOCK( wm_taskbar_semaphore );

			// release access to object array
			MACRO_UNLOCK( wm_object_semaphore );
		}
	}

	// release access to object array
	MACRO_UNLOCK( wm_list_semaphore );
}