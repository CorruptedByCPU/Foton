/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint64_t NEW_kernel_init_directory( struct EXCHANGE_LIB_VFS_STRUCTURE *current, struct EXCHANGE_LIB_VFS_STRUCTURE *previous ) {
	// size of this directory
	uint64_t bytes = EMPTY;

	// file properties
	struct EXCHANGE_LIB_VFS_STRUCTURE *file = (struct EXCHANGE_LIB_VFS_STRUCTURE *) current -> offset;

	// for every file
	do {
		// default file content address
		file -> offset += current -> offset;

		// modify offset depending on file type
		switch( file -> type ) {
			// for default symbolic links
			case NEW_STD_FILE_TYPE_link: {
				// current?
				if( file -> name_length == 1 && file -> name[ 0 ] == STD_ASCII_DOT ) file -> offset = (uintptr_t) current;

				// previous?
				if( file -> name_length == 2 && file -> name[ 0 ] == STD_ASCII_DOT && file -> name[ 1 ] == STD_ASCII_DOT ) file -> offset = (uintptr_t) previous;

				// done
				break;
			}

			// for directories
			case NEW_STD_FILE_TYPE_directory: {
				// parse directory entries
				file -> byte = MACRO_PAGE_ALIGN_UP( NEW_kernel_init_directory( (struct EXCHANGE_LIB_VFS_STRUCTURE *) file, current ) );
				
				// done
				break;
			}
		}

		// entry parsed
		bytes += sizeof( struct EXCHANGE_LIB_VFS_STRUCTURE );
	// until end of file list
	} while( (++file) -> name_length );

	// return directory size in pages
	return MACRO_PAGE_ALIGN_UP( bytes );
}

void NEW_kernel_init_vfs( void ) {
	// allocate area for list of open sockets
	kernel -> NEW_vfs_base_address = (struct NEW_KERNEL_VFS_STRUCTURE *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( NEW_KERNEL_VFS_limit * sizeof( struct NEW_KERNEL_VFS_STRUCTURE ) ) >> STD_SHIFT_PAGE );

	// detect VFS storages
	for( uint64_t i = 0; i < NEW_KERNEL_STORAGE_limit; i++ ) {
		// marked as VFS
		if( kernel -> NEW_storage_base_address[ i ].device_type != NEW_KERNEL_STORAGE_TYPE_vfs ) continue;

		// create superblock for VFS
		struct EXCHANGE_LIB_VFS_STRUCTURE *superblock = (struct EXCHANGE_LIB_VFS_STRUCTURE *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( NEW_LIB_VFS_block ) >> STD_SHIFT_PAGE );

		// superblock is of type: directory
		superblock -> type = NEW_STD_FILE_TYPE_directory;

		// root directory name
		superblock -> name_length = 1;
		superblock -> name[ 0 ] = '/';
		// uint8_t local_string_root[] = "/";
		// for( uint64_t j = 0; j < sizeof( local_string_root ) - 1; j++ ) superblock -> name[ superblock -> name_length++ ] = local_string_root[ j ];

		// superblock content offset
		superblock -> offset = kernel -> NEW_storage_base_address[ i ].device_block;

		// realloc VFS structures regarded of memory location
		superblock -> byte = NEW_kernel_init_directory( superblock, superblock );
		// superblock -> byte = STD_PAGE_byte;

		// set new location of storage root
		kernel -> NEW_storage_base_address[ i ].device_block = (uint64_t) superblock;
	}

	// select VFS storage
	for( uint64_t i = 0; i < NEW_KERNEL_STORAGE_limit; i++ ) {
		// storage type of VFS?
		if( kernel -> NEW_storage_base_address[ i ].device_type != NEW_KERNEL_STORAGE_TYPE_vfs ) continue;

		// set as default
		kernel -> NEW_storage_root = i;

		// containing special purpose file
		uint8_t string_file_path[] = "/system/etc/version";

		// retrieve properties of file
		struct NEW_KERNEL_VFS_STRUCTURE *socket = NEW_kernel_vfs_file_open( string_file_path, sizeof( string_file_path ) - 1, NEW_KERNEL_VFS_MODE_read );
		if( ! socket ) continue;	// file not found

		// kernels current directory
		kernel -> task_base_address -> directory = kernel -> NEW_storage_base_address[ i ].device_block;

		// release
		NEW_kernel_vfs_file_close( socket );

		// show information about root directory
		kernel -> log( (uint8_t *) "VFS: %u KiB occupied by root directory.\n", MACRO_PAGE_ALIGN_UP( kernel -> NEW_storage_base_address[ i ].device_length * kernel -> NEW_storage_base_address[ i ].device_byte ) >> STD_SHIFT_1024 );

		// done
		break;
	}
}