/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_storage( void ) {
	// allocate space for the list of available storages
	kernel -> NEW_storage_base_address = (struct NEW_KERNEL_STORAGE_STRUCTURE *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( NEW_KERNEL_STORAGE_limit  * sizeof( struct NEW_KERNEL_STORAGE_STRUCTURE ) ) >> STD_SHIFT_PAGE );

	// register modules of Virtual File System as storages
	for( uint64_t i = 0; i < limine_module_request.response -> module_count; i++ ) {
		// module type of VFS?
		if( ! NEW_kernel_vfs_identify( (uintptr_t) limine_module_request.response -> modules[ i ] -> address, limine_module_request.response -> modules[ i ] -> size ) ) continue;	// no

		// register device of type VFS
		struct NEW_KERNEL_STORAGE_STRUCTURE *storage = NEW_kernel_storage_register( NEW_KERNEL_STORAGE_TYPE_vfs );

		// address of VFS main block location
		storage -> device_block = (uintptr_t) limine_module_request.response -> modules[ i ] -> address;

		// default block size in Bytes
		storage -> device_byte = STD_PAGE_byte;

		// length of storage
		storage -> device_length = MACRO_PAGE_ALIGN_UP( limine_module_request.response -> modules[ i ] -> size ) >> STD_SHIFT_PAGE;
	}
}
