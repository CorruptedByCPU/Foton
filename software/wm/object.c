/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_object( void ) {
	// block access to object list
	MACRO_LOCK( wm_list_semaphore );

	// search whole list for object flush
	for( uint16_t i = 0; i < wm_list_limit; i++ ) {
		// ignore cursor object
		if( wm_list_base_address[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_cursor ) continue;

		// object minimized or visible and requested flush?
		if( wm_list_base_address[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_minimize || wm_list_base_address[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_flush ) {
			// parse whole object area
			wm_zone_insert( (struct WM_STRUCTURE_ZONE *) wm_list_base_address[ i ], FALSE );

			// request parsed
			wm_list_base_address[ i ] -> descriptor -> flags &= ~STD_WINDOW_FLAG_flush;

			// remove minimize flag (if set)
			if( wm_list_base_address[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_minimize ) {
				// hide object
				wm_list_base_address[ i ] -> descriptor -> flags &= ~STD_WINDOW_FLAG_visible;

				// remove minimize flag
				wm_list_base_address[ i ] -> descriptor -> flags  &= ~STD_WINDOW_FLAG_minimize;

				// find new active object
				wm_object_active_new();

				// update taskbar list
				wm_taskbar_modified = TRUE;
			}

			// redraw cursor too
			wm_object_cursor -> descriptor -> flags |= STD_WINDOW_FLAG_flush;
		}

		// object renamed?
		if( wm_list_base_address[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_name ) {
			// done
			wm_list_base_address[ i ] -> descriptor -> flags &= ~STD_WINDOW_FLAG_name;

			// update taskbar list
			wm_taskbar_modified = TRUE;
		}
	}

	// release access to object list
	MACRO_UNLOCK( wm_list_semaphore );
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
	// block access to object list
	MACRO_LOCK( wm_list_semaphore );

	// entry id
	uint64_t i = 0;	// first one

	// find taskbar object on list
	for( ; i < wm_list_limit; i++ ) {
		// taskbar object?
		if( wm_list_base_address[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_taskbar ) {
			// move all objects one position forward
			for( uint64_t j = wm_list_limit; j > i; j-- ) wm_list_base_address[ j ] = wm_list_base_address[ j - 1 ];

			// entry prepared
			break;
		}
	}

	// insert object on list
	wm_list_base_address[ i ] = object;

	// amount of objects on list
	wm_list_limit++;

	// release access to object list
	MACRO_UNLOCK( wm_list_semaphore );
}

struct WM_STRUCTURE_OBJECT *wm_object_create( int16_t x, int16_t y, uint16_t width, uint16_t height ) {
	// block access to object array
	MACRO_LOCK( wm_object_semaphore );

	// find available entry
	for( uint64_t i = 0; i < WM_OBJECT_LIMIT; i++ ) {
		// entry in use?
		if( wm_object_base_address[ i ].descriptor ) continue;	// yes

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

		// release acces to object array
		MACRO_UNLOCK( wm_object_semaphore );

		// ready
		return (struct WM_STRUCTURE_OBJECT *) &wm_object_base_address[ i ];
	}

	// release acces to object array
	MACRO_UNLOCK( wm_object_semaphore );

	// no available entry
	return EMPTY;
}

struct WM_STRUCTURE_OBJECT *wm_object_find( uint16_t x, uint16_t y, uint8_t parse_hidden ) {
	// block access to object list
	MACRO_LOCK( wm_list_semaphore );

	// find object at current cursor coordinates
	for( uint16_t i = wm_list_limit - 1; i >= 0; i-- ) {
		// object marked as cursor?
		if( wm_list_base_address[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_cursor ) continue;	// leave it

		// object is visible? (or include hidden ones too)
		if( wm_list_base_address[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_visible || parse_hidden ) {
			// coordinates at object area?
			if( wm_list_base_address[ i ] -> x > x ) continue;	// no
			if( wm_list_base_address[ i ] -> y > y ) continue;	// no
			if( (wm_list_base_address[ i ] -> x + wm_list_base_address[ i ] -> width) < x ) continue;	// no
			if( (wm_list_base_address[ i ] -> y + wm_list_base_address[ i ] -> height) < y ) continue;	// no

			// preserve object properties
			struct WM_STRUCTURE_OBJECT *object = wm_list_base_address[ i ];

			// release access to object list
			MACRO_UNLOCK( wm_list_semaphore );

			// return a pointer to an object
			return object;
		}
	}

	// release access to object list
	MACRO_UNLOCK( wm_list_semaphore );

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
	// block access to object list
	MACRO_LOCK( wm_list_semaphore );

	// find object on list
	for( uint16_t i = 0; i < wm_list_limit; i++ ) {
		// object located?
		if( wm_list_base_address[ i ] != object ) continue;	// no

		// move all objects in place of selected
		for( uint16_t j = i; j < wm_list_limit - 1; j++ ) {
			// next object will be a taskbar?
			if( wm_list_base_address[ j + 1 ] -> descriptor -> flags & STD_WINDOW_FLAG_taskbar ) break;

			// no, move next object to current position
			wm_list_base_address[ i++ ] = wm_list_base_address[ j + 1 ];
		}

		// put object back on its new position
		wm_list_base_address[ i ] = object;

		// release access to object list
		MACRO_UNLOCK( wm_list_semaphore );

		// object changed position
		return TRUE;
	}

	// release access to object list
	MACRO_UNLOCK( wm_list_semaphore );

	// object not found
	return FALSE;
}