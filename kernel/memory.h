/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_MEMORY
	#define	KERNEL_MEMORY

	#define	KERNEL_MEMORY_LOW	256

	// returns number of first page with a continuum of N consecutive allocated pages
	uint64_t kernel_memory_acquire( uint32_t *memory_map, uint64_t N, uint64_t p, uint64_t limit );

	// returns logical address of an N-page area
	uintptr_t kernel_memory_alloc( uint64_t pages );

	// return logical addres of an N-page area from low memory region
	uintptr_t kernel_memory_alloc_low( uint64_t N );

	// returns physical address of allocated page
	uintptr_t kernel_memory_alloc_page( void );

	// fills up memory with EMPTY value
	void kernel_memory_clean( uintptr_t *address, uint64_t n );
#endif