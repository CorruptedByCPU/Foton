/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_fs_format( uint64_t storage_id ) {
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

	// mark pages used by file system structures as unusable
	for( uint64_t i = 0; i < (TRUE + bitmap_block + TRUE); i++ )
		bitmap[ i >> STD_SHIFT_32 ] |= 1 << (i & 0b00011111);

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