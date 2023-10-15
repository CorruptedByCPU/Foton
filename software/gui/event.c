/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void gui_event( void ) {
	// incomming request
	uint8_t data[ STD_IPC_SIZE_byte ]; int64_t source = EMPTY;
	while( (source = std_ipc_receive( (uint8_t *) &data )) ) {
		// if request is invalid
		struct STD_WINDOW_STRUCTURE_REQUEST *request = (struct STD_WINDOW_STRUCTURE_REQUEST *) &data;
		if( ! request -> width || ! request -> height ) continue;	// nothing to do

		// transfer it to Window Manager
		std_ipc_send( wm_pid, (uint8_t *) &data );

		// wait for answer from Window Manager
		while( ! std_ipc_receive_by_pid( (uint8_t *) &data, wm_pid ) );

		// properties of answer
		struct STD_WINDOW_STRUCTURE_ANSWER *answer = (struct STD_WINDOW_STRUCTURE_ANSWER *) &data;

		// share new object descriptor with process
		if( ! (answer -> descriptor = std_memory_share( source, answer -> descriptor, MACRO_PAGE_ALIGN_UP( answer -> size_byte ) >> STD_SHIFT_PAGE )) ) continue;	// no enough memory?

		// send answer
		std_ipc_send( source, (uint8_t *) answer );

	}
}