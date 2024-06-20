/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint64_t kernel_memory_acquire( uint32_t *memory_map, uint64_t N ) {
	// define memory semaphore location
	uint8_t *semaphore = (uint8_t *) memory_map + MACRO_PAGE_ALIGN_UP( kernel -> page_limit >> STD_SHIFT_8 ) - STD_SIZE_BYTE_byte;

	// block access to binary memory map (only one at a time)
	MACRO_LOCK( *semaphore );

	// search binary memory map for N continuous (p)ages
	for( uint64_t p = 0; p < kernel -> page_limit; p++ ) {
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
	if( ! (p = kernel_memory_acquire( kernel -> memory_base_address, N )) ) return p;

	// less available pages
	kernel -> page_available -= N;

	// convert page ID to logical address and return
	return (uintptr_t) (p << STD_SHIFT_PAGE) | KERNEL_PAGE_logical;
}

uintptr_t kernel_memory_alloc_page( void ) {
	// acquire single physical page
	uintptr_t page = kernel_memory_alloc( TRUE ) & ~KERNEL_PAGE_logical;

	// page used for structure
	kernel -> page_structure++;

	// return physical address
	return page;
}

void kernel_memory_clean( uint64_t *address, uint64_t n ) {
	// clear every 64 bit value inside N pages
	for( uint64_t i = 0; i < n << STD_SHIFT_512; i++ ) address[ i ] = EMPTY;
}

void kernel_memory_dispose( uint32_t *memory_map, uint64_t p, uint64_t N ) {
	// mark pages as available
	for( uint64_t i = p; i < p + N; i++ )
		memory_map[ i >> STD_SHIFT_32 ] |= 1 << (i & 0b00011111);
}

void kernel_memory_release( uintptr_t address, uint64_t N ) {
	// we need to guarantee clean memory inside binary memory map
	kernel_memory_clean( (uint64_t *) address, N );

	// release occupied pages inside kernels binary memory map
	kernel_memory_dispose( kernel -> memory_base_address, (address & ~KERNEL_PAGE_logical) >> STD_SHIFT_PAGE, N );

	// more available pages
	kernel -> page_available += N;
}

void kernel_memory_release_page( uintptr_t page ) {
	// release single physical page
	kernel_memory_release( page | KERNEL_PAGE_logical, TRUE );

	// page released from structure
	kernel -> page_structure--;
}

uintptr_t kernel_memory_share( uintptr_t address, uint64_t page ) {
	// task properties
	struct KERNEL_TASK_STRUCTURE *task = (struct KERNEL_TASK_STRUCTURE *) kernel_task_active();

	// acquire N continuous pages
	uintptr_t allocated = EMPTY;
	if( (allocated = kernel_memory_acquire( task -> memory_map, page )) ) {
		// map memory area to process
		kernel_page_map( (uintptr_t *) task -> cr3, address, (uintptr_t) (allocated << STD_SHIFT_PAGE), page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | KERNEL_PAGE_FLAG_shared );

		// shared pages
		kernel -> page_shared += page;

		// return the address of the first page in the collection
		return (allocated << STD_SHIFT_PAGE);
	}

	// no free space
	return EMPTY;
}