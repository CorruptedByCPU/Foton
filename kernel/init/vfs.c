/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_vfs( struct LIB_VFS_STRUCTURE *vfs, uintptr_t root ) {
	// current directory address
	uintptr_t dir = (uintptr_t) vfs;

	// for every file
	do {
		// default file content address
		vfs -> offset += dir;

		// modify offset depending on file type
		switch( vfs -> type ) {
			// for default symbolic links
			case LIB_VFS_TYPE_symbolic_link: { if( vfs -> length == 2 && lib_string_compare( vfs -> name, (uint8_t *) "..", vfs -> length ) ) vfs -> offset = root; break; }

			// for directories
			case LIB_VFS_TYPE_directory: { kernel_init_vfs( (struct LIB_VFS_STRUCTURE *) vfs -> offset, dir ); break; }
		}
	// until end of file list
	} while( (++vfs) -> length );
}
