/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void console_init( void ) {
	// initialize window interface
	interface.properties = (uint8_t *) &file_interface_start;
	lib_interface( &interface );
}