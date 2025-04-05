/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t	kernel_init_vfs_identify( uintptr_t base, uint64_t limit ) {
	// file properties in chunks
	uint32_t *vfs = (uint32_t *) base;

	// at end of file, magic value exist?
	if( vfs[ (limit >> STD_SHIFT_4) - 1 ] == LIB_VFS_magic ) return TRUE;	// yes

	// no
	return FALSE;
}

uint64_t kernel_init_vfs_length( uintptr_t base_address ) {
	// detected size of directory
	uint64_t limit = INIT;

	// set pointer to directory content
	struct LIB_VFS_STRUCTURE *vfs = (struct LIB_VFS_STRUCTURE *) base_address;

	// for every entry
	do {
		// expand limit by VFS entry
		limit += sizeof( struct LIB_VFS_STRUCTURE );
	// until empty one
	} while( (vfs++) -> type );

	// directory length
	return MACRO_PAGE_ALIGN_UP( limit );
}

struct LIB_VFS_STRUCTURE *kernel_init_vfs_search( struct LIB_VFS_STRUCTURE *dir, uint8_t *name, uint64_t limit ) {
	// for each data block of directory
	for( uint64_t b = INIT; b < MACRO_PAGE_ALIGN_UP( dir -> limit ) >> STD_SHIFT_PAGE; b++ ) {
		// properties of directory content
		struct LIB_VFS_STRUCTURE *vfs = (struct LIB_VFS_STRUCTURE *) kernel_vfs_block_by_id( dir, b );

		// for every possible entry
		for( uint8_t e = INIT; e < LIB_VFS_BLOCK_byte / sizeof( struct LIB_VFS_STRUCTURE ); e++ )
			// if 
			if( vfs[ e ].name_limit == limit && lib_string_compare( (uint8_t *) vfs[ e ].name, name, limit ) ) return (struct LIB_VFS_STRUCTURE *) &vfs[ e ];
	}

	// file not found
	return EMPTY;
}

uint8_t kernel_init_vfs_check( struct LIB_VFS_STRUCTURE *vfs, uint8_t *path ) {
	// until found
	while( TRUE ) {
		// remove leading slash
		while( *path == STD_ASCII_SLASH ) path++;

		// calculate file name
		uint64_t limit = lib_string_word_end( path, lib_string_length( path ), STD_ASCII_SLASH );

		// search for file inside current directory
		struct LIB_VFS_STRUCTURE *found = (struct LIB_VFS_STRUCTURE *) kernel_init_vfs_search( vfs, path, limit );

		// file not found?
		if( ! found ) return FALSE;	// yep

		// follow symbolic links (if possible)
		while( found -> type & STD_FILE_TYPE_link ) found = (struct LIB_VFS_STRUCTURE *) found -> block[ 0 ];

		// last file from path?
		if( limit == lib_string_length( path ) ) return TRUE;	// yes

		// next file is a directory?
		if( ! (found -> type & STD_FILE_TYPE_directory) ) return FALSE;	// no!

		// start from next directory
		vfs = found;

		// remove current directory name from path
		path += limit;
	}
}

void kernel_init_vfs_realloc( struct LIB_VFS_STRUCTURE *vfs, uintptr_t offset ) {
	// directory blocks
	uint64_t b = vfs -> limit >> STD_SHIFT_PAGE;

	// true block offset
	offset += vfs -> block[ FALSE ];
	vfs -> block[ 0 ] = offset;

	// no more blocks?
	if( ! --b ) return;	// yes

	// next block address
	offset += STD_PAGE_byte;

	// craete indirect block
	vfs -> block[ 1 ] = kernel_memory_alloc( TRUE );

	// indirect block properties
	uintptr_t *indirect = (uintptr_t *) vfs -> block[ 1 ];

	// parse indirect blocks
	for( uint16_t i = INIT; i < 512; i++ ) {
		// true block offset
		indirect[ i ] = offset;

		// next block address
		offset += STD_PAGE_byte;

		// no more blocks?
		if( ! --b ) return;	// yes
	}

	// no support for double-indirect and so on, yet
	// file of 2 MiB is enough for now
}

void kernel_init_vfs_setup( struct LIB_VFS_STRUCTURE *current, struct LIB_VFS_STRUCTURE *previous ) {
	// for each data block of directory
	for( uint64_t b = INIT; b < MACRO_PAGE_ALIGN_UP( current -> limit ) >> STD_SHIFT_PAGE; b++ ) {
		// properties of directory content
		struct LIB_VFS_STRUCTURE *dir = (struct LIB_VFS_STRUCTURE *) kernel_vfs_block_by_id( current, b );

		// for every possible entry
		for( uint8_t e = INIT; e < LIB_VFS_BLOCK_byte / sizeof( struct LIB_VFS_STRUCTURE ); e++ )
			// depending of file type
			switch( dir[ e ].type ) {
				case STD_FILE_TYPE_file: {
					// realloc blocks of file
					kernel_init_vfs_realloc( (struct LIB_VFS_STRUCTURE *) &dir[ e ], current -> block[ 0 ] + current -> limit );

					// done
					break;
				}

				case STD_FILE_TYPE_directory: {
					// set directory limit in Bytes
					dir[ e ].limit = kernel_init_vfs_length( current -> block[ 0 ] + current -> limit + dir[ e ].block[ 0 ] );

					// realloc blocks of directory
					kernel_init_vfs_realloc( (struct LIB_VFS_STRUCTURE *) &dir[ e ], current -> block[ 0 ] + current -> limit );

					// realloc blocks of VFS directory
					kernel_init_vfs_setup( (struct LIB_VFS_STRUCTURE *) &dir[ e ], current );

					// done
					break;
				}

				case STD_FILE_TYPE_link: {
					// make sure, there is no limit set, links are empty!
					dir[ e ].limit = EMPTY;

					// set link to current entry?
					if( dir[ e ].name_limit == 1 && dir[ e ].name[ 0 ] == STD_ASCII_DOT ) dir[ e ].block[ FALSE ] = (uintptr_t) current;

					// set link to previous entry?
					if( dir[ e ].name_limit == 2 && dir[ e ].name[ 0 ] == STD_ASCII_DOT && dir[ e ].name[ 1 ] == STD_ASCII_DOT ) dir[ e ].block[ FALSE ] = (uintptr_t) previous;

					// done
					break;
				}
			}
	}
}

void kernel_init_vfs( void ) {
	// allocate area for list of open files
	kernel -> vfs_limit = KERNEL_VFS_limit;
	kernel -> vfs_base_address = (struct KERNEL_STRUCTURE_VFS_SOCKET *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( kernel -> vfs_limit * sizeof( struct KERNEL_STRUCTURE_VFS_SOCKET ) ) >> STD_SHIFT_PAGE );

	// create list of sharable file management functions
	kernel -> vfs = (struct KERNEL_STRUCTURE_VFS *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct KERNEL_STRUCTURE_VFS ) ) >> STD_SHIFT_PAGE );
	kernel -> vfs -> file			= kernel_vfs_file;
	kernel -> vfs -> file_read		= kernel_vfs_file_read;

	// detect modules with Virtual File System (like initramfs from GNU/Linux)
	for( uint64_t i = INIT; i < limine_module_request.response -> module_count; i++ ) {
		// VFS module type?
		if( ! kernel_init_vfs_identify( (uintptr_t) limine_module_request.response -> modules[ i ] -> address, (uint64_t) limine_module_request.response -> modules[ i ] -> size ) ) continue;	// no

		// create root superblock for VFS
		struct LIB_VFS_STRUCTURE *vfs = (struct LIB_VFS_STRUCTURE *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( LIB_VFS_BLOCK_byte ) >> STD_SHIFT_PAGE );
	
		// default properties of superblock
		vfs -> type		= STD_FILE_TYPE_directory;
		vfs -> name_limit	= TRUE; *vfs -> name = STD_ASCII_SLASH;
		vfs -> limit		= kernel_init_vfs_length( (uintptr_t) limine_module_request.response -> modules[ i ] -> address );

		// realloc blocks of VFS superblock
		kernel_init_vfs_realloc( vfs, (uintptr_t) limine_module_request.response -> modules[ i ] -> address );

		// realloc directory content structures
		kernel_init_vfs_setup( vfs, vfs );

		// take a look inside VFS for special file
		if( ! kernel_init_vfs_check( vfs, (uint8_t *) "/etc/hostname" ) ) {
			// release created superblock
			kernel_memory_release( (uintptr_t) vfs, MACRO_PAGE_ALIGN_UP( LIB_VFS_BLOCK_byte ) >> STD_SHIFT_PAGE );

			// next module
			continue;
		}

		// update list of sharable file management functions/variables
		kernel -> vfs -> root = (uint64_t) vfs;	// root directory id

		//--------------------------------------------------------------

		// create storage
		struct KERNEL_STRUCTURE_STORAGE *storage = (struct KERNEL_STRUCTURE_STORAGE *) kernel_storage_create();

		// main block data at
		storage -> block = (uint64_t) vfs;

		// set already definied filesystem
		storage -> vfs = kernel -> vfs;

		// storage is active from now on
		storage -> flags |= KERNEL_STORAGE_FLAGS_active;
	}
}
