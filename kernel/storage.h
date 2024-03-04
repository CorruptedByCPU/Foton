/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_STORAGE
	#define	KERNEL_STORAGE

	#define	KERNEL_STORAGE_limit		(STD_PAGE_byte / sizeof( struct KERNEL_STORAGE_STRUCTURE ))	// hard limit
	#define	KERNEL_STORAGE_NODE_limit	(STD_PAGE_byte / sizeof( struct KERNEL_STORAGE_STRUCTURE_NODE ))	// hard limit

	#define	KERNEL_STORAGE_TYPE_vfs		1
	#define	KERNEL_STORAGE_TYPE_nvme	2
	#define	KERNEL_STORAGE_TYPE_usb		3
	#define	KERNEL_STORAGE_TYPE_ahci	4

	struct KERNEL_STORAGE_STRUCTURE {
		uint8_t		device_type;
		uint64_t	device_id;
		uint64_t	device_block;
		uint64_t	device_size_byte;
	};

	struct KERNEL_STORAGE_STRUCTURE_NODE {
		uint64_t	storage;
		uint64_t	node;
		int64_t		pid;
	};
#endif