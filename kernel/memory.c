/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uintptr_t kernel_memory_alloc_page( void ) {
	// alloc only 1 page
	return kernel_memory_alloc( 1 ) & ~KERNEL_PAGE_logical;	// each operation on "pages" is performed on their physical addresses
}

uintptr_t kernel_memory_alloc( uint64_t N ) {
	// initialize page ID
	uintptr_t p = EMPTY;

	// search for requested length of area
	if( ! (p = kernel_memory_acquire( kernel -> memory_base_address, N )) ) return EMPTY;

	// less available pages
	kernel -> page_available -= N;

	// convert page ID to logical address
	p = (p << STD_SHIFT_PAGE) | KERNEL_PAGE_logical;

	// we need to guarantee clean memory area before use
	kernel_page_clean( (uintptr_t) p, N );

	// convert page ID to logical address
	return (uintptr_t) p;
}

uint64_t kernel_memory_acquire( uint32_t *memory, uint64_t N ) {
	// block access to binary memory map (only one at a time)
	while( __sync_val_compare_and_swap( &kernel -> memory_semaphore, UNLOCK, LOCK ) );

	// search binary memory map for N continuous (p)ages
	for( uint64_t p = 0; p < kernel -> page_limit; p++ ) {
		// by default we found N enabled bits
		uint8_t found = TRUE;

		// check N (c)onsecutive pages
		for( uint64_t c = p; c < p + N; c++ ) {
			// broken continous?
			if( ! (memory[ c / 32 ] & 1 << c % 32) ) {
				// one of the bits is disabled
				found = FALSE;

				// start looking from next position
				p = c;

				// restart
				break;
			}
		}

		// if N consecutive pages have been found
		if( found ) {
			// mark pages as (r)eserved
			for( uint64_t r = p; r < p + N; r++ )
				memory[ r / 32 ] &= ~(1 << r % 32 );

			// unlock access to binary memory map
			kernel -> memory_semaphore = UNLOCK;

			// return address of acquired memory area
			return p;
		}
	}

	// unlock access to binary memory map
	kernel -> memory_semaphore = UNLOCK;

	// no available memory area
	return EMPTY;
}