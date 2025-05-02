/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void tiwyn_release( void ) {
	// check every object on list
	for( uint64_t i = 0; i < tiwyn -> list_limit; i++ ) {
		// object loose connection with parent? or set for release?
		if( std_pid_exist( tiwyn -> list[ i ] -> pid ) && ! (tiwyn -> list[ i ] -> descriptor -> flags & LIB_WINDOW_FLAG_release) ) continue;	// no

		// redraw area of object
		tiwyn_zone_insert( (struct TIWYN_STRUCTURE_ZONE *) tiwyn -> list[ i ], FALSE );

		// release object area
		std_memory_release( (uintptr_t) tiwyn -> list[ i ] -> descriptor, MACRO_PAGE_ALIGN_UP( tiwyn -> list[ i ] -> limit ) >> STD_SHIFT_PAGE );

		// release old entry in object table
		tiwyn -> list[ i ] -> descriptor = EMPTY;

		// remove object from object list
		for( uint16_t j = i; j < tiwyn -> list_limit; j++ ) tiwyn -> list[ j ] = tiwyn -> list[ j + 1 ]; tiwyn -> list[ --tiwyn -> list_limit ] = EMPTY;

		// redisplay cursor
		tiwyn -> cursor -> descriptor -> flags |= LIB_WINDOW_FLAG_flush;

		//--------------------------------------------------------------

		// select new active object
		tiwyn_object_current();
	}
}
