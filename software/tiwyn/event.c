/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void tiwyn_event( void ) {
	// mouse
	tiwyn_event_mouse();
}

void tiwyn_event_mouse( void ) {
	// retrieve current mouse status and position
	struct STD_STRUCTURE_MOUSE_SYSCALL mouse;
	std_mouse( (struct STD_STRUCTURE_MOUSE_SYSCALL *) &mouse );

	// select object under cursor position
	struct TIWYN_STRUCTURE_OBJECT *object = tiwyn_object_find( mouse.x, mouse.y, FALSE );

	// calculate delta of cursor new position
	int16_t delta_x = mouse.x - tiwyn_object_cursor -> x;
	int16_t delta_y = mouse.y - tiwyn_object_cursor -> y;
	int16_t delta_z = mouse.z - tiwyn_mouse_z; tiwyn_mouse_z = mouse.z;

	//--------------------------------------------------------------

	// update pointer position inside current object
	object -> descriptor -> x = (tiwyn_object_cursor -> x + delta_x) - object -> x;
	object -> descriptor -> y = (tiwyn_object_cursor -> y + delta_y) - object -> y;

	//--------------------------------------------------------------

	// if cursor pointer movement doesn't occur
	if( ! delta_x && ! delta_y ) return;	// done

	// remove current cursor position from workbench
	tiwyn_zone_insert( (struct TIWYN_STRUCTURE_ZONE *) tiwyn_object_cursor, FALSE );

	// update cursor position
	tiwyn_object_cursor -> x = mouse.x;
	tiwyn_object_cursor -> y = mouse.y;

	// redisplay cursor at new location
	tiwyn_object_cursor -> descriptor -> flags |= STD_WINDOW_FLAG_flush;
}
