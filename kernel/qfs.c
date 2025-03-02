/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_qfs_format( uint64_t storage_id ) {
	// properties of storage
	struct KERNEL_STRUCTURE_STORAGE *storage = (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ storage_id ];

	//----------------------------------------------------------------------

	// prepare superblock
	struct LIB_VFS_STRUCTURE *superblock = (struct LIB_VFS_STRUCTURE *) kernel -> memory_alloc( TRUE );

	// root directory size in blocks
	superblock -> limit = TRUE;

	// bitmap length in blocks
	uint64_t bitmap_block = ((MACRO_PAGE_ALIGN_DOWN( storage -> device_byte * storage -> device_limit ) >> STD_SHIFT_PAGE) - (TRUE + TRUE)) / 32768;
	if( ((MACRO_PAGE_ALIGN_DOWN( storage -> device_byte * storage -> device_limit ) >> STD_SHIFT_PAGE) - (TRUE + TRUE)) % 32768 ) bitmap_block++;

	// first block of root directory
	superblock -> block[ FALSE ] = TRUE + bitmap_block;

	// type
	superblock -> type = STD_FILE_TYPE_directory;

	// name
	superblock -> name_limit = 1;
	superblock -> name[ FALSE ] = STD_ASCII_SLASH;

	// write inside first usable block of storage
	storage -> block_write( storage_id, storage -> device_block, (uint8_t *) superblock, LIB_VFS_BLOCK_byte / storage -> device_byte );

	// release superblock content
	kernel -> memory_release( (uintptr_t) superblock, TRUE );

	//----------------------------------------------------------------------

	// prepare bitmap
	uint32_t *bitmap = (uint32_t *) kernel -> memory_alloc( bitmap_block );

	// clean'up
	for( uint64_t i = 0; i < bitmap_block << STD_SHIFT_1024; i++ ) bitmap[ i ] = STD_MAX_unsigned;

	// mark pages used by file system structures as unusable
	for( uint64_t i = 0; i < (TRUE + bitmap_block + TRUE); i++ ) bitmap[ i >> STD_SHIFT_32 ] &= ~(1 << (i & 0b00011111) );

	// write after superblock
	storage -> block_write( storage_id, storage -> device_block + (LIB_VFS_BLOCK_byte / 512), (uint8_t *) bitmap, (bitmap_block << STD_SHIFT_PAGE) / storage -> device_byte );

	// release bitmap content
	kernel -> memory_release( (uintptr_t) bitmap, bitmap_block );

	//----------------------------------------------------------------------

	// prepare root directory
	struct LIB_VFS_STRUCTURE *root = (struct LIB_VFS_STRUCTURE *) kernel -> memory_alloc( TRUE );

	// prepare default symlinks for root directory

	// current symlink
	root[ 0 ].block[ FALSE ]	= EMPTY;	// pointing to superblock!
	root[ 0 ].type			= STD_FILE_TYPE_link;
	root[ 0 ].name_limit		= 1;
	root[ 0 ].name[ 0 ]		= STD_ASCII_DOT;

	// previous symlink
	root[ 1 ].block[ FALSE ]	= EMPTY;	// pointing to superblock!
	root[ 1 ].type			= STD_FILE_TYPE_link;
	root[ 1 ].name_limit		= 2;
	root[ 1 ].name[ 0 ]		= STD_ASCII_DOT;
	root[ 1 ].name[ 1 ]		= STD_ASCII_DOT;

	// write after bitmap
	storage -> block_write( storage_id, storage -> device_block + ((TRUE + bitmap_block) * 8), (uint8_t *) root, LIB_VFS_BLOCK_byte / storage -> device_byte );

	// release root directory content
	kernel -> memory_release( (uintptr_t) root, TRUE );
}

uint8_t kernel_qfs_identify( uint64_t storage_id ) {
	// lets assume, that is Quark File system
	uint8_t qfs = TRUE;

	// properties of storage
	struct KERNEL_STRUCTURE_STORAGE *storage = (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ storage_id ];

	// properties of knot under consideration
	struct LIB_VFS_STRUCTURE *vfs = (struct LIB_VFS_STRUCTURE *) kernel -> memory_alloc( TRUE );

	// load superblock knot
	storage -> block_read( storage -> device_id, storage -> device_block, (uint8_t *) vfs, LIB_VFS_BLOCK_byte / storage -> device_byte );

	// knot type of directory?
	if( ! (vfs -> type & STD_FILE_TYPE_directory ) ) qfs = FALSE;	// no

	// knot name length is correct?
	if( vfs -> name_limit != TRUE ) qfs = FALSE;	// no

	// knot name is correct?
	if( vfs -> name[ FALSE ] != STD_ASCII_SLASH ) qfs = FALSE;	// no

	// first block of knot is within limits of storage?
	if( (vfs -> block[ FALSE ] * (LIB_VFS_BLOCK_byte / storage -> device_byte)) >= storage -> device_limit ) qfs = FALSE;	// no

	// continue?
	if( qfs ) {
		// load first block of root directory content
		storage -> block_read( storage -> device_id, storage -> device_block + (vfs -> block[ FALSE ] * (LIB_VFS_BLOCK_byte / storage -> device_byte)), (uint8_t *) vfs, LIB_VFS_BLOCK_byte / storage -> device_byte );

		// knot type of link?
		if( ! (vfs -> type & STD_FILE_TYPE_link ) ) qfs = FALSE;	// no

		// knot name length is correct?
		if( vfs -> name_limit != TRUE ) qfs = FALSE;	// no

		// knot name is correct?
		if( vfs -> name[ FALSE ] != STD_ASCII_SLASH ) qfs = FALSE;	// no

		// link leads to superblock?
		if( ! vfs -> block[ FALSE ] ) qfs = FALSE;	// no
	}

	// release knot
	kernel -> memory_release( (uintptr_t) vfs, TRUE );

	// if properly recognized
	if( qfs ) {
		// connect essential functions
		// storage -> fs -> open = (void *) kernel_qfs_open;
	}

	// it is?
	return qfs;
}

struct LIB_VFS_STRUCTURE kernel_qfs_file( uint64_t storage_id, uint64_t file_id ) {
	// properties of storage
	struct KERNEL_STRUCTURE_STORAGE *storage = (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ storage_id ];

	// allocate block of data
	uint8_t *block_data = (uint8_t *) kernel -> memory_alloc( TRUE );

	// load block of data containing knot
	storage -> block_read( storage -> device_id, storage -> device_block + ((file_id & STD_PAGE_mask) >> STD_SHIFT_PAGE), block_data, LIB_VFS_BLOCK_byte / storage -> device_byte );

// debug, memory
// for( uint8_t i = 0; i < 8; i ++ ) { kernel -> log( (uint8_t *) "0x%16X", (uintptr_t) block_data + (16 * i) ); for( uint8_t j = 0; j < 16; j++ ) kernel -> log( (uint8_t *) " %2X", block_data[ (16 * i) + j ] ); kernel -> log( (uint8_t *) "\n" ); }

	// properties of file
	struct LIB_VFS_STRUCTURE vfs = *((struct LIB_VFS_STRUCTURE *) &block_data[ file_id & ~STD_PAGE_mask ]);

	// return file properties
	return vfs;
}

uint64_t kernel_qfs_search( uint64_t storage_id, uint64_t directory_id, uint8_t *name, uint64_t name_length ) {
	// properties of storage
	struct KERNEL_STRUCTURE_STORAGE *storage = (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ storage_id ];

	// allocate block of data
	uint8_t *block_data = (uint8_t *) kernel -> memory_alloc( TRUE );

// 	// for each data block of directory
// 	for( uint64_t b = 0; b < (directory -> limit >> STD_SHIFT_PAGE); b++ ) {
// 		// properties of directory entry
// 		struct LIB_VFS_STRUCTURE *entry = (struct LIB_VFS_STRUCTURE *) kernel_vfs_block_by_id( directory, b );

// 		// for every possible entry
// 		for( uint8_t e = 0; e < STD_PAGE_byte / sizeof( struct LIB_VFS_STRUCTURE ); e++ ) {
// 			// if 
// 			if( entry[ e ].name_limit == name_length && lib_string_compare( (uint8_t *) entry[ e ].name, name, name_length ) ) return (struct LIB_VFS_STRUCTURE *) &entry[ e ];
// 		}
// 	}

	// not located
	return EMPTY;
}

uint64_t kernel_qfs_open( uint64_t storage_id, uint8_t *path, uint64_t length, uint8_t mode ) {
	// properties of storage
	struct KERNEL_STRUCTURE_STORAGE *storage = (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ storage_id ];

	// by default start from root directory
	uint64_t file_id = EMPTY;

	// start from directory file?
	if( *path != STD_ASCII_SLASH ) {	// yes
		// properties of task
		struct KERNEL_STRUCTURE_TASK *task = kernel_task_active();
	
		// choose task current file
		file_id = task -> directory;
	}

	// parse path
	while( length ) {
		// remove leading '/', if exist
		while( *path == '/' ) { path++; length--; }

		// select file name from path
		uint64_t name_length = lib_string_word_end( path, length, '/' );

		// locate file inside directory
		file_id = kernel_qfs_search( storage_id, file_id, path, name_length );
	}

	// return file identificator
	return file_id;
}