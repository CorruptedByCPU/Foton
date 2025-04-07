/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint64_t kernel_init_vfs_length( uintptr_t base_address ) {
	// detected size of directory
	uint64_t bytes = STD_PAGE_byte;	// default length

	// block number
	uint64_t i = 0;

	// for each data block of file
	while( TRUE ) {
		// set pointer to directory content
		struct LIB_VFS_STRUCTURE *entry = (struct LIB_VFS_STRUCTURE *) (base_address + (i << STD_SHIFT_PAGE));

		// for every entry
		for( uint8_t e = 0; e < STD_PAGE_byte / sizeof( struct LIB_VFS_STRUCTURE ); e++ ) {
			// end of entries?
			if( ! entry[ e ].name_limit ) return bytes;	// return size of directory
		}

		// extend size by block
		bytes += STD_PAGE_byte;

		// next block
		i++;
	}
}

void kernel_init_vfs_realloc( struct LIB_VFS_STRUCTURE *vfs, uint64_t offset ) {
	// length of file in blocks
	uint64_t blocks = MACRO_PAGE_ALIGN_UP( vfs -> limit ) >> STD_SHIFT_PAGE;

	// remember file content pointer
	offset += vfs -> block[ FALSE ];

	// parse direct blocks

	// update block pointer
	vfs -> block[ 0 ] = offset;

	// next block address
	offset += STD_PAGE_byte;

	// update file properties
	vfs -> block[ 1 ] = kernel -> memory_alloc( TRUE );

	// properties of indirect block
	uintptr_t *indirect = (uintptr_t *) vfs -> block[ 1 ];

	// parse indirect blocks
	for( uint16_t i = 0; i < 512; i++ ) {
		// update block pointer
		indirect[ i ] = offset;

		// next block address
		offset += STD_PAGE_byte;

		// no more blocks?
		if( ! --blocks ) return;
	}

	// no support for double-indirect and so on, yet
	// file of 2 MiB is enough for now
}

void kernel_init_vfs_setup( struct LIB_VFS_STRUCTURE *current, struct LIB_VFS_STRUCTURE *previous ) {
	// for each data block of directory
	for( uint64_t i = 0; i < MACRO_PAGE_ALIGN_UP( current -> limit ) >> STD_SHIFT_PAGE; i++ ) {
		// properties of directory entry
		struct LIB_VFS_STRUCTURE *entry = (struct LIB_VFS_STRUCTURE *) kernel_vfs_block_by_id( current, i );

		// for every possible entry
		for( uint8_t j = 0; j < STD_PAGE_byte / sizeof( struct LIB_VFS_STRUCTURE ); j++ ) {
			// if regular file
			if( entry[ j ].type == STD_FILE_TYPE_file ) {
				// realloc blocks of file
				kernel_init_vfs_realloc( (struct LIB_VFS_STRUCTURE *) &entry[ j ], current -> block[ FALSE ] + current -> limit );

				// next entry
				continue;
			}

			// if directory
			if( entry[ j ].type == STD_FILE_TYPE_directory ) {
				// calculate root directory size
				entry[ j ].limit = kernel_init_vfs_length( current -> block[ FALSE ] + current -> limit + entry[ j ].block[ FALSE ] );

				// realloc blocks of superblock
				kernel_init_vfs_realloc( (struct LIB_VFS_STRUCTURE *) &entry[ j ], current -> block[ FALSE ] + current -> limit );

				// realloc VFS structures regarded of memory location
				kernel_init_vfs_setup( (struct LIB_VFS_STRUCTURE *) &entry[ j ], current );

				// next entry
				continue;
			}

			// if symbolic link
			if( entry[ j ].type == STD_FILE_TYPE_link ) {
				// remove size information
				entry[ j ].limit = EMPTY;

				// current?
				if( entry[ j ].name_limit == 1 && entry[ j ].name[ 0 ] == STD_ASCII_DOT ) entry[ j ].block[ FALSE ] = (uintptr_t) current;

				// previous?
				if( entry[ j ].name_limit == 2 && entry[ j ].name[ 0 ] == STD_ASCII_DOT && entry[ j ].name[ 1 ] == STD_ASCII_DOT ) entry[ j ].block[ FALSE ] = (uintptr_t) previous;

				// next entry
				continue;
			}
		}
	}
}

void kernel_init_vfs( void ) {
	// allocate area for list of open files
	kernel -> vfs_base_address = (struct KERNEL_STRUCTURE_VFS *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( KERNEL_VFS_limit * sizeof( struct KERNEL_STRUCTURE_VFS ) ) >> STD_SHIFT_PAGE );

	// detect VFS storages
	for( uint64_t i = 0; i < KERNEL_STORAGE_limit; i++ ) {
		// entry type of MEMORY marked as UNDEFINIED?
		if( kernel -> storage_base_address[ i ].device_type != STD_STORAGE_TYPE_memory || kernel -> storage_base_address[ i ].device_fs != KERNEL_STORAGE_FS_undefinied ) continue;	// thats not it

		// storage type of VFS?
		if( ! kernel_vfs_identify( kernel -> storage_base_address[ i ].device_block, kernel -> storage_base_address[ i ].device_limit ) ) continue;	// no

		// create superblock for VFS
		struct LIB_VFS_STRUCTURE *superblock = (struct LIB_VFS_STRUCTURE *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( LIB_VFS_BLOCK_byte ) >> STD_SHIFT_PAGE );

		// superblock is of type: directory
		superblock -> type = STD_FILE_TYPE_directory;

		// root directory name
		superblock -> name_limit = 1;
		*superblock -> name = STD_ASCII_SLASH;

		// calculate root directory size
		superblock -> limit = kernel_init_vfs_length( kernel -> storage_base_address[ i ].device_block );

		// realloc blocks of superblock
		kernel_init_vfs_realloc( superblock, kernel -> storage_base_address[ i ].device_block );

		// realloc VFS structures based of memory location
		kernel_init_vfs_setup( superblock, superblock );

		// set new location of VFS main block
		kernel -> storage_base_address[ i ].device_block = (uint64_t) superblock;

		// kernels current directory already assigned?
		if( kernel -> task_base_address -> directory ) continue;	// yes

		// containing special purpose file
		uint8_t string_file_path[] = "/etc/hostname.txt";

		// retrieve properties of file
		struct LIB_VFS_STRUCTURE *vfs = kernel_vfs_path( string_file_path, sizeof( string_file_path ) - 1 ); if( ! vfs ) continue;	// file not found

		// set this one as default storage for kernel/system
		kernel -> storage_root = i;

		// set storage name
		uint8_t string_system[] = "System";
		kernel -> storage_base_address[ i ].device_name_limit = sizeof( string_system ) - 1;
		for( uint8_t c = 0; c < kernel -> storage_base_address[ i ].device_name_limit; c++ ) kernel -> storage_base_address[ i ].device_name[ c ] = string_system[ c ]; kernel -> storage_base_address[ i ].device_name[ kernel -> storage_base_address[ i ].device_name_limit ] = STD_ASCII_TERMINATOR;

		// kernels current directory
		kernel -> task_base_address -> directory = kernel -> storage_base_address[ i ].device_block;

		// share essential functions
		kernel -> storage_base_address[ i ].fs.root_directory_id = kernel -> storage_base_address[ i ].device_block;
		kernel -> storage_base_address[ i ].fs.dir = (void *) kernel_vfs_dir;
		kernel -> storage_base_address[ i ].fs.touch = (void *) kernel_vfs_touch;
		kernel -> storage_base_address[ i ].fs.open = (void *) kernel_vfs_file_open;
		kernel -> storage_base_address[ i ].fs.file = (void *) kernel_vfs_file_properties;
		kernel -> storage_base_address[ i ].fs.close = (void *) kernel_vfs_file_close;
		kernel -> storage_base_address[ i ].fs.write = (void *) kernel_vfs_file_write;
		kernel -> storage_base_address[ i ].fs.read = (void *) kernel_vfs_file_read;

		// attach read/write functions
		kernel -> storage_base_address[ i ].block_read = (void *) kernel_vfs_read;
		kernel -> storage_base_address[ i ].block_write = (void *) kernel_vfs_write;

		// set storage type
		kernel -> storage_base_address[ i ].device_fs = KERNEL_STORAGE_FS_vfs;
	}
}
