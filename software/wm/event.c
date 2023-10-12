/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_event( void ) {
	// incomming request
	uint8_t data[ STD_IPC_SIZE_byte ]; int64_t source = EMPTY;
	while( (source = std_ipc_receive( (uint8_t *) &data )) ) {
		// if there is no request, or request is invalid
		struct WM_STRUCTURE_REQUEST *request = (struct WM_STRUCTURE_REQUEST *) &data;
		if( ! source || ! request -> width || ! request -> height ) continue;	// nothing to do

		// create new object for process
		struct WM_STRUCTURE_OBJECT *object = wm_object_create( request -> x, request -> y, request -> width, request -> height );

		// share new object descriptor with process
		uintptr_t descriptor = EMPTY;
		if( ! (descriptor = std_memory_share( source, (uintptr_t) object -> descriptor, MACRO_PAGE_ALIGN_UP( object -> size_byte ) >> STD_SHIFT_PAGE )) ) continue;	// no enough memory?

		// send answer
		struct WM_STRUCTURE_ANSWER *answer = (struct WM_STRUCTURE_ANSWER *) &data;
		answer -> descriptor = descriptor;
		std_ipc_send( source, (uint8_t *) answer );
	}

	// retrieve current mouse status and position
	struct STD_SYSCALL_STRUCTURE_MOUSE mouse;
	std_mouse( (struct STD_SYSCALL_STRUCTURE_MOUSE *) &mouse );

	// calculate delta of cursor new position
	int16_t delta_x = mouse.x - wm_object_cursor -> x;
	int16_t delta_y = mouse.y - wm_object_cursor -> y;

	__asm__ volatile( "nop" );

	//--------------------------------------------------------------------------
	// if cursor pointer movement occurs
	if( wm_object_cursor && (delta_x || delta_y) ) {
		// remove current cursor position from workbench
		wm_zone_insert( (struct WM_STRUCTURE_ZONE *) wm_object_cursor, FALSE );

		// // update cursor position
		wm_object_cursor -> x += delta_x;
		wm_object_cursor -> y += delta_y;

		// // redisplay the cursor at the new location
		wm_object_cursor -> descriptor -> flags |= WM_OBJECT_FLAG_flush;
	}
}