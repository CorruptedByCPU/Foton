/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t init( void ) {
	// obtain information about kernel framebuffer
	std_framebuffer( &framebuffer );

	// allocate data container
	uint8_t data[ STD_IPC_SIZE_byte ];

	// prepeare new window request
	struct WM_STRUCTURE_REQUEST *request = (struct WM_STRUCTURE_REQUEST *) &data;
	request -> width = D3_WIDTH_pixel;
	request -> height = D3_HEIGHT_pixel;

	// send request
	std_ipc_send( framebuffer.owner_pid, (uint8_t *) request );

	// wait for answer
	while( ! std_ipc_receive( (uint8_t *) &data ) );

	// we are not allowed to create new windows?
	struct WM_STRUCTURE_ANSWER *answer = (struct WM_STRUCTURE_ANSWER *) &data;
	if( ! answer -> descriptor ) std_exit();	// nothing to do

	// descriptor properties
	descriptor = (struct WM_STRUCTURE_DESCRIPTOR *) answer -> descriptor;

	// initialize RGL library
	rgl = lib_rgl( descriptor -> width, descriptor -> height - LIB_FONT_HEIGHT_pixel, (uint32_t *) ((uintptr_t) descriptor + sizeof( struct WM_STRUCTURE_DESCRIPTOR )) + (descriptor -> width * LIB_FONT_HEIGHT_pixel) );

	// window content ready for display
	descriptor -> flags |= WM_OBJECT_FLAG_visible;

	//----------------------------------------------------------------------

	object_load();

	// execute interface as separate thread
	std_thread( (uintptr_t) &interface, (uint8_t *) "demo-interface", 14 );

	// it's ok
	return FALSE;
}