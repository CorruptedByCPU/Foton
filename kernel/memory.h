/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_MEMORY
	#define	KERNEL_MEMORY

	// between LOW and HIGH there are pages intended for physical devices
	// that don't understand logical addressing
	#define	KERNEL_MEMORY_LOW	0
	#define	KERNEL_MEMORY_HIGH	256

	uintptr_t kernel_memory_alloc( uint64_t n );
	void kernel_memory_clean( uint64_t *address, uint64_t n );
#endif
