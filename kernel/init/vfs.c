/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void NEW_kernel_init_vfs( void ) {
	// // detect VFS storages
	// for( uint64_t i = 0; i < NEW_KERNEL_STORAGE_limit; i++ ) {
	// 	// marked as VFS
	// 	if( kernel -> NEW_storage_base_address[ i ].type != NEW_KERNEL_STORAGE_TYPE_vfs ) continue;

	// 	// create superblock for VFS
	// 	struct NEW_LIB_VFS_STRUCTURE *superblock = (struct LIB_VFS_STRUCTURE *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( LIB_VFS_BLOCK_byte ) >> STD_SHIFT_PAGE );

	// 	// superblock is of type: directory
	// 	superblock -> type = STD_FILE_TYPE_directory;

	// 	// root directory name
	// 	uint8_t local_string_root[] = "/";
	// 	for( uint64_t j = 0; j < sizeof( local_string_root ) - 1; j++ ) superblock -> name[ superblock -> name_length++ ] = local_string_root[ j ];

	// 	// superblock content offset
	// 	superblock -> offset = kernel -> storage_base_address[ i ].root;

	// 	// initialize VFS
	// 	superblock -> byte = kernel_init_directory( superblock, superblock );

	// 	// set new location of storage root
	// 	kernel -> storage_base_address[ i ].root = (uintptr_t) superblock;
	// }
}

uint64_t DEPRECATED_kernel_init_vfs( struct EXCHANGE_KERNEL_VFS_STRUCTURE *vfs, struct EXCHANGE_KERNEL_VFS_STRUCTURE *previous ) {
	// size of this directory
	uint64_t size_byte = EMPTY;

	// directory properties
	struct EXCHANGE_KERNEL_VFS_STRUCTURE *knot = (struct EXCHANGE_KERNEL_VFS_STRUCTURE *) vfs -> offset;

	// for every file
	do {
		// default file content address
		knot -> offset += vfs -> offset;

		// modify offset depending on file type
		switch( knot -> type ) {
			// for default symbolic links
			case DEPRECATED_STD_FILE_TYPE_symbolic_link: {
				// current?
				if( knot -> name_length == 1 && knot -> name[ 0 ] == STD_ASCII_DOT ) knot -> offset = (uintptr_t) vfs;

				// previous?
				if( knot -> name_length == 2 && knot -> name[ 0 ] == STD_ASCII_DOT && knot -> name[ 1 ] == STD_ASCII_DOT ) knot -> offset = (uintptr_t) previous;

				break;
			}

			// for directories
			case DEPRECATED_STD_FILE_TYPE_directory: {
				// parse directory entries
				DEPRECATED_kernel_init_vfs( (struct EXCHANGE_KERNEL_VFS_STRUCTURE *) knot, vfs ); break; }
		}

		// entry parsed
		size_byte += sizeof( struct EXCHANGE_KERNEL_VFS_STRUCTURE );
	// until end of file list
	} while( (++knot) -> name_length );

	// return directory size in pages
	return MACRO_PAGE_ALIGN_UP( size_byte );
}
