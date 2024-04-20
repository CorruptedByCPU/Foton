/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_library( void ) {
	// prepare space for loaded library entries
	kernel -> library_base_address = (struct KERNEL_LIBRARY_STRUCTURE *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( KERNEL_LIBRARY_limit * sizeof( struct KERNEL_LIBRARY_STRUCTURE ) ) >> STD_SHIFT_PAGE );

	// prepare space for memory map of library space with size the same as kernels
	kernel -> library_map_address = (uint32_t *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( (kernel -> page_limit >> STD_SHIFT_8) + 1 ) >> STD_SHIFT_PAGE );

	// initialize memory map
	kernel_memory_dispose( kernel -> library_map_address, EMPTY, kernel -> page_limit );
}