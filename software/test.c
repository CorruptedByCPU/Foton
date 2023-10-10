
/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions of WM (window manager)
	//----------------------------------------------------------------------
	#include	"./wm/config.h"

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// obtain information about kernel framebuffer
	struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER framebuffer;
	std_framebuffer( &framebuffer );

	// allocate container data
	uint8_t data[ STD_IPC_SIZE_byte ];

	// prepeare new window request
	struct WM_STRUCTURE_REQUEST *request = (struct WM_STRUCTURE_REQUEST *) &data;
	request -> width = 320;
	request -> height = 200;

	// send request
	std_ipc_send( framebuffer.owner_pid, (uint8_t *) request );

	// wait for answer
	while( ! std_ipc_receive( (uint8_t *) &data ) );

	// window created?
	struct WM_STRUCTURE_ANSWER *answer = (struct WM_STRUCTURE_ANSWER *) &data;
	if( answer -> descriptor ) {	// yes
		// properties of window
		struct WM_STRUCTURE_DESCRIPTOR *descriptor = (struct WM_STRUCTURE_DESCRIPTOR *) answer -> descriptor;

		// fill window with default gradient
		uint32_t *pixel = (uint32_t *) ((uintptr_t) descriptor + sizeof( struct WM_STRUCTURE_DESCRIPTOR ));
		for( uint64_t y = 0; y < 200; y++ )
			for( uint64_t x = 0; x < 320; x++ )
				pixel[ (y * 320) + x ] = STD_COLOR_GREEN_light;

		// window content ready for display
		descriptor -> flags |= WM_OBJECT_FLAG_visible | WM_OBJECT_FLAG_flush;
	}

	// hold the door
	while( TRUE );
}