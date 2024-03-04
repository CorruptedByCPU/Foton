/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint64_t kernel_init_vfs( struct KERNEL_VFS_STRUCTURE *vfs, struct KERNEL_VFS_STRUCTURE *previous ) {
	// size of this directory
	uint64_t size_byte = EMPTY;

	// directory properties
	struct KERNEL_VFS_STRUCTURE *knot = (struct KERNEL_VFS_STRUCTURE *) vfs -> offset;

	// for every file
	do {
		// default file content address
		knot -> offset += vfs -> offset;

		// modify offset depending on file type
		switch( knot -> type ) {
			// for default symbolic links
			case STD_FILE_TYPE_symbolic_link: {
				// current?
				if( knot -> length == 1 && knot -> name[ 0 ] == STD_ASCII_DOT ) knot -> offset = (uintptr_t) vfs;

				// previous?
				if( knot -> length == 2 && knot -> name[ 0 ] == STD_ASCII_DOT && knot -> name[ 1 ] == STD_ASCII_DOT ) knot -> offset = (uintptr_t) previous;

				break;
			}

			// for directories
			case STD_FILE_TYPE_directory: {
				// parse directory entries
				kernel_init_vfs( (struct KERNEL_VFS_STRUCTURE *) knot, vfs ); break; }
		}

		// entry parsed
		size_byte += sizeof( struct KERNEL_VFS_STRUCTURE );
	// until end of file list
	} while( (++knot) -> length );

	// return directory size in pages
	return MACRO_PAGE_ALIGN_UP( size_byte );
}
