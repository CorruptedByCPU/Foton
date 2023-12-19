/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

int64_t wm_release( void ) {
	// main loop
	while( TRUE ) {
		// free up AP time
		std_sleep( 1 );		

		// // block access to object array
		// uint64_t wait_time = std_microtime();
		// MACRO_LOCK( wm_list_semaphore, UNLOCK, LOCK ) ) if( wait_time + WM_DEBUG_STARVATION_limit < std_uptime() ) { print( "[wm_release is starving]\n" ); }

		// // check every object inside array
		// for( uint64_t i = 0; i < wm_list_limit; i++ ) {
		// 	// object loose connection with process or marked for release?
		// 	if( wm_list_base_address[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_release || ! std_pid_check( wm_list_base_address[ i ] -> pid ) ) {
		// 		// block access to object array
		// 		uint64_t wait_time = std_microtime();
		// 		MACRO_LOCK( wm_object_semaphore, UNLOCK, LOCK ) ) if( wait_time + WM_DEBUG_STARVATION_limit < std_uptime() ) { print( "[wm_release subroutine is starving]\n" ); }

		// 		// mark object as invisible
		// 		wm_list_base_address[ i ] -> descriptor -> flags &= ~STD_WINDOW_FLAG_visible;

		// 		// redraw area of object
		// 		wm_zone_insert( (struct WM_STRUCTURE_ZONE *) wm_list_base_address[ i ], FALSE );

		// 		// redisplay cursor
		// 		wm_object_cursor -> descriptor -> flags |= STD_WINDOW_FLAG_flush;

		// 		// preserve properties of object
		// 		struct WM_STRUCTURE_OBJECT *release = wm_list_base_address[ i ];

		// 		// find object on object list
		// 		for( uint16_t j = 0; j < wm_list_limit; j++ ) {
		// 			// object located?
		// 			if( wm_list_base_address[ j ] != wm_list_base_address[ i ] ) continue;	// no

		// 			// remove object from list (and move all other in place)
		// 			for( uint16_t k = j; k < wm_list_limit - 1; k++ )
		// 				// no, move next object to current position
		// 				wm_list_base_address[ k ] = wm_list_base_address[ k + 1 ];

		// 			// clear last entry
		// 			wm_list_base_address[ --wm_list_limit ] = EMPTY;

		// 			// done
		// 			break;
		// 		}

		// 		// block access to object array
		// 		uint64_t taskbar_wait_time = std_microtime();
		// 		MACRO_LOCK( wm_taskbar_semaphore, UNLOCK, LOCK ) ) if( taskbar_wait_time + WM_DEBUG_STARVATION_limit < std_uptime() ) { print( "[wm_release subroutine is starving]\n" ); }

		// 		// find object on object list
		// 		for( uint16_t j = 0; j < wm_list_limit; j++ ) {
		// 			// object located?
		// 			if( wm_list_base_address[ j ] != release ) continue;	// no

		// 			// remove object from list (and move all other in place)
		// 			for( uint16_t k = j; k < wm_list_limit - 1; k++ )
		// 				// no, move next object to current position
		// 				wm_list_base_address[ k ] = wm_list_base_address[ k + 1 ];

		// 			// clear last entry
		// 			wm_list_base_address[ --wm_list_limit ] = EMPTY;

		// 			// done
		// 			break;
		// 		}

		// 		// find object on taskbar list
		// 		for( uint16_t j = 0; j < wm_taskbar_limit; j++ ) {
		// 			// object located?
		// 			if( wm_taskbar_base_address[ j ] != release ) continue;	// no

		// 			// remove object from list (and move all other in place)
		// 			for( uint16_t k = j; k < wm_taskbar_limit - 1; k++ )
		// 				// no, move next object to current position
		// 				wm_taskbar_base_address[ k ] = wm_taskbar_base_address[ k + 1 ];

		// 			// clear last entry
		// 			wm_taskbar_base_address[ --wm_taskbar_limit ] = EMPTY;

		// 			// done
		// 			break;
		// 		}

		// 		// release acces to object array
		// 		wm_taskbar_semaphore );

		// 		// select new active object
		// 		wm_object_active_new();

		// 		// update taskbar status
		// 		wm_taskbar_modified = TRUE;

		// 		// release old object area
		// 		std_memory_release( (uintptr_t) release -> descriptor, MACRO_PAGE_ALIGN_UP( release -> size_byte ) >> STD_SHIFT_PAGE );

		// 		// release old entry in object table
		// 		release -> descriptor = EMPTY;

		// 		// release acces to object array
		// 		wm_object_semaphore );
		// 	}
		// }

		// // release acces to object array
		// wm_list_semaphore );
	}
}