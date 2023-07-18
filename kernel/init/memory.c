/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_memory() {
	// limine shared with us a memory map?
	if( limine_memmap_request.response == NULL || ! limine_memmap_request.response -> entry_count ) {	// no
		// show an error
		lib_terminal_string( &kernel_terminal, "Houston, we have a problem.\n", 28 );

		// hold the door
		while( TRUE );
	}

	// find largest available space
	uint64_t local_largest_byte = EMPTY;

	// available memory space
	uint64_t local_overall_page = EMPTY;

	// all available memory space should be clean at kernel initialization
	for( uint64_t i = 0; i < limine_memmap_request.response -> entry_count; i++ ) {
		// USABLE memory space?
		if( limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_USABLE ) {
			// sum up available memory space
			local_overall_page += limine_memmap_request.response -> entries[ i ] -> length >> STD_SHIFT_PAGE;
	
			// clean it up
			kernel_page_clean( limine_memmap_request.response -> entries[ i ] -> base, limine_memmap_request.response -> entries[ i ] -> length >> STD_SHIFT_PAGE );

			// inside largest continous memory space we will create kernel environment metadata and a binary memory map next to it

			// this space is larger than previous one?
			if( local_largest_byte < limine_memmap_request.response -> entries[ i ] -> length ) {
				// keep logical address of largest continous memory space (reflection in Higher Half)
				kernel = (struct KERNEL *) (limine_memmap_request.response -> entries[ i ] -> base | KERNEL_PAGE_mirror);

				// keep size information
				local_largest_byte = limine_memmap_request.response -> entries[ i ] -> length;
			}
		}
	}

	// binary memory map base address will be placed after kernel environment variables/functions/rountines
	kernel -> memory_base_address = (uint64_t *) (MACRO_PAGE_ALIGN_UP( (uintptr_t) kernel + sizeof( struct KERNEL ) ));

	// [...]
}