/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_object( void ) {
	// properties of object list
	struct WM_STRUCTURE_OBJECT **list = wm_list_base_address;

	// search whole list for flush request on any object
	for( uint16_t i = 0; i < wm_list_limit; i++ ) {
		// found cursor object?
		if( list[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_cursor ) continue;	// done

		// requested flush?
		if( list[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_flush ) {
			// parse object area
		// 	wm_zone_insert( (struct WM_STRUCTURE_ZONE *) wm_list_base_address[ i ], FALSE );

			// request parsed
			list[ i ] -> descriptor -> flags &= ~STD_WINDOW_FLAG_flush;

			// always redraw cursor object (it might be covered)
		// 	if( wm_object_cursor ) wm_object_cursor -> descriptor -> flags |= STD_WINDOW_FLAG_flush;
		}
	}
}

struct WM_STRUCTURE_OBJECT *wm_object_create( uint16_t x, uint16_t y, uint16_t width, uint16_t height ) {
	// properties of object array
	struct WM_STRUCTURE_OBJECT *object = wm_object_base_address;

	// find available entry
	for( uint64_t i = 0; i < WM_OBJECT_LIMIT; i++ ) {
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
		wm_object_insert( object );

		// newly created object becomes active
		wm_object_active = object;

		// ready
		return object;
	}

	// cancel
	return EMPTY;
}

void wm_object_insert( struct WM_STRUCTURE_OBJECT *object ) {
	// properties of object list
	struct WM_STRUCTURE_OBJECT **list = wm_list_base_address;

	// find taskbar object on list
	for( uint64_t i = 0; i < wm_list_limit; i++ ) {
		// if not a taskbar
		if( list[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_taskbar ) { list++; continue; }	// next entry

		// move all objects one position further
		for( uint64_t j = wm_list_limit; j > i; j-- ) list[ j ] = list[ j - 1 ];

		// entry prepared
		break;
	}

	// insert object on list
	*list = object;

	// amount of objects on list
	wm_list_limit++;
}
