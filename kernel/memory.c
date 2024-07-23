/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint64_t kernel_memory_acquire( uint32_t *memory_map, uint64_t N, uint64_t p, uint64_t limit ) {
	// define memory semaphore location
	uint8_t *semaphore = (uint8_t *) memory_map + MACRO_PAGE_ALIGN_UP( (kernel -> page_limit >> STD_SHIFT_8) + TRUE ) - STD_SIZE_BYTE_byte;

	// block access to binary memory map (only one at a time)
	MACRO_LOCK( *semaphore );

	// search binary memory map for N continuous (p)ages
	for( ; p < limit; p++ ) {
		// by default we found N enabled bits
		uint8_t found = TRUE;

		// check N (c)onsecutive pages
		for( uint64_t c = p; c < p + N; c++ ) {
			// broken continous?
			if( ! (memory_map[ c >> STD_SHIFT_32 ] & 1 << (c & 0b00011111)) ) {
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
				memory_map[ r >> STD_SHIFT_32 ] &= ~(1 << (r & 0b00011111) );

			// unlock access to binary memory map
			MACRO_UNLOCK( *semaphore );

			// return address of acquired memory area
			return p;
		}
	}

	// unlock access to binary memory map
	MACRO_UNLOCK( *semaphore );

	// no available memory area
	return EMPTY;
}

uintptr_t kernel_memory_alloc( uint64_t N ) {
	// initialize page ID
	uintptr_t p = EMPTY;

	// search for requested length of area
	if( ! (p = kernel_memory_acquire( kernel -> memory_base_address, N, KERNEL_MEMORY_LOW, kernel -> page_limit )) ) return EMPTY;

	// less available pages
	kernel -> page_available -= N;

	// we guarantee clean memory area at first use
	kernel_memory_clean( (uint64_t *) ((p << STD_SHIFT_PAGE) | KERNEL_PAGE_mirror), N );

	// convert page ID to logical address and return
	return (uintptr_t) (p << STD_SHIFT_PAGE) | KERNEL_PAGE_mirror;
}

uintptr_t kernel_memory_alloc_low( uint64_t N ) {
	// initialize value
	uintptr_t p = EMPTY;

	// search for requested length of area
	if( ! (p = kernel_memory_acquire( kernel -> memory_base_address, N, 0, KERNEL_MEMORY_LOW )) ) return EMPTY;

	// less available pages
	kernel -> page_available -= N;

	// we guarantee clean memory area at first use
	kernel_memory_clean( (uint64_t *) ((p << STD_SHIFT_PAGE) | KERNEL_PAGE_mirror), N );

	// convert page ID to logical address and return
	return (uintptr_t) (p << STD_SHIFT_PAGE) | KERNEL_PAGE_mirror;
}

uintptr_t kernel_memory_alloc_page( void ) {
	// acquire single physical page
	uintptr_t page = kernel_memory_alloc( TRUE ) & ~KERNEL_PAGE_mirror;

	// page used for structure
	kernel -> page_structure++;

	// return physical address
	return page;
}

void kernel_memory_clean( uintptr_t *address, uint64_t n ) {
	// clear every 64 bit value inside N pages
	for( uint64_t i = 0; i < n << STD_SHIFT_512; i++ ) address[ i ] = EMPTY;
}

void kernel_memory_dispose( uint32_t *memory_map, uint64_t p, uint64_t N ) {
	// mark pages as available
	for( uint64_t i = p; i < (p + N); i++ ) __sync_or_and_fetch( &memory_map[ i >> STD_SHIFT_32 ], 1 << (i & 0b00011111) );
}

void kernel_memory_release( uintptr_t address, uint64_t N ) {
	// release occupied pages inside kernels binary memory map
	kernel_memory_dispose( kernel -> memory_base_address, (address & ~KERNEL_PAGE_mirror) >> STD_SHIFT_PAGE, N );

	// more available pages
	kernel -> page_available += N;
}

void kernel_memory_release_page( uintptr_t page ) {
	// release single physical page
	kernel_memory_release( page | KERNEL_PAGE_mirror, TRUE );

	// page released from structure
	kernel -> page_structure--;
}

uintptr_t kernel_memory_share( uintptr_t address, uint64_t page ) {
	// task properties
	struct KERNEL_TASK_STRUCTURE *task = (struct KERNEL_TASK_STRUCTURE *) kernel_task_active();

	// acquire N continuous pages
	uintptr_t allocated = EMPTY;
	if( (allocated = kernel_memory_acquire( task -> memory_map, page, KERNEL_MEMORY_LOW, kernel -> page_limit )) ) {
		// map memory area to process
		kernel_page_map( (uint64_t *) task -> cr3, address, (uintptr_t) (allocated << STD_SHIFT_PAGE), page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | (KERNEL_PAGE_TYPE_SHARED << KERNEL_PAGE_TYPE_offset) );

		// shared pages
		kernel -> page_shared += page;

		// return the address of the first page in the collection
		return (allocated << STD_SHIFT_PAGE);
	}

	// no free space
	return EMPTY;
}