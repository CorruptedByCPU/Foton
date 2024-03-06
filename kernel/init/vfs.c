/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint64_t kernel_init_vfs( struct LIB_VFS_STRUCTURE *vfs, struct LIB_VFS_STRUCTURE *previous ) {
	// size of this directory
	uint64_t bytes = EMPTY;

	// directory properties
	struct LIB_VFS_STRUCTURE *knot = (struct LIB_VFS_STRUCTURE *) vfs -> offset;

	// for every file
	do {
		// default file content address
		knot -> offset += vfs -> offset;

		// modify offset depending on file type
		switch( knot -> type ) {
			// for default symbolic links
			case STD_FILE_TYPE_symbolic_link: {
				// current?
				if( knot -> name_length == 1 && knot -> name[ 0 ] == STD_ASCII_DOT ) knot -> offset = (uintptr_t) vfs;

				// previous?
				if( knot -> name_length == 2 && knot -> name[ 0 ] == STD_ASCII_DOT && knot -> name[ 1 ] == STD_ASCII_DOT ) knot -> offset = (uintptr_t) previous;

				break;
			}

			// for directories
			case STD_FILE_TYPE_directory: {
				// parse directory entries
				kernel_init_vfs( (struct LIB_VFS_STRUCTURE *) knot, vfs ); break;
			}
		}

		// entry parsed
		bytes += sizeof( struct LIB_VFS_STRUCTURE );
	// until end of file list
	} while( (++knot) -> name_length );

	// return directory size in pages
	return MACRO_PAGE_ALIGN_UP( bytes );
}
