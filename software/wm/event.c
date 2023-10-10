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
		struct WM_STRUCTURE_OBJECT *object = wm_object_create( (wm_object_workbench -> width >> 1) - (request -> width >> 1 ), (wm_object_workbench -> height >> 1) - (request -> height >> 1), request -> width, request -> height );

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
}