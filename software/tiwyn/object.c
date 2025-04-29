/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void tiwyn_object( void ) {
	// properties of object list
	struct TIWYN_STRUCTURE_OBJECT **list = tiwyn_list_base_address;

	// search whole list for flush request on any object
	for( uint16_t i = 0; i < tiwyn_list_limit; i++ ) {
		// found cursor object?
		if( list[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_cursor ) continue;	// done

		// requested flush?
		if( list[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_flush ) {
			// parse object area
			tiwyn_zone_insert( (struct TIWYN_STRUCTURE_ZONE *) tiwyn_list_base_address[ i ], FALSE );

			// request parsed
			list[ i ] -> descriptor -> flags &= ~STD_WINDOW_FLAG_flush;

			// always redraw cursor object (it might be covered)
			tiwyn_object_cursor -> descriptor -> flags |= STD_WINDOW_FLAG_flush;
		}
	}
}

struct TIWYN_STRUCTURE_OBJECT *tiwyn_object_create( uint16_t x, uint16_t y, uint16_t width, uint16_t height ) {
	// properties of object array
	struct TIWYN_STRUCTURE_OBJECT *object = tiwyn_object_base_address;

	// find available entry
	for( uint64_t i = 0; i < TIWYN_OBJECT_LIMIT; i++ ) {
		// record in use?
		if( object -> descriptor ) { object++; continue; }	// yes, next

		// fill object properties
		object -> x		= x;
		object -> y		= y;
		object -> width		= width;
		object -> height	= height;

		// calculate object area size in Bytes
		object -> limit = ((width * height) << STD_VIDEO_DEPTH_shift) + sizeof( struct LIB_WINDOW_DESCRIPTOR );

		// assign area for object
		if( ! (object -> descriptor = (struct LIB_WINDOW_DESCRIPTOR *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( object -> limit ) >> STD_SHIFT_PAGE )) )
			// no enough memory
			return EMPTY;

		// register object on list
		tiwyn_object_insert( object );

		// newly created object becomes active
		tiwyn_object_active = object;

		// ready
		return object;
	}

	// cancel
	return EMPTY;
}

struct TIWYN_STRUCTURE_OBJECT *tiwyn_object_find( uint16_t x, uint16_t y, uint8_t hidden ) {
	// properties of object list
	struct TIWYN_STRUCTURE_OBJECT **list = tiwyn_list_base_address;

	// find object at current cursor coordinates
	for( uint16_t i = tiwyn_list_limit - 1; i >= 0; i-- ) {
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
	struct TIWYN_STRUCTURE_OBJECT **list = tiwyn_list_base_address;

	// find panel object on list
	uint64_t i = 0; for( ; i < tiwyn_list_limit; i++ ) {
		// if not a panel
		if( ! (list[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_panel) ) continue;	// next entry

		// move all objects one position further
		for( uint64_t j = tiwyn_list_limit; j > i; j-- ) list[ j ] = list[ j - 1 ];

		// entry prepared
		break;
	}

	// insert object on list
	list[ i ] = object;

	// amount of objects on list
	tiwyn_list_limit++;
}
