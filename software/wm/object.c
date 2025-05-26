/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_object( void ) {
	// properties of object list
	struct WM_STRUCTURE_OBJECT **list = wm -> list;

	// search whole list for flush request on any object
	for( uint16_t i = 0; i < wm -> list_limit; i++ ) {
		// found cursor object?
		if( list[ i ] -> descriptor -> flags & LIB_WINDOW_FLAG_cursor ) continue;	// done

		// remove active flag from non-compliant objects
		if( list[ i ] != wm -> active ) list[ i ] -> descriptor -> flags &= ~LIB_WINDOW_FLAG_active;

		// requested hide or flush?
		if( list[ i ] -> descriptor -> flags & LIB_WINDOW_FLAG_hide || list[ i ] -> descriptor -> flags & LIB_WINDOW_FLAG_flush ) {
			// request parsed
			list[ i ] -> descriptor -> flags &= ~LIB_WINDOW_FLAG_flush;

			// parse object area
			wm_zone_insert( (struct WM_STRUCTURE_ZONE *) list[ i ], FALSE );

			// hide object?
			if( list[ i ] -> descriptor -> flags & LIB_WINDOW_FLAG_hide ) {
				// accept and request parsed
				list[ i ] -> descriptor -> flags &= ~(LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_hide);

				// selet new active object
				wm_object_activate();

				// refresh panel content
				wm -> panel_semaphore = TRUE;
			}

			// always redraw cursor object (it might be covered)
			wm -> cursor -> descriptor -> flags |= LIB_WINDOW_FLAG_flush;
		}

		// requested maximize?
		if( list[ i ] -> descriptor -> flags & LIB_WINDOW_FLAG_enlarge ) {
			// propose new properties of object
			list[ i ] -> descriptor -> new_x	= 0;
			list[ i ] -> descriptor -> new_y	= 0;
			list[ i ] -> descriptor -> new_width	= wm -> workbench -> width;
			list[ i ] -> descriptor -> new_height	= wm -> workbench -> height - wm -> panel -> height;

			// inform application interface about requested properties
			list[ i ] -> descriptor -> flags |= LIB_WINDOW_FLAG_properties;

			// request parsed
			list[ i ] -> descriptor -> flags &= ~LIB_WINDOW_FLAG_enlarge;
		}
	}
}

struct WM_STRUCTURE_OBJECT *wm_object_create( uint16_t x, uint16_t y, uint16_t width, uint16_t height ) {
	// properties of object array
	struct WM_STRUCTURE_OBJECT *object = wm -> object;

	// find available entry
	for( uint64_t i = 0; i < WM_OBJECT_LIMIT; i++ ) {
		// record in use?
		if( object -> descriptor ) { object++; continue; }	// yes, next

		// fill object properties
		object -> x		= x;
		object -> y		= y;
		object -> z		= 0;	// no movement
		object -> width		= width;
		object -> height	= height;

		// calculate object area size in Bytes
		object -> limit = ((width * height) << STD_VIDEO_DEPTH_shift) + sizeof( struct LIB_WINDOW_STRUCTURE );

		// by default all object belong to Window Manager
		object -> pid = wm -> pid;

		// assign area for object
		if( ! (object -> descriptor = (struct LIB_WINDOW_STRUCTURE *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( object -> limit ) >> STD_SHIFT_PAGE )) )
			// no enough memory
			return EMPTY;

		// register object on list
		wm_object_insert( object );

		// update panel content
		wm -> panel_semaphore = TRUE;

		// ready
		return object;
	}

	// cancel
	return EMPTY;
}

void wm_object_activate( void ) {
	// search thru object list as far as to panel type object
	for( uint16_t i = 0; i < wm -> list_limit; i++ ) {
		// panel or cursor object?
		if( wm -> list[ i ] -> descriptor -> flags & LIB_WINDOW_FLAG_panel ) break;	// yes

		// object is visible?
		if( wm -> list[ i ] -> descriptor -> flags & LIB_WINDOW_FLAG_visible ) wm -> active = wm -> list[ i ];
	}
}

struct WM_STRUCTURE_OBJECT *wm_object_find( uint16_t x, uint16_t y, uint8_t hidden ) {
	// properties of object list
	struct WM_STRUCTURE_OBJECT **list = wm -> list;

	// find object at current cursor coordinates
	for( uint16_t i = wm -> list_limit - 1; i >= 0; i-- ) {
		// object marked as cursor?
		if( list[ i ] -> descriptor -> flags & LIB_WINDOW_FLAG_cursor ) continue;	// ignore

		// object is visible? (or include hidden ones too)
		if( list[ i ] -> descriptor -> flags & LIB_WINDOW_FLAG_visible || hidden ) {
			// coordinates at object area?
			if( list[ i ] -> x > x ) continue;	// no
			if( list[ i ] -> y > y ) continue;	// no
			if( (list[ i ] -> x + list[ i ] -> width) < x ) continue;	// no
			if( (list[ i ] -> y + list[ i ] -> height) < y ) continue;	// no

			// return a pointer to an object
			return list[ i ];;
		}
	}

	// nothing under specified coordinates
	return EMPTY;
}

void wm_object_insert( struct WM_STRUCTURE_OBJECT *object ) {
	// properties of object list
	struct WM_STRUCTURE_OBJECT **list = wm -> list;

	// find panel object on list
	uint64_t i = 0; for( ; i < wm -> list_limit; i++ ) {
		// if not a panel
		if( ! (list[ i ] -> descriptor -> flags & LIB_WINDOW_FLAG_panel) ) continue;	// next entry

		// move all objects one position further
		for( uint64_t j = wm -> list_limit; j > i; j-- ) list[ j ] = list[ j - 1 ];

		// entry prepared
		break;
	}

	// insert object on list
	list[ i ] = object;

	// amount of objects on list
	wm -> list_limit++;
}

void wm_object_move( int16_t x, int16_t y ) {
	// properties of zone for truncate operation
	struct WM_STRUCTURE_ZONE zone;

	// initial values
	zone.x		= wm -> selected -> x;
	zone.y		= wm -> selected -> y;
	zone.width	= wm -> selected -> width;
	zone.height	= wm -> selected -> height;

	// a movement on X axis occurred?
	if( x ) {
		// new position of selected object
		wm -> selected -> x += x;

		// X axis shift is positive?
		if( x > 0 )	// yes
			// width of exposed zone
			zone.width = x;
		else {
			// position and width of exposed zone
			zone.x = wm -> selected -> x + wm -> selected -> width;
			zone.width = ~x + 1;
		}

		// register zone
		wm_zone_insert( (struct WM_STRUCTURE_ZONE *) &zone, FALSE );

		// update zone properties
		zone.x = wm -> selected -> x;
		zone.width = wm -> selected -> width;
	}

	// a movement on Y axis occured?
	if( y ) {
		// new position of selected object
		wm -> selected -> y += y;

		// Y axis shift is positive?
		if( y > 0 )	// yes
			// height of exposed fragment
			zone.height = y;
		else {
			// position and height of exposed fragment
			zone.y = wm -> selected -> y + wm -> selected -> height;
			zone.height = ~y + 1;
		}

		// register zone
		wm_zone_insert( (struct WM_STRUCTURE_ZONE *) &zone, FALSE );
	}

	// object has been moved
	wm -> selected -> descriptor -> flags |= LIB_WINDOW_FLAG_flush;
}

uint8_t wm_object_move_up( struct WM_STRUCTURE_OBJECT *object ) {
	// find object on list
	for( uint64_t i = 0; i < wm -> list_limit; i++ ) {
		// object located?
		if( wm -> list[ i ] != object ) continue;	// no

		// move all objects in place of selected
		for( uint64_t j = i; j < wm -> list_limit - TRUE; j++ ) {
			// next object, panel?
			if( wm -> list[ j + 1 ] -> descriptor -> flags & LIB_WINDOW_FLAG_panel ) break;	// done

			// no, move next object to current position
			wm -> list[ i++ ] = wm -> list[ j + 1 ];
		}

		// put object back on its new position
		wm -> list[ i ] = object;

		// object changed position
		return TRUE;
	}

	// object not found
	return FALSE;
}
