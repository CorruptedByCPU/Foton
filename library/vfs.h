/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_VFS
	#define	LIB_VFS

	#ifndef	LIB_STRING
		#include	"./string.h"
	#endif

	#define	NEW_LIB_VFS_magic	0x53465623	// "#VFS"
	#define	EXCHANGE_LIB_VFS_name_limit		255

	struct EXCHANGE_LIB_VFS_STRUCTURE {
		uintptr_t	offset;
		uint64_t	byte;
		uint8_t		type;
		uint8_t		name_length;
		uint8_t		name[ EXCHANGE_LIB_VFS_name_limit ];
		uint16_t	DEPRECATED_mode;
		uint16_t	DEPRECATED_uid;
		uint16_t	DEPRECATED_guid;
	};

// OLD ========================================================================

	#define	DEPRECATED_LIB_VFS_align			16
	#define	DEPRECATED_LIB_VFS_base			64
	#define	DEPRECATED_LIB_VFS_length			4
	#define	DEPRECATED_LIB_VFS_magic			0x53465623	// "#VFS"
	#define	DEPRECATED_LIB_VFS_shift			6
	#define	DEPRECATED_LIB_VFS_default			2

	#define	DEPRECATED_LIB_VFS_BLOCK_size		4096

	// returns TRUE if at specified address is VFS structure
	uint8_t DEPRECATED_lib_vfs_check( uintptr_t address, uint64_t size_byte );
#endif