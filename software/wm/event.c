/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_event( void ) {
	// incomming request
	uint8_t data[ STD_IPC_SIZE_byte ]; int64_t source = EMPTY;
	while( (source = std_ipc_receive( (uint8_t *) &data )) ) {
		// if request is invalid
		struct STD_WINDOW_STRUCTURE_REQUEST *request = (struct STD_WINDOW_STRUCTURE_REQUEST *) &data;
		if( ! request -> width || ! request -> height ) continue;	// nothing to do

		// prepare answer structure
		struct STD_WINDOW_STRUCTURE_ANSWER *answer = (struct STD_WINDOW_STRUCTURE_ANSWER *) &data;

		// if source or we are NOT framebuffer owner
		if( framebuffer.pid != wm_pid && framebuffer.pid != source ) {
			// reject window creation
			answer -> descriptor = EMPTY;

			// send answer
			std_ipc_send( source, (uint8_t *) answer );

			// next request
			continue;
		}

		// create new object for process
		struct WM_STRUCTURE_OBJECT *object = wm_object_create( request -> x, request -> y, request -> width, request -> height );

		// share new object descriptor with process
		uintptr_t descriptor = EMPTY;
		if( ! (descriptor = std_memory_share( source, (uintptr_t) object -> descriptor, MACRO_PAGE_ALIGN_UP( object -> size_byte ) >> STD_SHIFT_PAGE )) ) continue;	// no enough memory?

		// send answer
		answer -> descriptor = descriptor;
		answer -> size_byte = object -> size_byte;
		std_ipc_send( source, (uint8_t *) answer );

		// it was first window request?
		if( wm_pid == framebuffer.pid ) {
			// change framebuffer owner
			framebuffer.pid = source;

			// change properties of kernels framebuffer
			std_framebuffer_change( &framebuffer );
		}
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