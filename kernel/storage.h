/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_STORAGE
	#define	KERNEL_STORAGE

	#define	KERNEL_STORAGE_limit		(STD_PAGE_byte / sizeof( struct KERNEL_STRUCTURE_STORAGE ))

	#define	KERNEL_STORAGE_FLAGS_active	1	// 0b00000001
	#define	KERNEL_STORAGE_FLAGS_reserved	128	// 0b10000000

	struct KERNEL_STRUCTURE_STORAGE {
		uint8_t				flags;
		uint8_t				type;
		uint8_t				id;
		uint64_t			block;	// first
		uint64_t			limit;	// blocks
		uint64_t			byte;
		void				(*read)( uint64_t id, uint64_t block, uint8_t *target, uint64_t limit );
		void				(*write)( uint64_t id, uint64_t block, uint8_t *source, uint64_t limit );
	};
#endif
