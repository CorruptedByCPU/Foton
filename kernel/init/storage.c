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
		// module type of VFS?
		if( lib_vfs_check( (uintptr_t) limine_module_request.response -> modules[ i ] -> address, limine_module_request.response -> modules[ i ] -> size ) ) {
			// register device of type VFS
			struct KERNEL_STORAGE_STRUCTURE *storage = kernel_storage_register( KERNEL_STORAGE_TYPE_vfs );

			// initialize Virtual File System files offsets
			kernel_init_vfs( (struct LIB_VFS_STRUCTURE *) limine_module_request.response -> modules[ i ] -> address, (uintptr_t) limine_module_request.response -> modules[ i ] -> address );

			// set device properties
			storage -> device_id = EMPTY;
			storage -> device_block = (uint64_t) limine_module_request.response -> modules[ i ] -> address;

			// size of this storage
			storage -> device_size_byte = limine_module_request.response -> modules[ i ] -> size;
		}
	}

	// find storage containing "init" file
	for( int64_t i = 0; i < KERNEL_STORAGE_limit; i++ ) {
		// empty storage entry?
		if( ! kernel -> storage_base_address[ i ].device_type ) continue;	// leave it

		// properties of file
		struct STD_FILE_STRUCTURE file = kernel_storage_file( i, (uint8_t *) "/system/bin/init", 16 );

		// storage contains requested file?
		if( file.id ) {	// yes
			// save storage ID
			kernel -> storage_root_id = i;

			// kernels storage device
			kernel -> task_base_address -> storage = i;

			#ifdef	DEBUG
				kernel -> log( (uint8_t *) "Storage: %u KiB occupied by root directory.\n", MACRO_PAGE_ALIGN_UP( kernel -> storage_base_address[ i ].device_size_byte ) >> STD_SHIFT_1024 );
			#endif
		}
	}

	// kernel storage not found?
	if( kernel -> storage_root_id == -1 ) {
		#ifdef	DEBUG
			kernel -> log( (uint8_t *) "Where are my testicles, Summer?\n" );
		#endif

		// hold the door
		while( TRUE );
	}
}
