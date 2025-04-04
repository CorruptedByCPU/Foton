/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_exec( void ) {
	// kernel file properties
	struct limine_file *file = limine_kernel_file_request.response -> kernel_file;

	// retrieve file name to execute
	// if( lib_string_length( (uint8_t *) file -> cmdline ) ) kernel_exec( (uint8_t *) file -> cmdline, lib_string_length( (uint8_t *) file -> cmdline ), EMPTY, FALSE );
}
