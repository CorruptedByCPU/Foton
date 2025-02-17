/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_bfs_format( uint64_t storage_id ) {
	// properties of storage
	struct KERNEL_STRUCTURE_STORAGE *storage = (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ storage_id ];

	//----------------------------------------------------------------------

	// prepare superblock
	struct LIB_BFS_STRUCTURE_INODE *superblock = (struct LIB_BFS_STRUCTURE_INODE *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct LIB_BFS_STRUCTURE_INODE ) ) >> STD_SHIFT_PAGE );

	// file system length in blocks
	superblock -> limit = MACRO_PAGE_ALIGN_DOWN( storage -> device_byte * storage -> device_limit ) >> STD_SHIFT_PAGE;

	// bitmap length in blocks
	uint64_t bitmap_block = (superblock -> limit - LIB_BFS_SUPERBLOCK_block - LIB_BFS_ROOT_DIRECTORY_block) / (LIB_BFS_BLOCK_byte * STD_SIZE_BYTE_bit);
	if( (superblock -> limit - LIB_BFS_SUPERBLOCK_block - LIB_BFS_ROOT_DIRECTORY_block) % (LIB_BFS_BLOCK_byte * STD_SIZE_BYTE_bit) ) bitmap_block++;

	// first block of root directory
	superblock -> block[ FALSE ] = LIB_BFS_SUPERBLOCK_block + bitmap_block;

	// available area
	superblock -> limit -= (LIB_BFS_SUPERBLOCK_block + bitmap_block + LIB_BFS_ROOT_DIRECTORY_block);

	// type
	superblock -> type = STD_FILE_TYPE_directory;

	// name
	superblock -> name_limit = 1;
	superblock -> name[ FALSE ] = STD_ASCII_SLASH;

	// write inside first usable block of storage
	storage -> write( storage_id, storage -> device_block, (uint8_t *) superblock, LIB_BFS_SUPERBLOCK_byte / storage -> device_byte );

	// release superblock content
	kernel -> memory_release( (uintptr_t) superblock, MACRO_PAGE_ALIGN_UP( sizeof( struct LIB_BFS_STRUCTURE_INODE ) ) >> STD_SHIFT_PAGE );

	//----------------------------------------------------------------------

	// prepare bitmap
	uint32_t *bitmap = (uint32_t *) kernel -> memory_alloc( bitmap_block );

	// mark pages used by file system structures as unusable
	for( uint64_t i = 0; i < (LIB_BFS_SUPERBLOCK_block + bitmap_block + LIB_BFS_ROOT_DIRECTORY_block); i++ )
		bitmap[ i >> STD_SHIFT_32 ] |= 1 << (i & 0b00011111);

	// write after superblock
	storage -> write( storage_id, storage -> device_block + (LIB_BFS_SUPERBLOCK_byte / 512), (uint8_t *) bitmap, (bitmap_block << STD_SHIFT_PAGE) / storage -> device_byte );

	// release bitmap content
	kernel -> memory_release( (uintptr_t) bitmap, bitmap_block );

	//----------------------------------------------------------------------

	// prepare root directory
	struct LIB_BFS_STRUCTURE_INODE *root = (struct LIB_BFS_STRUCTURE_INODE *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct LIB_BFS_STRUCTURE_INODE ) ) >> STD_SHIFT_PAGE );

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
	storage -> write( storage_id, storage -> device_block + (LIB_BFS_SUPERBLOCK_block * 8) + (bitmap_block * 8), (uint8_t *) root, LIB_BFS_ROOT_DIRECTORY_byte / storage -> device_byte );

	// release root directory content
	kernel -> memory_release( (uintptr_t) root, MACRO_PAGE_ALIGN_UP( sizeof( struct LIB_BFS_STRUCTURE_INODE ) ) >> STD_SHIFT_PAGE );
}