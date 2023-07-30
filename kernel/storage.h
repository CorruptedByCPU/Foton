/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_STORAGE
	#define	KERNEL_STORAGE

	#define	KERNEL_STORAGE_limit		(STD_PAGE_byte / sizeof( struct KERNEL_STORAGE_STRUCTURE ))	// hard limit
	#define	KERNEL_STORAGE_TYPE_memory	0b00000001

	struct KERNEL_STORAGE_STRUCTURE {
		uint8_t					device_type;
		uint64_t				device_blocks;
	};
#endif
