/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_STORAGE
	#define	KERNEL_STORAGE

	// ignore more storages than hard limit
	#define	KERNEL_STORAGE_limit		(STD_PAGE_byte / sizeof( struct KERNEL_STRUCTURE_STORAGE ))

	// storage type:
	#define	KERNEL_STORAGE_FS_vfs		0x01	// Virtual File System

	// storage class:
	#define	KERNEL_STORAGE_CLASS_block	0b00000001	// whole storage

	struct KERNEL_STRUCTURE_STORAGE {
		uint8_t				device_class;
		uint8_t				device_fs;
		uint8_t				device_id;
		uint64_t			device_block;	// first usable block of storage
		uint64_t			device_limit;	// size of storage CLASS in blocks
		uint64_t			device_byte;	// size of single BLOCK in Bytes
		void				(*read)( uint64_t id, uint64_t block, uint8_t *target, uint64_t length );
		void				(*write)( uint64_t id, uint64_t block, uint8_t *source, uint64_t length );
	};
#endif
