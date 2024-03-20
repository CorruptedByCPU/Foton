/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_STORAGE
	#define	KERNEL_STORAGE

	// ignore more storages than hard limit
	#define	KERNEL_STORAGE_limit		(STD_PAGE_byte / sizeof( struct KERNEL_STORAGE_STRUCTURE ))

	// storage type:
	#define	KERNEL_STORAGE_TYPE_vfs		0b00000001	// Virtual File System

	// storage class:
	#define	KERNEL_STORAGE_CLASS_block		0b00000001	// whole storage
	#define	KERNEL_STORAGE_CLASS_partition	0b00000010	// part of storage

	struct KERNEL_STORAGE_STRUCTURE {
		uint8_t		device_type;
		uint8_t		device_class;
		uint64_t	device_block;	// first usable block of storage
		uint64_t	device_byte;	// size of single BLOCK in Bytes;
		uint64_t	device_length;	// size of storage CLASS in blocks
	};
#endif