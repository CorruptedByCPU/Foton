/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_STORAGE
	#define	KERNEL_STORAGE

	#define	KERNEL_STORAGE_limit		(STD_PAGE_byte / sizeof( struct KERNEL_STRUCTURE_STORAGE ))

	#define	KERNEL_STORAGE_FLAGS_active	1	// 0b00000001
	#define	KERNEL_STORAGE_FLAGS_secure	128	// 0b10000000

	#define	KERNEL_STORAGE_NAME_limit	31

	struct KERNEL_STRUCTURE_STORAGE {
		uint8_t				flags;
		uint8_t				id;
		uint64_t			block;	// first
		uint64_t			limit;	// blocks
		uint64_t			byte;	// block limit
		uint8_t				name_limit;
		uint8_t				name[ KERNEL_STORAGE_NAME_limit + TRUE ];
		struct KERNEL_STRUCTURE_VFS_USE	*use;
	};
#endif
