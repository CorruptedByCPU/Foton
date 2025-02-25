/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_storage( void ) {
	// allocate area for list of available storages
	kernel -> storage_base_address = (struct KERNEL_STRUCTURE_STORAGE *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( KERNEL_STORAGE_limit * sizeof( struct KERNEL_STRUCTURE_STORAGE ) ) >> STD_SHIFT_PAGE );

	// register modules of Virtual File System as storages
	for( uint64_t i = 0; i < limine_module_request.response -> module_count; i++ ) {
		// module type of VFS?
		if( ! kernel_vfs_identify( (uintptr_t) limine_module_request.response -> modules[ i ] -> address, limine_module_request.response -> modules[ i ] -> size ) ) continue;	// no

		// register device of type VFS
		struct KERNEL_STRUCTURE_STORAGE *storage = (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ kernel_storage_add() ];

		// storage class
		storage -> device_type = STD_STORAGE_TYPE_memory;

		// file system type: VFS
		storage -> device_fs = KERNEL_STORAGE_FS_vfs;

		// set storage name
		uint8_t string_memory[] = "Memory";
		storage -> device_name_limit = sizeof( string_memory ) - 1;
		for( uint8_t c = 0; c < storage -> device_name_limit; c++ ) storage -> device_name[ c ] = string_memory[ c ]; storage -> device_name[ storage -> device_name_limit ] = STD_ASCII_TERMINATOR;

		// address of VFS main block location
		storage -> device_block = (uintptr_t) limine_module_request.response -> modules[ i ] -> address;

		// default block size in Bytes
		storage -> device_byte = STD_PAGE_byte;

		// length of storage in Blocks
		storage -> device_limit = MACRO_PAGE_ALIGN_UP( limine_module_request.response -> modules[ i ] -> size ) >> STD_SHIFT_PAGE;

		// attach read/write functions
		storage -> read = (void *) kernel_vfs_read;
		storage -> write = (void *) kernel_vfs_write;
	
		// storage active
		storage -> flags |= KERNEL_STORAGE_FLAGS_active;
	}

	// start Storage thread
	uint8_t storage_string_thread_name[] = "storage";
	kernel_module_thread( (uintptr_t) &kernel_storage, (uint8_t *) &storage_string_thread_name, sizeof( storage_string_thread_name ) );
}
