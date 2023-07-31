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

	#define	LIB_VFS_TYPE_regular_file	0b00000001
	#define	LIB_VFS_TYPE_directory		0b00000010
	#define	LIB_VFS_TYPE_symbolic_link	0b00000100
	#define	LIB_VFS_TYPE_shared_object	0b00001000
	#define	LIB_VFS_TYPE_unknown		0b10000000

	#define	LIB_VFS_MODE_user_read		0b0000000000000001
	#define	LIB_VFS_MODE_user_write		0b0000000000000010
	#define	LIB_VFS_MODE_user_exec		0b0000000000000100
	#define	LIB_VFS_MODE_group_read		0b0000000000001000
	#define	LIB_VFS_MODE_group_write	0b0000000000010000
	#define	LIB_VFS_MODE_group_exec		0b0000000000100000
	#define	LIB_VFS_MODE_other_read		0b0000000001000000
	#define	LIB_VFS_MODE_other_write	0b0000000010000000
	#define	LIB_VFS_MODE_other_exec		0b0000000100000000

	struct LIB_VFS_STRUCTURE {
		uint64_t	offset;
		uint64_t	size;
		uint16_t	length;
		uint16_t	mode;
		uint16_t	uid;
		uint16_t	guid;
		uint8_t		type;
		uint8_t		name[ LIB_VFS_name_limit + 1 ];	// last is STATIC_ASCII_TERMINATOR
	};
#endif