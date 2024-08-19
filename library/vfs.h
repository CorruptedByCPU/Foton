/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_VFS
	#define	LIB_VFS

	#ifndef	LIB_STRING
		#include	"./string.h"
	#endif

	#define	LIB_VFS_length		4		// length of magic in Bytes
	#define	LIB_VFS_magic		"#VFS"
	#define	LIB_VFS_block		STD_PAGE_byte
	#define	LIB_VFS_default		2		// default number of files in new directory (2 links)

	#define	LIB_VFS_NAME_limit	(254 - 17)

	struct LIB_VFS_STRUCTURE {
		uintptr_t	offset;
		uint64_t	byte;
		uint8_t		type;
		uint8_t		name_length;
		uint8_t		name[ LIB_VFS_NAME_limit + 1 ];
	} __attribute__( (packed) );
#endif