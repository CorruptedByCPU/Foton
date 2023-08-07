/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_VFS
	#define	LIB_VFS

	#define	LIB_VFS_align			16
	#define	LIB_VFS_base			64
	#define	LIB_VFS_length			4
	#define	LIB_VFS_magic			0x53465623	// "#VFS"
	#define	LIB_VFS_name_limit		255
	#define	LIB_VFS_shift			6
	#define	LIB_VFS_default			2

	#define	LIB_VFS_BLOCK_size		4096

	struct LIB_VFS_STRUCTURE {
		uint64_t	offset;
		uint64_t	size;
		uint16_t	length;
		uint16_t	mode;
		uint16_t	uid;
		uint16_t	guid;
		uint8_t		type;
		uint8_t		name[ LIB_VFS_name_limit ];
	};

	// returns TRUE if at specified address is VFS structure
	uint8_t lib_vfs_check( uintptr_t address, uint64_t size_byte );
#endif