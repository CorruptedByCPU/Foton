/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// run Window Manager
	// int64_t wm_pid = std_exec( (uint8_t *) "wm", 2 );

	// debug
	int64_t wm_pid = std_exec( (uint8_t *) "3d", 2 );

	// // debug
	// #include "./wm/config.h"
	// uint8_t data[ STD_IPC_SIZE_byte ];
	// struct WM_STRUCTURE_REQUEST *request = (struct WM_STRUCTURE_REQUEST *) &data;
	// request -> width = 320;
	// request -> height = 200;
	// std_ipc_send( wm_pid, (uint8_t *) request );

	// hold the door
	while( TRUE );
}
