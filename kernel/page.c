/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uintptr_t kernel_page_address( uint64_t *pml4, uintptr_t source ) {
	// locate page table pointers
	uint64_t *pml3 = (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml4[ (source >> KERNEL_PAGE_PML4_shift) & (KERNEL_PAGE_PMLx_entry - 1) ] ) | KERNEL_MEMORY_mirror);
	uint64_t *pml2 = (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml3[ (source >> KERNEL_PAGE_PML3_shift) & (KERNEL_PAGE_PMLx_entry - 1) ] ) | KERNEL_MEMORY_mirror);
	uint64_t *pml1 = (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml2[ (source >> KERNEL_PAGE_PML2_shift) & (KERNEL_PAGE_PMLx_entry - 1) ] ) | KERNEL_MEMORY_mirror);

	// return address of physical page connected to area
	return MACRO_PAGE_ALIGN_DOWN( pml1[ (source >> KERNEL_PAGE_PML1_shift) & (KERNEL_PAGE_PMLx_entry - 1) ] );
}

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
			pml4[ p4 ] = kernel_memory_alloc_page();

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
				pml3[ p3 ] = kernel_memory_alloc_page();

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
					pml2[ p2 ] = kernel_memory_alloc_page();

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
				p1 = INIT;
			}

			// first entry of PML2 array
			p2 = INIT;
		}

		// first entry of PML3 array
		p3 = INIT;
	}

	// invalid area target
	return FALSE;
}

uint8_t kernel_page_clang( uint64_t *pml4, uintptr_t source, uintptr_t target, uint64_t n, uint16_t flags ) {
	// properties of target task
	struct KERNEL_STRUCTURE_TASK *current = kernel_task_current();

	// start with following array[ entries ]
	uint16_t p4 = (target >> KERNEL_PAGE_PML4_shift) & (KERNEL_PAGE_PMLx_entry - 1); 
	uint16_t p3 = (target >> KERNEL_PAGE_PML3_shift) & (KERNEL_PAGE_PMLx_entry - 1);
	uint16_t p2 = (target >> KERNEL_PAGE_PML2_shift) & (KERNEL_PAGE_PMLx_entry - 1);
	uint16_t p1 = (target >> KERNEL_PAGE_PML1_shift) & (KERNEL_PAGE_PMLx_entry - 1);

	// start with an entry representing given target in PML4 array
	for( ; p4 < KERNEL_PAGE_PMLx_entry; p4++ ) {
		// PML3 array doesn't exist?
		if( ! pml4[ p4 ] ) {
			// register new PML3 array inside PML4 entry
			pml4[ p4 ] = kernel_memory_alloc_page();

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
				pml3[ p3 ] = kernel_memory_alloc_page();

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
					pml2[ p2 ] = kernel_memory_alloc_page();

					// failed to allocate page for PML1 array?
					if( ! pml2[ p2 ] ) return FALSE;

					// set flags for PML2 entry
					pml2[ p2 ] |= flags;
				}

				// set PML1 array pointer (remove flags)
				uint64_t *pml1 = (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml2[ p2 ] ) | KERNEL_MEMORY_mirror);

				// start with an entry representing given address in PML1 array
				for( ; p1 < KERNEL_PAGE_PMLx_entry; p1++ ) {
					// connect pages
					pml1[ p1 ] = kernel_page_address( (uint64_t *) current -> cr3, source );

					// source page do not exist?
					if( ! pml1[ p1 ] ) return FALSE;

					// assign flags to shared page
					pml1[ p1 ] |= flags;

					// next part of space
					source += STD_PAGE_byte;

					// if all area is described
					if( ! --n ) return TRUE;
				}

				// first entry of PML1 array
				p1 = INIT;
			}

			// first entry of PML2 array
			p2 = INIT;
		}

		// first entry of PML3 array
		p3 = INIT;
	}

	// done
	return TRUE;
}

uint8_t kernel_page_empty( uint64_t *target, uint64_t n ) {
	// check every entry
	for( uint64_t i = INIT; i < (n << STD_SHIFT_512); i++ ) if( target[ i ] ) return FALSE;

	// page is empty
	return TRUE;
}

void kernel_page_deconstruct( uint64_t *pml4, uint8_t type ) {
	// for each entry of PML4 array
	for( uint16_t p4 = INIT; p4 < KERNEL_PAGE_PMLx_entry; p4++ ) {
		// empty?
		if( ! pml4[ p4 ] ) continue;	// skip

		// properties of PML3 array
		uint64_t *pml3 = (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml4[ p4 ] ) | KERNEL_MEMORY_mirror);

		// for each entry of PML3 array
		for( uint16_t p3 = INIT; p3 < KERNEL_PAGE_PMLx_entry; p3++ ) {
			// empty?
			if( ! pml3[ p3 ] ) continue;	// skip

			// properties of PML2 array
			uint64_t *pml2 = (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml3[ p3 ] ) | KERNEL_MEMORY_mirror);

			// for each entry of PML2 array
			for( uint16_t p2 = INIT; p2 < KERNEL_PAGE_PMLx_entry; p2++ ) {
				// empty?
				if( ! pml2[ p2 ] ) continue;	// skip

				// properties of PML1 array
				uint64_t *pml1 = (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml2[ p2 ] ) | KERNEL_MEMORY_mirror);
		
				// for each entry of PML1 array
				for( uint16_t p1 = INIT; p1 < KERNEL_PAGE_PMLx_entry; p1++ ) {
					// entry doesn't belongs to task or is shared?
					if( ((uint16_t) pml1[ p1 ] & KERNEL_PAGE_TYPE_mask) != (type << KERNEL_PAGE_TYPE_offset) ) continue;	// yes
				
					// release page from array
					kernel_memory_release( MACRO_PAGE_ALIGN_DOWN( pml1[ p1 ] ) | KERNEL_MEMORY_mirror, TRUE );

					// remove entry from PML1 array
					pml1[ p1 ] = EMPTY;
				}

				// if PML1 array doesn't belong to task, omit
				if( ((uint16_t) pml2[ p2 ] & KERNEL_PAGE_TYPE_mask) != (type << KERNEL_PAGE_TYPE_offset) ) continue;

				// release
				kernel_memory_release_page( MACRO_PAGE_ALIGN_DOWN( pml2[ p2 ] ) );

				// remove entry from PML2 array
				pml2[ p2 ] = EMPTY;
			}

			// if PML2 array doesn't belong to task, omit
			if( ((uint16_t) pml3[ p3 ] & KERNEL_PAGE_TYPE_mask) != (type << KERNEL_PAGE_TYPE_offset) ) continue;

			// release
			kernel_memory_release_page( MACRO_PAGE_ALIGN_DOWN( pml3[ p3 ] ) );

			// remove entry from PML3 array
			pml3[ p3 ] = EMPTY;
		}

		// if PML3 array doesn't belong to task, omit
		if( ((uint16_t) pml4[ p4 ] & KERNEL_PAGE_TYPE_mask) != (type << KERNEL_PAGE_TYPE_offset) ) continue;

		// release
		kernel_memory_release_page( MACRO_PAGE_ALIGN_DOWN( pml4[ p4 ] ) );

		// remove entry from PML4 array
		pml4[ p4 ] = EMPTY;
	}

	// release
	kernel_memory_release_page( (uintptr_t) pml4 & ~KERNEL_MEMORY_mirror );
}

uint8_t kernel_page_disconnect( uint64_t *pml4, uint64_t source, uint64_t n ) {
	// start with following array[ entries ]
	uint16_t p4 = (source >> KERNEL_PAGE_PML4_shift) & (KERNEL_PAGE_PMLx_entry - 1); 
	uint16_t p3 = (source >> KERNEL_PAGE_PML3_shift) & (KERNEL_PAGE_PMLx_entry - 1);
	uint16_t p2 = (source >> KERNEL_PAGE_PML2_shift) & (KERNEL_PAGE_PMLx_entry - 1);
	uint16_t p1 = (source >> KERNEL_PAGE_PML1_shift) & (KERNEL_PAGE_PMLx_entry - 1);

	// start with an entry representing given address in PML4 array
	for( ; p4 < KERNEL_PAGE_PMLx_entry; p4++ ) {
		// PML3 array exist?
		if( ! pml4[ p4 ] ) return FALSE;	// no

		// get PML3 array address (remove flags)
		uint64_t *pml3 = (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml4[ p4 ] ) | KERNEL_MEMORY_mirror);

		// start with an entry representing given address in PML3 array
		for( ; p3 < KERNEL_PAGE_PMLx_entry; p3++ ) {
			// PML2 array exist?
			if( ! pml3[ p3 ] ) return FALSE;	// no

			// get PML2 array address (remove flags)
			uint64_t *pml2 = (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml3[ p3 ] ) | KERNEL_MEMORY_mirror);

			// start with an entry representing given address in PML2 array
			for( ; p2 < KERNEL_PAGE_PMLx_entry; p2++ ) {
				// PML1 array exist?
				if( ! pml2[ p2 ] ) return FALSE;	// no

				// get PML1 array address (remove flags)
				uint64_t *pml1 = (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml2[ p2 ] ) | KERNEL_MEMORY_mirror);

				// start with an entry representing given address in PML1 array
				for( ; p1 < KERNEL_PAGE_PMLx_entry; p1++ ) {
					// entry exist?
					if( ! pml1[ p1 ] ) return FALSE;	// no

					// remove entry
					pml1[ p1 ] = EMPTY;

					// whole area is disconnected
					if( ! --n ) return TRUE;
				}

				// first entry of PML1 array
				p1 = INIT;
			}

			// first entry of PML2 array
			p2 = INIT;
		}

		// first entry of PML3 array
		p3 = INIT;
	}

	// something is wrong
	return FALSE;
}

inline void kernel_page_flush( void ) {
	// reload paging structure
	__asm__ volatile( "push %rax\nmovq %cr3, %rax\nmovq %rax, %cr3\npop %rax" );
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
			pml4[ p4 ] = kernel_memory_alloc_page();

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
				pml3[ p3 ] = kernel_memory_alloc_page();

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
					pml2[ p2 ] = kernel_memory_alloc_page();

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
				p1 = INIT;
			}

			// first record of PML2 array
			p2 = INIT;
		}

		// first record of PML3 array
		p3 = INIT;
	}

	// invalid address area mapping
	return FALSE;
}

void kernel_page_merge( uint64_t *pml4_parent, uint64_t *pml4_child ) {
	// start with PML4 level of both arrays
	for( uint16_t p4 = INIT; p4 < KERNEL_PAGE_PMLx_entry; p4++ ) {
		// source entry exists?
		if( ! pml4_parent[ p4 ] ) continue;	// no

		// no target entry?
		if( ! pml4_child[ p4 ] ) {
			// reload space from source array to destination array
			pml4_child[ p4 ] = pml4_parent[ p4 ];

			// next entry from source array
			continue;
		}

		// PML3
		uint64_t *pml3_parent = (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml4_parent[ p4 ] ) | KERNEL_MEMORY_mirror);
		uint64_t *pml3_child = (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml4_child[ p4 ] ) | KERNEL_MEMORY_mirror);
		for( uint16_t p3 = INIT; p3 < KERNEL_PAGE_PMLx_entry; p3++ ) {
			// source entry exists?
			if( ! pml3_parent[ p3 ] ) continue;	// no

			// no target entry?
			if( ! pml3_child[ p3 ] ) {
				// reload space from source array to destination array
				pml3_child[ p3 ] = pml3_parent[ p3 ];

				// next entry from source array
				continue;
			}

			// PML2
			uint64_t *pml2_parent = (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml3_parent[ p3 ] ) | KERNEL_MEMORY_mirror);
			uint64_t *pml2_child = (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml3_child[ p3 ] ) | KERNEL_MEMORY_mirror);
			for( uint16_t p2 = INIT; p2 < KERNEL_PAGE_PMLx_entry; p2++ ) {
				// source entry exists?
				if( ! pml2_parent[ p2 ] ) continue;	// no

				// no target entry?
				if( ! pml2_child[ p2 ] ) {
					// reload space from source array to destination array
					pml2_child[ p2 ] = pml2_parent[ p2 ];

					// next entry from source array
					continue;
				}

				// PML1
				uint64_t *pml1_parent = (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml2_parent[ p2 ] ) | KERNEL_MEMORY_mirror);
				uint64_t *pml1_child = (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml2_child[ p2 ] ) | KERNEL_MEMORY_mirror);
				for( uint16_t p1 = INIT; p1 < KERNEL_PAGE_PMLx_entry; p1++ ) {
					// source entry exist?
					if( ! pml1_parent[ p1 ] ) continue;	// no

					// target entry exist?
					if( pml1_child[ p1 ] ) continue;	// yes

					// reload space from source array to destination array
					pml1_child[ p1 ] = pml1_parent[ p1 ];
				}
			}
		}
	}
}

uint8_t kernel_page_release( uint64_t *pml4, uint64_t source, uint64_t n ) {
	// start with following array[ entries ]
	uint16_t p4 = (source >> KERNEL_PAGE_PML4_shift) & (KERNEL_PAGE_PMLx_entry - 1); 
	uint16_t p3 = (source >> KERNEL_PAGE_PML3_shift) & (KERNEL_PAGE_PMLx_entry - 1);
	uint16_t p2 = (source >> KERNEL_PAGE_PML2_shift) & (KERNEL_PAGE_PMLx_entry - 1);
	uint16_t p1 = (source >> KERNEL_PAGE_PML1_shift) & (KERNEL_PAGE_PMLx_entry - 1);

	// start with an entry representing given address in PML4 array
	for( ; p4 < KERNEL_PAGE_PMLx_entry; p4++ ) {
		// PML3 array doesn't exist?
		if( ! pml4[ p4 ] ) return FALSE;	// not good...

		// get PML3 array address (remove flags)
		uint64_t *pml3 = (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml4[ p4 ] ) | KERNEL_MEMORY_mirror);

		// start with an entry representing given address in PML3 array
		for( ; p3 < KERNEL_PAGE_PMLx_entry; p3++ ) {
			// PML2 array doesn't exist?
			if( ! pml3[ p3 ] ) return FALSE;	// not good...

			// get PML2 array address (remove flags)
			uint64_t *pml2 = (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml3[ p3 ] ) | KERNEL_MEMORY_mirror);
	
			// start with an entry representing given address in PML2 array
			for( ; p2 < KERNEL_PAGE_PMLx_entry; p2++ ) {
				// PML1 array doesn't exist?
				if( ! pml2[ p2 ] ) return FALSE;	// not good...

				// get PML1 array address (remove flags)
				uint64_t *pml1 = (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml2[ p2 ] ) | KERNEL_MEMORY_mirror);
	
				// start with an entry representing given address in PML1 array
				for( ; p1 < KERNEL_PAGE_PMLx_entry; p1++ ) {
					// PAGE doesn't exist?
					if( ! pml1[ p1 ] ) return FALSE;	// not good...

					// release memory area
					kernel_memory_release( MACRO_PAGE_ALIGN_DOWN( pml1[ p1 ] ) | KERNEL_MEMORY_mirror, TRUE );

					// remove entry from PML1 array
					pml1[ p1 ] = EMPTY;

					// area removed?
					if( ! --n ) return TRUE;
				}

				// if page is empty
				if( kernel_page_empty( (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml2[ p2 ] ) | KERNEL_MEMORY_mirror), TRUE ) ) {
					// release
					kernel_memory_release_page( MACRO_PAGE_ALIGN_DOWN( pml2[ p2 ] ) );

					// remove entry from PML2 array
					pml2[ p2 ] = EMPTY;
				}

				// first entry of next PML1 array
				p1 = INIT;
			}

			// if page is empty
			if( kernel_page_empty( (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml3[ p3 ] ) | KERNEL_MEMORY_mirror), TRUE ) ) {
				// release
				kernel_memory_release_page( MACRO_PAGE_ALIGN_DOWN( pml3[ p3 ] ) );

				// remove entry from PML3 array
				pml3[ p3 ] = EMPTY;
			}

			// first entry of next PML2 array
			p2 = INIT;
		}

		// if page is empty
		if( kernel_page_empty( (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml4[ p4 ] ) | KERNEL_MEMORY_mirror), TRUE ) ) {
			// release
			kernel_memory_release_page( MACRO_PAGE_ALIGN_DOWN( pml4[ p4 ] ) );

			// remove entry from PML4 array
			pml4[ p4 ] = EMPTY;
		}

		// first entry of next PML3 array
		p3 = INIT;
	}

	// done
	return FALSE;
}
