/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_event( void ) {
	// mouse
	wm_event_mouse();
}

void wm_event_mouse( void ) {
	// retrieve current mouse status and position
	struct STD_STRUCTURE_MOUSE_SYSCALL mouse;
	std_mouse( (struct STD_STRUCTURE_MOUSE_SYSCALL *) &mouse );

	// select object under cursor position
	struct WM_STRUCTURE_OBJECT *object = wm_object_find( mouse.x, mouse.y, FALSE );

	// calculate delta of cursor new position
	int16_t delta_x = mouse.x - wm_object_cursor -> x;
	int16_t delta_y = mouse.y - wm_object_cursor -> y;
	int16_t delta_z = mouse.z - wm_mouse_z; wm_mouse_z = mouse.z;

	//--------------------------------------------------------------

	// update pointer position inside current object
	object -> descriptor -> x = (wm_object_cursor -> x + delta_x) - object -> x;
	object -> descriptor -> y = (wm_object_cursor -> y + delta_y) - object -> y;

	//--------------------------------------------------------------

	// if cursor pointer movement doesn't occur
	if( ! delta_x && ! delta_y ) return;	// done

	// remove current cursor position from workbench
	wm_zone_insert( (struct WM_STRUCTURE_ZONE *) wm_object_cursor, FALSE );

	// update cursor position
	wm_object_cursor -> x = mouse.x;
	wm_object_cursor -> y = mouse.y;

	// redisplay cursor at new location
	wm_object_cursor -> descriptor -> flags |= STD_WINDOW_FLAG_flush;
}
