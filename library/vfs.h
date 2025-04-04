/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_VFS
	#define	LIB_VFS

	#define	LIB_VFS_length			4		// length of magic in Bytes
	#define	LIB_VFS_magic			0x53465623	// "#VFS"
	#define	LIB_VFS_BLOCK_byte		STD_PAGE_byte
	#define	LIB_VFS_default			2		// default number of files in new directory (previous directory symlink)

	struct LIB_VFS_STRUCTURE {
		uintptr_t			block[ 5 ];
		uint64_t			limit;
		uint8_t				reserved[ 206 ];
		uint8_t				type;
		uint8_t				name_limit;
		#define	LIB_VFS_NAME_limit	255
		uint8_t				name[ LIB_VFS_NAME_limit + 1 ];
	} __attribute__( (packed) );
#endif
