/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_storage() {
	// allocate space for the list of available media
	kernel -> storage_base_address = (struct KERNEL_STORAGE_STRUCTURE *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( KERNEL_STORAGE_limit  * sizeof( struct KERNEL_STORAGE_STRUCTURE ) ) >> STD_SHIFT_PAGE );

	// no root storage by default
	kernel -> storage_root_id = -1;

	// check each attached module
	for( uint64_t i = 0; i < limine_module_request.response -> module_count; i++ ) {
		MACRO_DEBUF();
		// module type of TAR?
		if( lib_tar_check( (uintptr_t) limine_module_request.response -> modules[ i ] -> address ) ) {
			// register device of type MEMORY
			struct KERNEL_STORAGE_STRUCTURE *storage = kernel_storage_register( KERNEL_STORAGE_TYPE_memory );

			// initialize Virtual File System based on content of TAR file

			// set device properties
			storage -> device_blocks = limine_module_request.response -> modules[ i ] -> size;
			// storage -> storage_file = (void *) kernel_storage_file;
			// storage -> storage_read = (void *) kernel_storage_read;
		}
	}
}
