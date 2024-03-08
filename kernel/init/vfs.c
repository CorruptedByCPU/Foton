/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint64_t kernel_init_directory( struct LIB_VFS_STRUCTURE *current, struct LIB_VFS_STRUCTURE *previous ) {
	// size of this directory
	uint64_t bytes = EMPTY;

	// directory properties
	struct LIB_VFS_STRUCTURE *file = (struct LIB_VFS_STRUCTURE *) current -> offset;

	// for every file
	do {
		// default file content address
		file -> offset += current -> offset;

		// modify offset depending on file type
		switch( file -> type ) {
			// for default symbolic links
			case STD_FILE_TYPE_symbolic_link: {
				// current?
				if( file -> name_length == 1 && file -> name[ 0 ] == STD_ASCII_DOT ) file -> offset = (uintptr_t) current;

				// previous?
				if( file -> name_length == 2 && file -> name[ 0 ] == STD_ASCII_DOT && file -> name[ 1 ] == STD_ASCII_DOT ) file -> offset = (uintptr_t) previous;

				break;
			}

			// for directories
			case STD_FILE_TYPE_directory: {
				// parse directory entries
				kernel_init_directory( (struct LIB_VFS_STRUCTURE *) file, current ); break;
			}
		}

		// entry parsed
		bytes += sizeof( struct LIB_VFS_STRUCTURE );
	// until end of file list
	} while( (++file) -> name_length );

	// return directory size in pages
	return MACRO_PAGE_ALIGN_UP( bytes );
}

void kernel_init_vfs( void ) {
	// allocate area for list of storage devices
	kernel -> vfs_base_address = (struct KERNEL_VFS_STRUCTURE *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( KERNEL_VFS_limit * sizeof( struct KERNEL_VFS_STRUCTURE ) ) >> STD_SHIFT_PAGE );

	// detect VFS storages
	for( uint64_t i = 0; i < KERNEL_STORAGE_limit; i++ ) {
		// marked as VFS
		if( kernel -> storage_base_address[ i ].type != KERNEL_STORAGE_TYPE_vfs ) continue;

		// create superblock of VFS
		struct LIB_VFS_STRUCTURE *superblock = (struct LIB_VFS_STRUCTURE *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( LIB_VFS_BLOCK_byte ) >> STD_SHIFT_PAGE );

		// superblock is of type: directory
		superblock -> type = STD_FILE_TYPE_directory;

		// root directory name
		uint8_t local_string_root[] = "/";
		for( uint64_t j = 0; j < sizeof( local_string_root ) - 1; j++ ) superblock -> name[ superblock -> name_length++ ] = local_string_root[ j ];

		// superblock content offset
		superblock -> offset = kernel -> storage_base_address[ i ].root;

		// initialize VFS
		superblock -> byte = kernel_init_directory( superblock, superblock );

		// set new location of storage root
		kernel -> storage_base_address[ i ].root = (uintptr_t) superblock;
	}

	// select VFS storage
	for( uint64_t i = 0; i < KERNEL_STORAGE_limit; i++ ) {
		// storage type of VFS?
		if( kernel -> storage_base_address[ i ].type != KERNEL_STORAGE_TYPE_vfs ) continue;

		// set as default
		kernel -> storage_root = i;

		// containing special purpose file
		uint8_t string_file_path[] = "/system/etc/version";

		// retrieve properties of file
		struct KERNEL_VFS_STRUCTURE *socket = kernel_vfs_file_open( string_file_path, sizeof( string_file_path ) - 1 );
		if( ! socket ) continue;	// file not found

		// release
		kernel_vfs_file_close( socket );

		// information
		kernel -> log( (uint8_t *) "Storage: %u KiB occupied by root directory.\n", MACRO_PAGE_ALIGN_UP( kernel -> storage_base_address[ i ].byte ) >> STD_SHIFT_1024 );

		// done
		break;
	}
}
