/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_VFS
	#define	LIB_VFS

	#ifndef	LIB_STRING
		#include	"./string.h"
	#endif

	#define	NEW_LIB_VFS_magic		0x53465623	// "#VFS"
	#define	NEW_LIB_VFS_block		STD_PAGE_byte

	#define	EXCHANGE_LIB_VFS_NAME_limit	255

	struct EXCHANGE_LIB_VFS_STRUCTURE {
		uintptr_t	offset;
		uint64_t	byte;
		uint8_t		type;
		uint8_t		name_length;
		uint8_t		name[ EXCHANGE_LIB_VFS_NAME_limit ];
	};

// OLD ========================================================================

	#define	DEPRECATED_LIB_VFS_length			4
	#define	DEPRECATED_LIB_VFS_magic			0x53465623	// "#VFS"
	#define	DEPRECATED_LIB_VFS_default			2
#endif