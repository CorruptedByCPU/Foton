/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_page_clean( uintptr_t address, uint64_t n ) {
	// properties of address
	uint64_t *page = (uint64_t *) address;

	// clear every 64 bit value inside N pages
	for( uint64_t i = 0; i < n << STD_SHIFT_512; i++ ) page[ i ] = EMPTY;
}