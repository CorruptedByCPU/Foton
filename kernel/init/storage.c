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

	// initialize storages
	for( uint64_t i = 0; i < KERNEL_STORAGE_limit; i++ ) {
		// marked as
		switch( kernel -> storage_base_address[ i ].type ) {
			// VFS
			case KERNEL_STORAGE_TYPE_vfs: {
				// properties of registered storage
				struct KERNEL_STORAGE_STRUCTURE *storage = (struct KERNEL_STORAGE_STRUCTURE *) &kernel -> storage_base_address[ kernel_storage_register( KERNEL_STORAGE_TYPE_vfs ) ];

				// craete superblock of VFS
				struct LIB_VFS_STRUCTURE *superblock = (struct LIB_VFS_STRUCTURE *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( LIB_VFS_BLOCK_byte ) >> STD_SHIFT_PAGE );

				// superblock is of type: directory
				superblock -> type = STD_FILE_TYPE_directory;

				// root directory name
				uint8_t local_string_root[] = "/";
				for( uint64_t j = 0; j < sizeof( local_string_root ) - 1; j++ ) superblock -> name[ superblock -> name_length++ ] = local_string_root[ j ];

				// superblock content offset
				superblock -> offset = storage -> root;

				// initialize VFS
				kernel_init_vfs( superblock, superblock );

				// set new location of storage root
				storage -> root = (uintptr_t) superblock;
			}
		}
	}

// OLD ========================================================================

	// // find storage containing "init" file
	// for( int64_t i = 0; i < KERNEL_STORAGE_OLD_limit; i++ ) {
	// 	// empty storage entry?
	// 	if( ! kernel -> storage_old_base_address[ i ].device_type ) continue;	// leave it

	// 	// properties of file
	// 	struct STD_FILE_OLD_STRUCTURE file = { EMPTY };

	// 	// set file path name
	// 	uint8_t path[ 19 ] = "/system/etc/version";
	// 	for( uint64_t j = 0; j < sizeof( path ); j++ ) file.name[ file.length++ ] = path[ j ];

	// 	// retrieve properties of file
	// 	file.id_storage = i;
	// 	kernel_storage_old_file( (struct STD_FILE_OLD_STRUCTURE *) &file );

	// 	// storage contains requested file?
	// 	if( file.id ) {	// yes
	// 		// save storage ID
	// 		kernel -> storage_old_root_id = i;

	// 		// kernel storage device
	// 		kernel -> task_base_address -> storage = i;

	// 		// kernel root directory
	// 		kernel -> task_base_address -> directory = kernel -> storage_old_base_address[ i ].device_block;

	// 		#ifdef	DEBUG
	// 			kernel -> log( (uint8_t *) "Storage: %u KiB occupied by root directory.\n", MACRO_PAGE_ALIGN_UP( kernel -> storage_old_base_address[ i ].device_size_byte ) >> STD_SHIFT_1024 );
	// 		#endif
	// 	}
	// }

	// // kernel storage not found?
	// if( kernel -> storage_old_root_id == -1 ) {
	// 	#ifdef	DEBUG
	// 		kernel -> log( (uint8_t *) "Where are my testicles, Summer?\n" );
	// 	#endif

	// 	// hold the door
	// 	while( TRUE );
	// }
}
