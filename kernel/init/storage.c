/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_storage( void ) {
	// allocate area for list of storage devices
	kernel -> storage_base_address = (struct KERNEL_STORAGE_STRUCTURE *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( KERNEL_STORAGE_limit * sizeof( struct KERNEL_STORAGE_STRUCTURE ) ) >> STD_SHIFT_PAGE );

	// check each attached module
	for( uint64_t i = 0; i < limine_module_request.response -> module_count; i++ ) {
		// module type of VFS?
		if( kernel_vfs_identify( (uintptr_t) limine_module_request.response -> modules[ i ] -> address, limine_module_request.response -> modules[ i ] -> size ) ) {
			// properties of registered storage
			struct KERNEL_STORAGE_STRUCTURE *storage = (struct KERNEL_STORAGE_STRUCTURE *) &kernel -> storage_base_address[ kernel_storage_register( KERNEL_STORAGE_TYPE_vfs ) ];

			// default storage properties
			storage -> root = (uint64_t) limine_module_request.response -> modules[ i ] -> address;
			storage -> byte = limine_module_request.response -> modules[ i ] -> size;
		}
	}
}
