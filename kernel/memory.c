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
			if( memory[ c >> STD_SHIFT_32 ] & 1 << (c & STD_BIT_CONTROL_DWORD_bit) ) continue;

			// one of the bits is disabled
			found = FALSE;

			// start looking from next position
			p = c; break;
		}

		// if N consecutive pages have been found
		if( ! found ) continue;	// nope

		// mark pages as (r)eserved
		for( uint64_t r = p; r < (p + n); r++ ) memory[ r >> STD_SHIFT_32 ] &= ~(1 << (r & STD_BIT_CONTROL_DWORD_bit) );

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
	uintptr_t p = 0;

	// search for requested length of area
	if( ! (p = kernel_memory_acquire( kernel -> memory_base_address, n, KERNEL_MEMORY_HIGH, kernel -> page_limit )) ) return EMPTY;

	// less memory available
	kernel -> page_available -= n;

	// we guarantee clean memory area at first use
	kernel_memory_clean( (uint64_t *) ((p << STD_SHIFT_PAGE) | KERNEL_MEMORY_mirror), n );

	// convert page ID to logical address and return
	return (uintptr_t) (p << STD_SHIFT_PAGE) | KERNEL_MEMORY_mirror;
}

uintptr_t kernel_memory_alloc_low( uint64_t n ) {
	// initialize value
	uintptr_t p = 0;

	// search for requested length of area
	if( ! (p = kernel_memory_acquire( kernel -> memory_base_address, n, KERNEL_MEMORY_LOW, KERNEL_MEMORY_HIGH )) ) return EMPTY;

	// less available pages
	kernel -> page_available -= n;

	// we guarantee clean memory area at first use
	kernel_memory_clean( (uint64_t *) ((p << STD_SHIFT_PAGE) | KERNEL_MEMORY_mirror), n );

	// convert page ID to logical address and return
	return (uintptr_t) (p << STD_SHIFT_PAGE) | KERNEL_MEMORY_mirror;
}

uintptr_t kernel_memory_alloc_page( void ) {
	// acquire single physical page
	uintptr_t page = kernel_memory_alloc( TRUE ) & ~KERNEL_MEMORY_mirror;

	// page intended for structures
	kernel -> page_structure++;

	// return physical address
	return page;
}

void kernel_memory_clean( uint64_t *address, uint64_t n ) { for( uint64_t i = 0; i < n << STD_SHIFT_512; i++ ) address[ i ] = EMPTY; }

void kernel_memory_dispose( uint32_t *memory, uint64_t p, uint64_t n ) {
	// mark pages as available
	for( uint64_t i = p; i < (p + n); i++ ) __sync_or_and_fetch( &memory[ i >> STD_SHIFT_32 ], 1 << (i & STD_BIT_CONTROL_DWORD_bit) );
}

void kernel_memory_release( uintptr_t address, uint64_t n ) {
	// release occupied pages inside kernels binary memory map
	kernel_memory_dispose( kernel -> memory_base_address, (address & ~KERNEL_MEMORY_mirror) >> STD_SHIFT_PAGE, n );

	// more available pages
	kernel -> page_available += n;
}

void kernel_memory_release_page( uintptr_t address ) {
	// release single physical page
	kernel_memory_release( address | KERNEL_MEMORY_mirror, TRUE );

	// page released from structure
	kernel -> page_structure--;
}
