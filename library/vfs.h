/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_VFS
	#define	LIB_VFS

	#ifndef	LIB_STRING
		#include	"./string.h"
	#endif

	#define	LIB_VFS_MAGIC			0x53465623	// "#VFS"
	#define	LIB_VFS_NAME_limit		16
	#define	LIB_VFS_FILE_default		2

	#define	LIB_VFS_BLOCK_byte		4096

	struct	LIB_VFS_STRUCTURE {
		uintptr_t	offset;
		uint64_t	byte;
		uint8_t		type;
		uint8_t		name_length;
		uint8_t		name[ LIB_VFS_NAME_limit ];
	};

	// returns TRUE if at specified base address is a VFS structure
	uint8_t lib_vfs_identify( uintptr_t base_address, uint64_t byte );
#endif