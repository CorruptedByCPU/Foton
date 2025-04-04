/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_library( void ) {
	// prepare area for registered libraries
	kernel -> library_limit = KERNEL_LIBRARY_limit;
	kernel -> library_base_address = (struct KERNEL_STRUCTURE_LIBRARY *) kernel_memory_alloc( kernel -> library_limit );

	// prepare area for memory map of libraries
	kernel -> library_memory_address = (uint32_t *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( (kernel -> page_limit >> STD_SHIFT_8) + TRUE ) >> STD_SHIFT_PAGE );

	// initialize memory map of libraries
	kernel_memory_dispose( kernel -> library_memory_address, EMPTY, kernel -> page_limit );
}
