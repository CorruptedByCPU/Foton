/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// run Window Manager
	int64_t wm_pid = std_exec( (uint8_t *) "wm", 2 );

	// debug
	#include "./wm/config.h"
	struct WM_STRUCTURE_REQUEST request = { 320, 200 };
	std_ipc_send( wm_pid, (uint8_t *) &request );

	// hold the door
	while( TRUE );
}
