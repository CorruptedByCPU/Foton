/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_storage( void ) {
	// allocate area for list of available storages
	kernel -> storage_base_address = (struct KERNEL_STRUCTURE_STORAGE *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( KERNEL_STORAGE_limit * sizeof( struct KERNEL_STRUCTURE_STORAGE ) ) >> STD_SHIFT_PAGE );

	// register modules as storages
	for( uint64_t i = 0; i < limine_module_request.response -> module_count; i++ ) {
		// register device of type MEMORY
		struct KERNEL_STRUCTURE_STORAGE *storage = (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ kernel_storage_add() ];

		// storage class
		storage -> device_type = STD_STORAGE_TYPE_memory;

		// file system type
		storage -> device_fs = KERNEL_STORAGE_FS_undefinied;

		// address of main block
		storage -> device_block = (uintptr_t) limine_module_request.response -> modules[ i ] -> address;

		// default block size in Bytes
		storage -> device_byte = STD_PAGE_byte;

		// storage size in Bytes
		storage -> device_limit = limine_module_request.response -> modules[ i ] -> size;
	
		// storage active
		storage -> flags |= KERNEL_STORAGE_FLAGS_active;
	}

	//----------------------------------------------------------------------

	// register device of type VFS
	struct KERNEL_STRUCTURE_STORAGE *storage = (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ kernel_storage_add() ];

	// storage class
	storage -> device_type = STD_STORAGE_TYPE_memory;

	// file system type: VFS
	storage -> device_fs = KERNEL_STORAGE_FS_vfs;

	// default block size in Bytes
	storage -> device_byte = kernel_vfs_format( storage );

	// length of storage in Blocks
	storage -> device_limit = kernel -> page_total << STD_SHIFT_PAGE;	// unlimited up to size of RAM

	// set storage name
	uint8_t string_tempfs[] = "RamFS";
	storage -> device_name_limit = sizeof( string_tempfs ) - 1;
	for( uint8_t c = 0; c < storage -> device_name_limit; c++ ) storage -> device_name[ c ] = string_tempfs[ c ]; storage -> device_name[ storage -> device_name_limit ] = STD_ASCII_TERMINATOR;

	// attach read/write functions
	storage -> block_read = (void *) kernel_vfs_read;
	storage -> block_write = (void *) kernel_vfs_write;

	// share essential functions
	storage -> fs.root_directory_id = storage -> device_block;
	storage -> fs.dir = (void *) kernel_vfs_dir;
	storage -> fs.touch = (void *) kernel_vfs_touch;
	storage -> fs.open = (void *) kernel_vfs_file_open;
	storage -> fs.file = (void *) kernel_vfs_file_properties;
	storage -> fs.close = (void *) kernel_vfs_file_close;
	storage -> fs.write = (void *) kernel_vfs_file_write;
	storage -> fs.read = (void *) kernel_vfs_file_read;

	// storage active
	storage -> flags |= KERNEL_STORAGE_FLAGS_active;

	// select as user main storage
	kernel -> storage_user = ((uintptr_t) storage - (uintptr_t) kernel -> storage_base_address) / sizeof( struct KERNEL_STRUCTURE_STORAGE );

	//----------------------------------------------------------------------

	// start Storage thread
	uint8_t storage_string_thread_name[] = "storage";
	kernel_module_thread( (uintptr_t) &kernel_storage, (uint8_t *) &storage_string_thread_name, sizeof( storage_string_thread_name ) );
}
