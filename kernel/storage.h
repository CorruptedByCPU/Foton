/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_STORAGE
	#define	KERNEL_STORAGE

	#define	KERNEL_STORAGE_limit	(STD_PAGE_byte / sizeof( struct KERNEL_STORAGE_STRUCTURE ))	// hard limit

	#define	KERNEL_STORAGE_TYPE_vfs	0b00000001

	struct KERNEL_STORAGE_STRUCTURE {
		uint8_t				device_type;
		uint64_t			device_block_count;
		uint64_t			device_block_first;
		struct STD_FILE_STRUCTURE	(*storage_file)( uint64_t storage_id, uint8_t *path, uint64_t length );
	};
#endif