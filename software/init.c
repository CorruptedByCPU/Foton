/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// run Window Manager
	int64_t wm_pid = std_exec( (uint8_t *) "wm", 2 );

	// debug
	int64_t gui_pid = std_exec( (uint8_t *) "gui", 3 );

	// hold the door
	while( TRUE );
}
