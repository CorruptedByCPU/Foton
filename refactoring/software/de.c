/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/font.h"
	#include	"../library/image.h"
	#include	"../library/integer.h"
	#include	"../library/window.h"
	//======================================================================

	//----------------------------------------------------------------------
	// definitions, structures
	//----------------------------------------------------------------------
	#include	"./de/config.h"
	//======================================================================

	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"./de/data.c"
	//======================================================================

	//----------------------------------------------------------------------
	// routines, procedures
	//----------------------------------------------------------------------
	#include	"./de/clock.c"
	#include	"./de/init.c"
	//======================================================================

uint64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize desktop environment
	de_init();

	// main loop
	while( TRUE ) {
		// release CPU time
		sleep( 128 );

		// allocate message container
		uint8_t ipc_data[ STD_IPC_SIZE_byte ] = { EMPTY };

		// properties of messages
		struct STD_STRUCTURE_IPC_WINDOW *request = (struct STD_STRUCTURE_IPC_WINDOW *) &ipc_data;
		struct STD_STRUCTURE_IPC_WINDOW_DESCRIPTOR *answer = (struct STD_STRUCTURE_IPC_WINDOW_DESCRIPTOR *) &ipc_data;

		// window properties
		request -> ipc.type = STD_IPC_TYPE_default;
		request -> properties = STD_IPC_WINDOW_list;

		// send request to Window Manager
		std_ipc_send( de_framebuffer.pid, (uint8_t *) request );

		// wait for answer
		uint64_t timeout = std_microtime() + 32768;
		while( (! std_ipc_receive( (uint8_t *) answer ) || answer -> ipc.type != STD_IPC_TYPE_default) && timeout > std_microtime() );

		// descriptor received?
		if( ! answer -> descriptor ) {
			// don't wait anymore, remove any obsolete memory assignments
			std_memory_purge();

			// continue
			continue;
		}

		// properties of window list
		de_taskbar_base_address = (struct LIB_WINDOW_STRUCTURE_LIST *) answer -> descriptor;

		// calculate new amount of entries
		while( de_taskbar_base_address[ de_taskbar_limit ].id ) de_taskbar_limit++;

		// clear taskbar window list
		for( uint16_t y = 0; y < de_window_taskbar -> height; y++ )
			for( uint16_t x = 0; x < (de_window_taskbar -> width - DE_TASKBAR_HEIGHT_pixel - DE_TASKBAR_CLOCK_pixel) ; x++ )
				de_taskbar_list_base_address[ (y * de_window_taskbar -> width) + x ] = DE_TASKBAR_BACKGROUND_default;
	}

	return 0;
}
