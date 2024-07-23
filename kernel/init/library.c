/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_library( void ) {
	// prepare area for library entries
	kernel -> library_base_address = (struct KERNEL_LIBRARY_STRUCTURE *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( KERNEL_LIBRARY_limit * sizeof( struct KERNEL_LIBRARY_STRUCTURE ) ) >> STD_SHIFT_PAGE );

	// prepare area for memory map of libraries with size same as kernels memory map
	kernel -> library_map_address = (uint32_t *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( (kernel -> page_limit >> STD_SHIFT_8) + TRUE ) >> STD_SHIFT_PAGE );

	// initialize library memory map
	kernel_memory_dispose( kernel -> library_map_address, EMPTY, kernel -> page_limit );
}