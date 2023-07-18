/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_memory() {
	// limine shared with us a memory map?
	if( limine_memmap_request.response == NULL || ! limine_memmap_request.response -> entry_count ) {
		// show error
		lib_terminal_string( &kernel_terminal, "Houston, we have a problem.", 27 );

		// hold the door
		while( TRUE );
	}
}