/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	#define	LIB_BFS_BLOCK_byte		STD_PAGE_byte

	// prepare superblock
	struct LIB_BFS_STRUCTURE_INODE {
		uint64_t	block[ 5 ];
		uint64_t	limit;
		uint8_t		reserved[ 206 ];
		uint8_t		type;
		uint8_t		name_limit;
		#define		NAME_LIMIT	255
		uint8_t		name[ NAME_LIMIT + 1 ];
	} __attribute( (packed) );

	// length of objects in file system blocks
	#define	LIB_BFS_SUPERBLOCK_block	(MACRO_PAGE_ALIGN_UP( sizeof( struct LIB_BFS_STRUCTURE_INODE ) ) >> STD_SHIFT_PAGE)
	#define	LIB_BFS_ROOT_DIRECTORY_block	TRUE