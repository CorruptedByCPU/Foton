/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t kernel_page_alloc( uint64_t *pml4, uintptr_t target, uint64_t n, uint16_t flags ) {
	// start with following array[ entries ]
	uint16_t p1 = (target >> KERNEL_PAGE_PML1_shift) & (KERNEL_PAGE_PMLx_entry - 1);
	uint16_t p2 = (target >> KERNEL_PAGE_PML2_shift) & (KERNEL_PAGE_PMLx_entry - 1);
	uint16_t p3 = (target >> KERNEL_PAGE_PML3_shift) & (KERNEL_PAGE_PMLx_entry - 1);
	uint16_t p4 = (target >> KERNEL_PAGE_PML4_shift) & (KERNEL_PAGE_PMLx_entry - 1); 

	// start with an entry representing given target in PML4 array
	for( ; p4 < KERNEL_PAGE_PMLx_entry; p4++ ) {
		// PML3 array doesn't exist?
		if( ! pml4[ p4 ] ) {
			// register new PML3 array inside PML4 entry
			pml4[ p4 ] = kernel_memory_alloc( TRUE ) & ~KERNEL_MEMORY_mirror;

			// failed to allocate page for PML3 array?
			if( ! pml4[ p4 ] ) return FALSE;

			// set flags for PML4 entry
			pml4[ p4 ] |= flags;
		}

		// set PML3 array pointer (remove flags)
		uint64_t *pml3 = (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml4[ p4 ] ) | KERNEL_MEMORY_mirror);

		// start with an entry representing given target in PML3 array
		for( ; p3 < KERNEL_PAGE_PMLx_entry; p3++ ) {
			// PML2 array doesn't exist?
			if( ! pml3[ p3 ] ) {
				// register new PML2 array inside PML3 entry
				pml3[ p3 ] = kernel_memory_alloc( TRUE ) & ~KERNEL_MEMORY_mirror;

				// if failed to allocate page for PML2 array
				if( ! pml3[ p3 ] ) return FALSE;

				// set flags for PML3 entry
				pml3[ p3 ] |= flags;
			}

			// get PML2 array target (remove flags)
			uint64_t *pml2 = (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml3[ p3 ] ) | KERNEL_MEMORY_mirror);

			// start with an entry representing given target in PML2 array
			for( ; p2 < KERNEL_PAGE_PMLx_entry; p2++ ) {
				// PML1 array doesn't exist?
				if( ! pml2[ p2 ] ) {
					// register new PML1 array inside PML2 entry
					pml2[ p2 ] = kernel_memory_alloc( TRUE ) & ~KERNEL_MEMORY_mirror;

					// if failed to allocate page for PML1 array
					if( ! pml2[ p2 ] ) return FALSE;

					// set flags for PML2 entry
					pml2[ p2 ] |= flags;
				}

				// get PML1 array target (remove flags)
				uint64_t *pml1 = (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml2[ p2 ] ) | KERNEL_MEMORY_mirror);

				// start with an entry representing given target in PML1 array
				for( ; p1 < KERNEL_PAGE_PMLx_entry; p1++ ) {
					// entry already exist?
					if( pml1[ p1 ] ) return FALSE;	// something is wrong...

					// allocate page to a logical target
					pml1[ p1 ] = kernel_memory_alloc( TRUE ) & ~KERNEL_MEMORY_mirror;

					// if failed to allocate page for logical target
					if( ! pml1[ p1 ] ) return FALSE;

					// set flags for PML1 entry
					pml1[ p1 ] |= flags;

					// if all area is described
					if( ! --n ) return TRUE;
				}

				// first entry of PML1 array
				p1 = 0;
			}

			// first entry of PML2 array
			p2 = 0;
		}

		// first entry of PML3 array
		p3 = 0;
	}

	// invalid area target
	return FALSE;
}

uint8_t kernel_page_map( uint64_t *pml4, uintptr_t source, uintptr_t target, uint64_t n, uint16_t flags ) {
	// start with following table[ entry ]
	uint16_t p1 = (target >> KERNEL_PAGE_PML1_shift) & (KERNEL_PAGE_PMLx_entry - 1);
	uint16_t p2 = (target >> KERNEL_PAGE_PML2_shift) & (KERNEL_PAGE_PMLx_entry - 1);
	uint16_t p3 = (target >> KERNEL_PAGE_PML3_shift) & (KERNEL_PAGE_PMLx_entry - 1);
	uint16_t p4 = (target >> KERNEL_PAGE_PML4_shift) & (KERNEL_PAGE_PMLx_entry - 1); 

	// start with an entry representing given target in PML4 array
	for( ; p4 < KERNEL_PAGE_PMLx_entry; p4++ ) {
		// PML3 array doesn't exist?
		if( ! pml4[ p4 ] ) {
			// register new PML3 array inside PML4 entry
			pml4[ p4 ] = kernel_memory_alloc( TRUE ) & ~KERNEL_MEMORY_mirror;

			// failed to allocate page for PML3 array?
			if( ! pml4[ p4 ] ) return FALSE;

			// set flags for PML4 entry
			pml4[ p4 ] |= flags;
		}

		// set PML3 array pointer (remove flags)
		uint64_t *pml3 = (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml4[ p4 ] ) | KERNEL_MEMORY_mirror);

		// start with an entry representing given target in PML3 array
		for( ; p3 < KERNEL_PAGE_PMLx_entry; p3++ ) {
			// PML2 array doesn't exist?
			if( ! pml3[ p3 ] ) {
				// register new PML2 array inside PML3 entry
				pml3[ p3 ] = kernel_memory_alloc( TRUE ) & ~KERNEL_MEMORY_mirror;

				// failed to allocate page for PML2 array?
				if( ! pml3[ p3 ] ) return FALSE;

				// set flags for PML3 entry
				pml3[ p3 ] |= flags;
			}

			// set PML2 array pointer (remove flags)
			uint64_t *pml2 = (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml3[ p3 ] ) | KERNEL_MEMORY_mirror);
	
			// start with an entry representing given target in PML2 array
			for( ; p2 < KERNEL_PAGE_PMLx_entry; p2++ ) {
				// PML1 array doesn't exist?
				if( ! pml2[ p2 ] ) {
					// register new PML1 array inside PML2 entry
					pml2[ p2 ] = kernel_memory_alloc( TRUE ) & ~KERNEL_MEMORY_mirror;

					// failed to allocate page for PML1 array?
					if( ! pml2[ p2 ] ) return FALSE;

					// set flags for PML2 entry
					pml2[ p2 ] |= flags;
				}

				// set PML1 array pointer (remove flags)
				uint64_t *pml1 = (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml2[ p2 ] ) | KERNEL_MEMORY_mirror);
	
				// start with an entry representing given target in PML1 array
				for( ; p1 < KERNEL_PAGE_PMLx_entry; p1++ ) {
					// entry already exist?
					if( pml1[ p1 ] ) return FALSE;	// something is wrong...

					// copy page from source to target with flags
					pml1[ p1 ] = source | flags;

					// next page target from described physical memory area
					source += STD_PAGE_byte;

					// if all area is described
					if( ! --n ) return TRUE;
				}

				// first record of PML1 array
				p1 = 0;
			}

			// first record of PML2 array
			p2 = 0;
		}

		// first record of PML3 array
		p3 = 0;
	}

	// invalid address area mapping
	return FALSE;
}