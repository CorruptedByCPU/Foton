/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_cmd( void ) {
	// kernel file properties
	struct limine_file *limine_file = limine_kernel_file_request.response -> kernel_file;

	// retrieve file name to execute
	kernel_exec( (uint8_t *) limine_file -> cmdline, lib_string_length( (uint8_t *) limine_file -> cmdline ), EMPTY );
}