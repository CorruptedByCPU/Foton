/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_object( void ) {
	// search whole list for object flush
	for( uint16_t i = 0; i < wm_list_limit; i++ ) {
		// cursor object located?
		if( wm_list_base_address[ i ] -> descriptor -> flags & WM_OBJECT_FLAG_cursor ) { wm_object_cursor = wm_list_base_address[ i ]; continue; }	// remember

		// object visible and requested flush?
		if( wm_list_base_address[ i ] -> descriptor -> flags & WM_OBJECT_FLAG_visible && wm_list_base_address[ i ] -> descriptor -> flags & WM_OBJECT_FLAG_flush ) {
			// parse whole object area
			wm_zone_insert( (struct WM_STRUCTURE_ZONE *) wm_list_base_address[ i ], FALSE );

			// request parsed
			wm_list_base_address[ i ] -> descriptor -> flags ^= WM_OBJECT_FLAG_flush;

			// redraw cursor too, if exist
			if( wm_object_cursor ) wm_object_cursor -> descriptor -> flags |= WM_OBJECT_FLAG_flush;
		}
	}
}

void wm_object_insert( struct WM_STRUCTURE_OBJECT *object ) {
	// block modification of object list
	while( __sync_val_compare_and_swap( &wm_list_semaphore, UNLOCK, LOCK ) );

	// find available entry or extend object list
	while( TRUE ) {
		// search for empty list entry
		for( uint64_t i = 0; i < wm_list_limit; i++ ) {
			// entry in use?
			if( ! wm_list_base_address[ i ] ) {
				// insert object on list
				wm_list_base_address[ i ] = object;

				// release list for modification
				wm_list_semaphore = UNLOCK;

				// done
				return;
			}
		}

		// no available entry

		// extend object list
		wm_list_base_address = (struct WM_STRUCTURE_OBJECT **) realloc( wm_list_base_address, sizeof( struct WM_STRUCTURE_OBJECT * ) * ++wm_list_limit );
	}
}

struct WM_STRUCTURE_OBJECT *wm_object_create( int16_t x, int16_t y, uint16_t width, uint16_t height ) {
	// block modification of object table
	while( __sync_val_compare_and_swap( &wm_object_semaphore, UNLOCK, LOCK ) );

	// find available entry or extend object table
	while( TRUE ) {
		// search for empty entry inside table
		for( uint64_t i = 0; i < wm_object_limit; i++ ) {
			// entry in use?
			if( ! wm_object_base_address[ i ].descriptor ) {	// no
				// fill object properties
				wm_object_base_address[ i ].x = x;
				wm_object_base_address[ i ].y = y;
				wm_object_base_address[ i ].width = width;
				wm_object_base_address[ i ].height = height;

				// calculate object area size in Bytes
				wm_object_base_address[ i ].size_byte = (width * height * STD_VIDEO_DEPTH_byte) + sizeof( struct WM_STRUCTURE_DESCRIPTOR );

				// assign area for object
				wm_object_base_address[ i ].descriptor = (struct WM_STRUCTURE_DESCRIPTOR *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( wm_object_base_address[ i ].size_byte ) >> STD_SHIFT_PAGE );

				// fill descriptor with known values
				wm_object_base_address[ i ].descriptor -> width = width;
				wm_object_base_address[ i ].descriptor -> height = height;

				// register object on list
				wm_object_insert( (struct WM_STRUCTURE_OBJECT *) &wm_object_base_address[ i ] );

				// newly created object becomes active
				wm_object_active = (struct WM_STRUCTURE_OBJECT *) &wm_object_base_address[ i ];

				// release table for modification
				wm_object_semaphore = UNLOCK;

				// ready
				return wm_object_active;
			}
		}

		// no available entry

		// extend object table
		wm_object_base_address = (struct WM_STRUCTURE_OBJECT *) realloc( wm_object_base_address, sizeof( struct WM_STRUCTURE_OBJECT ) * ++wm_object_limit );
	}
}