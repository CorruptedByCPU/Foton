/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_bfs_format( uint64_t storage_id ) {
	// properties of storage
	struct KERNEL_STRUCTURE_STORAGE *storage = (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ storage_id ];

	// prepare bitmap
	struct LIB_BFS_STRUCTURE_INODE *superblock = (struct LIB_BFS_STRUCTURE_INODE *) kernel -> memory_alloc( TRUE );

	// file system length in blocks
	superblock -> limit = MACRO_PAGE_ALIGN_DOWN( storage -> device_byte * storage -> device_limit ) >> STD_SHIFT_PAGE;

	// bitmap length in blocks
	uint64_t bitmap_block = (superblock -> limit - LIB_BFS_SUPERBLOCK_block - LIB_BFS_ROOT_DIRECTORY_block) / (LIB_BFS_BLOCK_byte * STD_SIZE_BYTE_bit);
	if( (superblock -> limit - LIB_BFS_SUPERBLOCK_block - LIB_BFS_ROOT_DIRECTORY_block) % (LIB_BFS_BLOCK_byte * STD_SIZE_BYTE_bit) ) bitmap_block++;

	// first block of root directory
	superblock -> block[ FALSE ] = LIB_BFS_SUPERBLOCK_block + bitmap_block;

	// available area
	superblock -> limit -= (LIB_BFS_SUPERBLOCK_block + LIB_BFS_ROOT_DIRECTORY_block + bitmap_block);

	// type
	superblock -> type = STD_FILE_TYPE_directory;

	// name
	superblock -> name_limit = 1;
	superblock -> name[ FALSE ] = STD_ASCII_SLASH;

	storage -> write( storage_id, 0, (uint8_t *) superblock, 1 );
}