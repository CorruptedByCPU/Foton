/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_storage( void ) {
	// allocate area for list of available storages
	kernel -> storage_base_address = (struct KERNEL_STRUCTURE_STORAGE *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( KERNEL_STORAGE_limit * sizeof( struct KERNEL_STRUCTURE_STORAGE ) ) >> STD_SHIFT_PAGE );

	// register all modules as memory storage
	for( uint64_t i = 0; i < limine_module_request.response -> module_count; i++ ) {
		// register device
		struct KERNEL_STRUCTURE_STORAGE *storage = (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ kernel_storage_add() ];

		// as memory type
		storage -> device_type = STD_STORAGE_TYPE_memory;

		// with unknown file system
		storage -> device_fs = KERNEL_STORAGE_FS_undefinied;

		// first block of device
		storage -> device_block = (uintptr_t) limine_module_request.response -> modules[ i ] -> address;

		// device limit (Bytes)
		storage -> device_limit = limine_module_request.response -> modules[ i ] -> size;
	}

	//----------------------------------------------------------------------

	// register device
	struct KERNEL_STRUCTURE_STORAGE *storage = (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ kernel_storage_add() ];

	// as memory type
	storage -> device_type = STD_STORAGE_TYPE_memory;

	// and with file system of VFS
	storage -> device_fs = KERNEL_STORAGE_FS_vfs;

	// create default directory structure
	kernel_vfs_format( storage );

	// set storage name
	uint8_t local_string_home[] = "home";
	storage -> device_name_limit = sizeof( local_string_home ) - 1;
	for( uint8_t c = 0; c < storage -> device_name_limit; c++ ) storage -> device_name[ c ] = local_string_home[ c ]; storage -> device_name[ storage -> device_name_limit ] = STD_ASCII_TERMINATOR;

	// storage active
	storage -> flags |= KERNEL_STORAGE_FLAGS_active;

	// select as user main storage
	kernel -> storage_user = ((uintptr_t) storage - (uintptr_t) kernel -> storage_base_address) / sizeof( struct KERNEL_STRUCTURE_STORAGE );

	//----------------------------------------------------------------------

	// start Storage thread
	uint8_t local_string_storage[] = "storage";
	kernel_module_thread( (uintptr_t) &kernel_storage, (uint8_t *) &local_string_storage, sizeof( local_string_storage ) );
}
