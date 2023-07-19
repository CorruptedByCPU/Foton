/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_MEMORY
	#define	KERNEL_MEMORY

	// returns number of first page with a continuum of N consecutive allocated pages
	uint64_t kernel_memory_acquire( uint32_t *memory, uint64_t N );

	// returns logical address of an N-page area
	uintptr_t kernel_memory_alloc( uint64_t pages );

	// returns physical address of allocated page
	uintptr_t kernel_memory_alloc_page( void );
#endif