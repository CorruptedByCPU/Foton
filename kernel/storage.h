/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_STORAGE
	#define	KERNEL_STORAGE

	#define	KERNEL_STORAGE_limit		(STD_PAGE_byte / sizeof( struct KERNEL_STRUCTURE_STORAGE ))

	#define	KERNEL_STORAGE_FLAGS_active	1	// 0b00000001
	#define	KERNEL_STORAGE_FLAGS_secure	128	// 0b10000000

	#define	KERNEL_STORAGE_FILESYSTEM_vfs	0x01

	struct KERNEL_STRUCTURE_STORAGE {
		uint8_t				flags;
		uint8_t				id;
		uint64_t			block;	// first
		uint64_t			limit;	// blocks
		uint64_t			byte;	// block limit
		struct KERNEL_STRUCTURE_VFS	*vfs;
	};
#endif
