/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_STORAGE
	#define	KERNEL_STORAGE

	// ignore more storages than hard limit
	#define	NEW_KERNEL_STORAGE_limit		(STD_PAGE_byte / sizeof( struct NEW_KERNEL_STORAGE_STRUCTURE ))

	// storage type:
	#define	NEW_KERNEL_STORAGE_TYPE_vfs		0b00000001	// Virtual File System

	// storage class:
	#define	NEW_KERNEL_STORAGE_CLASS_block		0b00000001	// whole storage
	#define	NEW_KERNEL_STORAGE_CLASS_partition	0b00000010	// part of storage

	struct NEW_KERNEL_STORAGE_STRUCTURE {
		uint8_t		device_type;
		uint8_t		device_class;
		uintptr_t	device_block;	// first usable block of storage
		uint64_t	device_byte;	// size of single BLOCK in Bytes;
		uint64_t	device_length;	// size of storage CLASS in blocks
	};

// OLD ========================================================================

	#define	DEPRECATED_KERNEL_STORAGE_limit		(STD_PAGE_byte / sizeof( struct DEPRECATED_KERNEL_STORAGE_STRUCTURE ))	// hard limit
	#define	DEPRECATED_KERNEL_STORAGE_NODE_limit	(STD_PAGE_byte / sizeof( struct DEPRECATED_KERNEL_STORAGE_STRUCTURE_NODE ))	// hard limit

	#define	DEPRECATED_KERNEL_STORAGE_TYPE_vfs		1
	#define	DEPRECATED_KERNEL_STORAGE_TYPE_nvme	2
	#define	DEPRECATED_KERNEL_STORAGE_TYPE_usb		3
	#define	DEPRECATED_KERNEL_STORAGE_TYPE_ahci	4

	struct DEPRECATED_KERNEL_STORAGE_STRUCTURE {
		uint8_t		device_type;
		uint64_t	device_id;
		uint64_t	device_block;
		uint64_t	device_size_byte;
	};

	struct DEPRECATED_KERNEL_STORAGE_STRUCTURE_NODE {
		uint64_t	storage;
		uint64_t	node;
		int64_t		pid;
	};
#endif