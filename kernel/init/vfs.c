/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint64_t kernel_init_vfs_realloc( struct LIB_VFS_STRUCTURE *current, struct LIB_VFS_STRUCTURE *previous ) {
	// size of this directory
	uint64_t bytes = EMPTY;

	// file properties
	struct LIB_VFS_STRUCTURE *file = (struct LIB_VFS_STRUCTURE *) current -> offset;

	// for every file
	do {
		// default file content address
		file -> offset += current -> offset;

		// modify offset depending on file type
		switch( file -> type ) {
			// for default symbolic links
			case STD_FILE_TYPE_link: {
				// current?
				if( file -> name_length == 1 && file -> name[ 0 ] == STD_ASCII_DOT ) file -> offset = (uintptr_t) current;

				// previous?
				if( file -> name_length == 2 && file -> name[ 0 ] == STD_ASCII_DOT && file -> name[ 1 ] == STD_ASCII_DOT ) file -> offset = (uintptr_t) previous;

				// done
				break;
			}

			// for directories
			case STD_FILE_TYPE_directory: {
				// parse directory entries
				file -> byte = MACRO_PAGE_ALIGN_UP( kernel_init_vfs_realloc( (struct LIB_VFS_STRUCTURE *) file, current ) );
				
				// done
				break;
			}
		}

		// entry parsed
		bytes += sizeof( struct LIB_VFS_STRUCTURE );
	// until end of file list
	} while( (++file) -> name_length );

	// return directory size in Bytes
	return MACRO_PAGE_ALIGN_UP( bytes );
}

void kernel_init_vfs( void ) {
	// allocate area for list of open files
	kernel -> vfs_base_address = (struct KERNEL_STRUCTURE_VFS *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( KERNEL_VFS_limit * sizeof( struct KERNEL_STRUCTURE_VFS ) ) >> STD_SHIFT_PAGE );

	// detect VFS storages
	for( uint64_t i = 0; i < KERNEL_STORAGE_limit; i++ ) {
		// entry marked as VFS?
		if( kernel -> storage_base_address[ i ].device_type != KERNEL_STORAGE_TYPE_vfs ) continue;	// thats not it

		// create superblock for VFS
		struct LIB_VFS_STRUCTURE *superblock = (struct LIB_VFS_STRUCTURE *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( LIB_VFS_block ) >> STD_SHIFT_PAGE );

		// superblock is of type: directory
		superblock -> type = STD_FILE_TYPE_directory;

		// root directory name
		superblock -> name_length = 1;
		*superblock -> name = STD_ASCII_SLASH;

		// superblock content offset
		superblock -> offset = kernel -> storage_base_address[ i ].device_block;

		// realloc VFS structures regarded of memory location
		superblock -> byte = kernel_init_vfs_realloc( superblock, superblock );

		// set new location of VFS main block
		kernel -> storage_base_address[ i ].device_block = (uint64_t) superblock;

		// kernels current directory already assigned?
		if( kernel -> task_base_address -> directory ) continue;	// yes

		// containing special purpose file
		uint8_t string_file_path[] = "/system/etc/version";

		// retrieve properties of file
		struct LIB_VFS_STRUCTURE *vfs = kernel_vfs_path( string_file_path, sizeof( string_file_path ) - 1 ); if( ! vfs ) continue;	// file not found

		// set this one as default storage
		kernel -> storage_root = i;

		// kernels current directory
		kernel -> task_base_address -> directory = (struct LIB_VFS_STRUCTURE *) kernel -> storage_base_address[ i ].device_block;
	}
}