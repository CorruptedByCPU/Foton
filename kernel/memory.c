/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint64_t kernel_memory_acquire( uint32_t *memory, uint64_t n, uint64_t p, uint64_t l ) {
	// define memory lock location
	uint8_t *lock = (uint8_t *) memory + MACRO_PAGE_ALIGN_UP( (kernel -> page_limit >> STD_SHIFT_8) + TRUE ) - STD_SIZE_BYTE_byte;

	// lock access to binary memory map (only one process at a time)
	MACRO_LOCK( *lock );

	// search binary memory map for N continuous (p)ages
	for( ; (p + n) < l; p++ ) {
		// by default we found N enabled bits
		uint8_t found = TRUE;

		// check for N (c)onsecutive pages
		for( uint64_t c = p; c < (p + n); c++ ) {
			// continous?
			if( memory[ c >> STD_SHIFT_32 ] & 1 << (c & 0b00011111) ) continue;

			// one of the bits is disabled
			found = FALSE;

			// start looking from next position
			p = c; break;
		}

		// if N consecutive pages have been found
		if( ! found ) continue;	// nope

		// mark pages as (r)eserved
		for( uint64_t r = p; r < (p + n); r++ ) memory[ r >> STD_SHIFT_32 ] &= ~(1 << (r & 0b00011111) );

		// unlock access to binary memory map
		MACRO_UNLOCK( *lock );

		// return address of acquired memory area
		return p;
	}

	// unlock access to binary memory map
	MACRO_UNLOCK( *lock );

	// no available memory area
	return EMPTY;
}

uintptr_t kernel_memory_alloc( uint64_t n ) {
	// initialize page ID
	uintptr_t p = EMPTY;

	// search for requested length of area
	if( ! (p = kernel_memory_acquire( kernel -> memory_base_address, n, KERNEL_MEMORY_LOW, kernel -> page_limit )) ) return EMPTY;

	// less memory available
	kernel -> page_available -= n;

	// we guarantee clean memory area at first use
	kernel_memory_clean( (uint64_t *) ((p << STD_SHIFT_PAGE) | KERNEL_MEMORY_mirror), n );

	// convert page ID to logical address and return
	return (uintptr_t) (p << STD_SHIFT_PAGE) | KERNEL_MEMORY_mirror;
}

void kernel_memory_clean( uint64_t *address, uint64_t n ) { for( uint64_t i = 0; i < n << STD_SHIFT_512; i++ ) address[ i ] = EMPTY; }