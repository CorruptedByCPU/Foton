/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_object( void ) {
	// properties of object list
	struct WM_STRUCTURE_OBJECT **list = wm -> list;

	// search whole list for flush request on any object
	for( uint16_t i = 0; i < wm -> list_limit; i++ ) {
		// remove active flag from non-compliant objects
		if( list[ i ] != wm -> active ) list[ i ] -> descriptor -> flags &= ~LIB_WINDOW_FLAG_active;

		// requested hide or flush?
		if( list[ i ] -> descriptor -> flags & LIB_WINDOW_FLAG_hide || list[ i ] -> descriptor -> flags & LIB_WINDOW_FLAG_flush ) {
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

			// request parsed
			list[ i ] -> descriptor -> flags &= ~LIB_WINDOW_FLAG_flush;
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

		// window name change?
		if( list[ i ] -> descriptor -> flags & LIB_WINDOW_FLAG_name ) {
			// refresh panel content
			wm -> panel_semaphore = TRUE;

			// remove flag
			list[ i ] -> descriptor -> flags ^= LIB_WINDOW_FLAG_name;
		}
	}
}

struct WM_STRUCTURE_OBJECT *wm_object_create( uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t flags ) {
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

		// set crucial properties
		object -> descriptor -> pixel		= (uint32_t *) ((uintptr_t) object -> descriptor + sizeof( struct LIB_WINDOW_STRUCTURE ));
		object -> descriptor -> current_x	= x;
		object -> descriptor -> current_y	= y;
		object -> descriptor -> current_width	= width;
		object -> descriptor -> current_height	= height;

		// set flags
		object -> descriptor -> flags		= flags;

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
	for( uint64_t i = wm -> list_start; i < wm -> list_limit; i++ )
		// object is visible?
		if( wm -> list[ i ] -> descriptor -> flags & LIB_WINDOW_FLAG_visible ) wm -> active = wm -> list[ i ];
}

struct WM_STRUCTURE_OBJECT *wm_object_find( uint16_t x, uint16_t y, uint8_t hidden ) {
	// properties of object list
	struct WM_STRUCTURE_OBJECT **list = wm -> list;

	// find object at current cursor coordinates
	for( uint64_t i = TRUE; i < wm -> list_limit; i++ ) {
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

	// insert object at
	uint64_t offset = wm -> list_start;
	// except if thats a special object
	if( object -> descriptor -> flags & LIB_WINDOW_FLAG_cursor ) offset = EMPTY;

	// move all objects one position further
	for( uint64_t j = wm -> list_limit; j > offset; j-- ) list[ j ] = list[ j - 1 ];

	// insert object on list
	list[ offset ] = object;

	// special object?
	if( object -> descriptor -> flags & (LIB_WINDOW_FLAG_panel | LIB_WINDOW_FLAG_cursor | LIB_WINDOW_FLAG_menu) ) wm -> list_start++;

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

void wm_object_move_up( struct WM_STRUCTURE_OBJECT *object ) {
	// remove object from list
	for( uint64_t i = wm -> list_start; i < wm -> list_limit; i++ ) if( wm -> list[ i ] == object ) { for( uint64_t j = i; j < wm -> list_limit; j++ ) wm -> list[ j ] = wm -> list[ j + 1 ]; wm -> list[ --wm -> list_limit ] = EMPTY; break; }

	// insert again as beginning
	wm_object_insert( object );
}
