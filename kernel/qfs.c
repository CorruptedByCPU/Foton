/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_qfs_format( uint64_t storage_id ) {
	// properties of storage
	struct KERNEL_STRUCTURE_STORAGE *storage = (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ storage_id ];

	//----------------------------------------------------------------------

	// prepare superblock
	struct LIB_VFS_STRUCTURE *superblock = (struct LIB_VFS_STRUCTURE *) kernel -> memory_alloc( TRUE );

	// file system length in blocks
	superblock -> limit = MACRO_PAGE_ALIGN_DOWN( storage -> device_byte * storage -> device_limit ) >> STD_SHIFT_PAGE;

	// bitmap length in blocks
	uint64_t bitmap_block = (superblock -> limit - (TRUE + TRUE)) / 32768;
	if( (superblock -> limit - (TRUE + TRUE)) % 32768 ) bitmap_block++;

	// first block of root directory
	superblock -> block[ FALSE ] = TRUE + bitmap_block;

	// available area
	superblock -> limit -= (TRUE + bitmap_block + TRUE);

	// type
	superblock -> type = STD_FILE_TYPE_directory;

	// name
	superblock -> name_limit = 1;
	superblock -> name[ FALSE ] = STD_ASCII_SLASH;

	// write inside first usable block of storage
	storage -> write( storage_id, storage -> device_block, (uint8_t *) superblock, LIB_VFS_BLOCK_byte / storage -> device_byte );

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
	storage -> write( storage_id, storage -> device_block + (LIB_VFS_BLOCK_byte / 512), (uint8_t *) bitmap, (bitmap_block << STD_SHIFT_PAGE) / storage -> device_byte );

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
	storage -> write( storage_id, storage -> device_block + ((TRUE + bitmap_block) * 8), (uint8_t *) root, LIB_VFS_BLOCK_byte / storage -> device_byte );

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
	storage -> read( storage -> device_id, storage -> device_block, (uint8_t *) vfs, LIB_VFS_BLOCK_byte / storage -> device_byte );

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
		storage -> read( storage -> device_id, storage -> device_block + (vfs -> block[ FALSE ] * (LIB_VFS_BLOCK_byte / storage -> device_byte)), (uint8_t *) vfs, LIB_VFS_BLOCK_byte / storage -> device_byte );

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

	// no
	return qfs;
}
