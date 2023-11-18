/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_object( void ) {
	// search whole list for object flush
	for( uint16_t i = 0; i < wm_list_limit; i++ ) {
		// ignore cursor object
		if( wm_list_base_address[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_cursor ) continue;

		// object visible and requested flush?
		if( wm_list_base_address[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_minimize || (wm_list_base_address[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_visible && wm_list_base_address[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_flush) ) {
			// parse whole object area
			wm_zone_insert( (struct WM_STRUCTURE_ZONE *) wm_list_base_address[ i ], FALSE );

			// request parsed
			wm_list_base_address[ i ] -> descriptor -> flags ^= STD_WINDOW_FLAG_flush;

			// remove minimize flag (even if not set)
			wm_list_base_address[ i ] -> descriptor -> flags &= ~STD_WINDOW_FLAG_minimize;

			// redraw cursor too
			wm_object_cursor -> descriptor -> flags |= STD_WINDOW_FLAG_flush;
		}

		// object still in touch with process?
		if( ! std_pid_check( wm_list_base_address[ i ] -> pid ) ) {	// no
			// object invisible and marked for delete?
			if( ! (wm_list_base_address[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_visible) && wm_list_base_address[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_release ) {
				// release object
				wm_object_remove( i );

				// update taskbar list
				wm_taskbar_semaphore = TRUE;
			}

			// object visible?
			if( wm_list_base_address[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_visible ) {
				// hide it!
				wm_list_base_address[ i ] -> descriptor -> flags &= ~STD_WINDOW_FLAG_visible;
				wm_zone_insert( (struct WM_STRUCTURE_ZONE *) wm_list_base_address[ i ], FALSE );

				// mark object for delete
				wm_list_base_address[ i ] -> descriptor -> flags |= STD_WINDOW_FLAG_release;
			}
		}

		// object renamed?
		if( wm_list_base_address[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_name ) {
			// done
			wm_list_base_address[ i ] -> descriptor -> flags ^= STD_WINDOW_FLAG_name;

			// update taskbar list
			wm_taskbar_semaphore = TRUE;
		}
	}
}

void wm_object_active_new( void ) {
	// search thru object list as far as to taskbar object
	for( uint16_t i = 0; i < wm_list_limit; i++ ) {
		// taskbar object?
		if( wm_list_base_address[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_taskbar ) break;	// yes

		// object is visible?
		if( wm_list_base_address[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_visible ) wm_object_active = wm_list_base_address[ i ];
	}
}

void wm_object_insert( struct WM_STRUCTURE_OBJECT *object ) {
	// block modification of object list
	while( __sync_val_compare_and_swap( &wm_list_semaphore, UNLOCK, LOCK ) );

	// extend object list
	wm_list_base_address = (struct WM_STRUCTURE_OBJECT **) realloc( wm_list_base_address, sizeof( struct WM_STRUCTURE_OBJECT * ) * ++wm_list_limit );

	// find available entry
	for( uint64_t i = 0; i < wm_list_limit; i++ ) {
		// entry in use?
		if( wm_list_base_address[ i ] ) {
			// by taskbar?
			if( wm_list_base_address[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_taskbar ) {
				// move all objects from this point on list including taskbar one place forward
				for( uint64_t j = wm_list_limit - 1; j > i; j-- ) wm_list_base_address[ j ] = wm_list_base_address[ j - 1 ];
			// next entry
			} else continue;
		}

		// insert object on list
		wm_list_base_address[ i ] = object;

		// update taskbar list
		wm_taskbar_semaphore = TRUE;

		// release list for modification
		wm_list_semaphore = UNLOCK;

		// done
		return;
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
				wm_object_base_address[ i ].x		= x;
				wm_object_base_address[ i ].y		= y;
				wm_object_base_address[ i ].width	= width;
				wm_object_base_address[ i ].height	= height;

				// calculate object area size in Bytes
				wm_object_base_address[ i ].size_byte = (width * height * STD_VIDEO_DEPTH_byte) + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR );

				// assign area for object
				wm_object_base_address[ i ].descriptor = (struct STD_WINDOW_STRUCTURE_DESCRIPTOR *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( wm_object_base_address[ i ].size_byte ) >> STD_SHIFT_PAGE );

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

struct WM_STRUCTURE_OBJECT *wm_object_find( uint16_t x, uint16_t y, uint8_t parse_hidden ) {
	// find object at current cursor coordinates
	for( uint16_t i = wm_list_limit - 1; i >= 0; i-- ) {
		// object marked as cursor?
		if( wm_list_base_address[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_cursor ) continue;	// leave it

		// object is visible? (or include hidden ones too)
		if( parse_hidden || wm_list_base_address[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_visible ) {
			// coordinates at object area?
			if( wm_list_base_address[ i ] -> x > x ) continue;	// no
			if( wm_list_base_address[ i ] -> y > y ) continue;	// no
			if( (wm_list_base_address[ i ] -> x + wm_list_base_address[ i ] -> width) < x ) continue;	// no
			if( (wm_list_base_address[ i ] -> y + wm_list_base_address[ i ] -> height) < y ) continue;	// no

			// return a pointer to an object
			return wm_list_base_address[ i ];
		}
	}

	// nothing under specified coordinates
	return EMPTY;
}

void wm_object_move( int16_t x, int16_t y ) {
	// object attached to XY axis?
	if( wm_object_selected -> descriptor -> flags & STD_WINDOW_FLAG_fixed_xy ) return;	// yep

	// prepare zone for truncate
	struct WM_STRUCTURE_ZONE zone;
	zone.x = wm_object_selected -> x;
	zone.y = wm_object_selected -> y;
	zone.width = wm_object_selected -> width;
	zone.height = wm_object_selected -> height;

	// a movement on X axis occurred?
	if( x ) {
		// new position of selected object
		wm_object_selected -> x += x;

		// X axis shift is positive?
		if( x > 0 )	// yes
			// width of exposed zone
			zone.width = x;
		else {
			// position and width of exposed zone
			zone.x = wm_object_selected -> x + wm_object_selected -> width;
			zone.width = ~x + 1;
		}

		// register zone
		wm_zone_insert( (struct WM_STRUCTURE_ZONE *) &zone, FALSE );

		// update zone properties
		zone.x = wm_object_selected -> x;
		zone.width = wm_object_selected -> width;
	}

	// a movement on Y axis occured?
	if( y ) {
		// new position of selected object
		wm_object_selected -> y += y;

		// Y axis shift is positive?
		if( y > 0 )	// yes
			// height of exposed fragment
			zone.height = y;
		else {
			// position and height of exposed fragment
			zone.y = wm_object_selected -> y + wm_object_selected -> height;
			zone.height = ~y + 1;
		}

		// register zone
		wm_zone_insert( (struct WM_STRUCTURE_ZONE *) &zone, FALSE );
	}

	// object has been moved
	wm_object_selected -> descriptor -> flags |= STD_WINDOW_FLAG_flush;
}

uint8_t wm_object_move_up( struct WM_STRUCTURE_OBJECT *object ) {
	// find object at list
	for( uint16_t i = 0; i < wm_list_limit; i++ ) {
		// object located?
		if( wm_list_base_address[ i ] != object ) continue;	// no

		// move all objects in place of selected
		for( uint16_t j = i; j < wm_list_limit; j++ ) {
			// next object will be a taskbar?
			if( wm_list_base_address[ j + 1 ] -> descriptor -> flags & STD_WINDOW_FLAG_taskbar ) break;

			// no, move the next object to the current position
			wm_list_base_address[ i++ ] = wm_list_base_address[ j + 1 ];
		}

		// put the object back in its new position
		wm_list_base_address[ i ] = object;

		// object changed position
		return TRUE;
	}

	// object not found
	return FALSE;
}

void wm_object_remove( uint16_t i ) {
	// preserve pointer to object
	struct WM_STRUCTURE_OBJECT *object = wm_list_base_address[ i ];

	// remove object from list
	for( uint64_t j = i; j < wm_list_limit - 1; j++ ) wm_list_base_address[ j ] = wm_list_base_address[ j + 1 ];

	// clear last entry
	wm_list_base_address[ --wm_list_limit ] = EMPTY;

	// select new active object
	wm_object_active_new();

	// release old object area
	std_memory_release( (uintptr_t) object -> descriptor, MACRO_PAGE_ALIGN_UP( object -> size_byte ) >> STD_SHIFT_PAGE );

	// release old entry in object table
	object -> descriptor = EMPTY;

	// update taskbar status
	wm_taskbar_semaphore = TRUE;
}