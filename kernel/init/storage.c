/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_storage( void ) {
	// allocate area for list of available storages
	kernel -> storage_limit = KERNEL_STORAGE_limit;
	kernel -> storage_base_address = (struct KERNEL_STRUCTURE_STORAGE *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( kernel -> storage_limit * sizeof( struct KERNEL_STRUCTURE_STORAGE ) ) >> STD_SHIFT_PAGE );
}
