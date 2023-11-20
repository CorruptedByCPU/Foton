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
			if( ! (memory[ c >> STD_SHIFT_32 ] & 1 << (c & 0b0011111)) ) {
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
				memory[ r >> STD_SHIFT_32 ] &= ~(1 << (r & 0b0011111) );

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

uint64_t kernel_memory_acquire_secured( struct KERNEL_TASK_STRUCTURE *task, uint64_t N ) {
	// block access to binary memory map (only one at a time)
	while( __sync_val_compare_and_swap( &task -> memory_semaphore, UNLOCK, LOCK ) );

	// search binary memory map for N continuous (p)ages
	for( uint64_t p = 0; p < kernel -> page_limit; p++ ) {
		// by default we found N enabled bits
		uint8_t found = TRUE;

		// check N (c)onsecutive pages
		for( uint64_t c = p; c < p + N; c++ ) {
			// broken continous?
			if( ! (task -> memory_map[ c >> STD_SHIFT_32 ] & 1 << (c & 0b0011111)) ) {
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
				task -> memory_map[ r >> STD_SHIFT_32 ] &= ~(1 << (r & 0b0011111) );

			// unlock access to binary memory map
			task -> memory_semaphore = UNLOCK;

			// return address of acquired memory area
			return p;
		}
	}

	// unlock access to binary memory map
	task -> memory_semaphore = UNLOCK;

	// no available memory area
	return EMPTY;
}

void kernel_memory_dispose( uint32_t *memory_map, uint64_t p, uint64_t N ) {
	// mark pages as available
	for( uint64_t i = p; i < p + N; i++ )
		memory_map[ i >> STD_SHIFT_32 ] |= 1 << (i & 0b0011111);
}

void kernel_memory_release( uintptr_t address, uint64_t N ) {
	// release occupied pages inside kernels binary memory map
	kernel_memory_dispose( kernel -> memory_base_address, (address & ~KERNEL_PAGE_logical) >> STD_SHIFT_PAGE, N );

	// more available pages
	kernel -> page_available += N;
}

void kernel_memory_release_page( uintptr_t address ) {
	// release page from binary memory map
	kernel_memory_release( address | KERNEL_PAGE_logical, 1 );
}

uintptr_t kernel_memory_share( uintptr_t address, uint64_t page ) {
	// task properties
	struct KERNEL_TASK_STRUCTURE *task = (struct KERNEL_TASK_STRUCTURE *) kernel_task_active();

	// acquire N continuous pages
	uintptr_t allocated = EMPTY;
	if( (allocated = kernel_memory_acquire_secured( task, page )) ) {
		// map memory area to process
		kernel_page_map( (uintptr_t *) task -> cr3, address, (uintptr_t) KERNEL_EXEC_base_address + (allocated << STD_SHIFT_PAGE), page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | KERNEL_PAGE_FLAG_shared );

		// return the address of the first page in the collection
		return KERNEL_EXEC_base_address + (allocated << STD_SHIFT_PAGE);
	}

	// no free space
	return EMPTY;
}