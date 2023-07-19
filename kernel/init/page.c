/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_page( void ) {
	// alloc 1 page for PML4 kernel environment array
	kernel -> page_base_address = (uint64_t *) kernel_memory_alloc( 1 );
}