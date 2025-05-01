/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void tiwyn_object( void ) {
	// properties of object list
	struct TIWYN_STRUCTURE_OBJECT **list = tiwyn -> list;

	// search whole list for flush request on any object
	for( uint16_t i = 0; i < tiwyn -> list_limit; i++ ) {
		// found cursor object?
		if( list[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_cursor ) continue;	// done

		// active window selected?
		if( list[ i ] == tiwyn -> active ) tiwyn -> active -> descriptor -> flags |= STD_WINDOW_FLAG_active;
		else list[ i ] -> descriptor -> flags &= ~STD_WINDOW_FLAG_active;

		// requested flush?
		if( list[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_flush ) {
			// parse object area
			tiwyn_zone_insert( (struct TIWYN_STRUCTURE_ZONE *) tiwyn -> list[ i ], FALSE );

			// request parsed
			list[ i ] -> descriptor -> flags &= ~STD_WINDOW_FLAG_flush;

			// always redraw cursor object (it might be covered)
			tiwyn -> cursor -> descriptor -> flags |= STD_WINDOW_FLAG_flush;
		}
	}
}

struct TIWYN_STRUCTURE_OBJECT *tiwyn_object_create( uint16_t x, uint16_t y, uint16_t width, uint16_t height ) {
	// properties of object array
	struct TIWYN_STRUCTURE_OBJECT *object = tiwyn -> object;

	// find available entry
	for( uint64_t i = 0; i < TIWYN_OBJECT_LIMIT; i++ ) {
		// record in use?
		if( object -> descriptor ) { object++; continue; }	// yes, next

		// fill object properties
		object -> x		= x;
		object -> y		= y;
		object -> z		= 0;	// no movement
		object -> width		= width;
		object -> height	= height;

		// calculate object area size in Bytes
		object -> limit = ((width * height) << STD_VIDEO_DEPTH_shift) + sizeof( struct LIB_WINDOW_DESCRIPTOR );

		// by default all object belong to Tiwyn
		object -> pid = tiwyn -> pid;

		// assign area for object
		if( ! (object -> descriptor = (struct LIB_WINDOW_DESCRIPTOR *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( object -> limit ) >> STD_SHIFT_PAGE )) )
			// no enough memory
			return EMPTY;

		// register object on list
		tiwyn_object_insert( object );

		// newly created object becomes active
		tiwyn -> active = object;

		// ready
		return object;
	}

	// cancel
	return EMPTY;
}

void tiwyn_object_current( void ) {
	// search thru object list as far as to panel type object
	for( uint16_t i = 0; i < tiwyn -> list_limit; i++ ) {
		// panel or cursor object?
		if( tiwyn -> list[ i ] -> descriptor -> flags & (STD_WINDOW_FLAG_panel | STD_WINDOW_FLAG_cursor) ) break;	// yes

		// object is visible?
		if( tiwyn -> list[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_visible ) tiwyn -> active = tiwyn -> list[ i ];
	}
}

struct TIWYN_STRUCTURE_OBJECT *tiwyn_object_find( uint16_t x, uint16_t y, uint8_t hidden ) {
	// properties of object list
	struct TIWYN_STRUCTURE_OBJECT **list = tiwyn -> list;

	// find object at current cursor coordinates
	for( uint16_t i = tiwyn -> list_limit - 1; i >= 0; i-- ) {
		// object marked as cursor?
		if( list[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_cursor ) continue;	// ignore

		// object is visible? (or include hidden ones too)
		if( list[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_visible || hidden ) {
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

void tiwyn_object_insert( struct TIWYN_STRUCTURE_OBJECT *object ) {
	// properties of object list
	struct TIWYN_STRUCTURE_OBJECT **list = tiwyn -> list;

	// find panel object on list
	uint64_t i = 0; for( ; i < tiwyn -> list_limit; i++ ) {
		// if not a panel
		if( ! (list[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_panel) ) continue;	// next entry

		// move all objects one position further
		for( uint64_t j = tiwyn -> list_limit; j > i; j-- ) list[ j ] = list[ j - 1 ];

		// entry prepared
		break;
	}

	// insert object on list
	list[ i ] = object;

	// amount of objects on list
	tiwyn -> list_limit++;
}

void tiwyn_object_move( int16_t x, int16_t y ) {
	// properties of zone for truncate operation
	struct TIWYN_STRUCTURE_ZONE zone;

	// initial values
	zone.x		= tiwyn -> selected -> x;
	zone.y		= tiwyn -> selected -> y;
	zone.width	= tiwyn -> selected -> width;
	zone.height	= tiwyn -> selected -> height;

	// a movement on X axis occurred?
	if( x ) {
		// new position of selected object
		tiwyn -> selected -> x += x;

		// X axis shift is positive?
		if( x > 0 )	// yes
			// width of exposed zone
			zone.width = x;
		else {
			// position and width of exposed zone
			zone.x = tiwyn -> selected -> x + tiwyn -> selected -> width;
			zone.width = ~x + 1;
		}

		// register zone
		tiwyn_zone_insert( (struct TIWYN_STRUCTURE_ZONE *) &zone, FALSE );

		// update zone properties
		zone.x = tiwyn -> selected -> x;
		zone.width = tiwyn -> selected -> width;
	}

	// a movement on Y axis occured?
	if( y ) {
		// new position of selected object
		tiwyn -> selected -> y += y;

		// Y axis shift is positive?
		if( y > 0 )	// yes
			// height of exposed fragment
			zone.height = y;
		else {
			// position and height of exposed fragment
			zone.y = tiwyn -> selected -> y + tiwyn -> selected -> height;
			zone.height = ~y + 1;
		}

		// register zone
		tiwyn_zone_insert( (struct TIWYN_STRUCTURE_ZONE *) &zone, FALSE );
	}

	// object has been moved
	tiwyn -> selected -> descriptor -> flags |= STD_WINDOW_FLAG_flush;
}

uint8_t wm_object_move_up( struct TIWYN_STRUCTURE_OBJECT *object ) {
	// find object on list
	for( uint64_t i = 0; i < tiwyn -> list_limit; i++ ) {
		// object located?
		if( tiwyn -> list[ i ] != object ) continue;	// no

		// move all objects in place of selected
		for( uint64_t j = i; j < tiwyn -> list_limit - TRUE; j++ ) {
			// next object, panel?
			if( tiwyn -> list[ j + 1 ] -> descriptor -> flags & STD_WINDOW_FLAG_panel ) break;	// done

			// no, move next object to current position
			tiwyn -> list[ i++ ] = tiwyn -> list[ j + 1 ];
		}

		// put object back on its new position
		tiwyn -> list[ i ] = object;

		// object changed position
		return TRUE;
	}

	// object not found
	return FALSE;
}
