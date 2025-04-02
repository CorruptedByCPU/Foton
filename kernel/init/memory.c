/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_memory( void ) {
	// binary memory map will be placed right after kernel environment global variables/functions/rountines
	kernel -> memory_base_address = (uint32_t *) (MACRO_PAGE_ALIGN_UP( (uintptr_t) kernel + sizeof( struct KERNEL ) ));

	// describe all memory areas marked as USBALE inside binary memory map
	for( uint64_t i = INIT; i < limine_memmap_request.response -> entry_count; i++ ) {
		// ignore irrelevant entries
		if( limine_memmap_request.response -> entries[ i ] -> type != LIMINE_MEMMAP_USABLE && limine_memmap_request.response -> entries[ i ] -> type != LIMINE_MEMMAP_KERNEL_AND_MODULES && limine_memmap_request.response -> entries[ i ] -> type != LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE && limine_memmap_request.response -> entries[ i ] -> type != LIMINE_MEMMAP_ACPI_RECLAIMABLE ) continue;

		// ignore not aligned entries
		if( limine_memmap_request.response -> entries[ i ] -> base & ~STD_PAGE_mask ) continue;

		// retrieve the farthest part of memory area for use
		if( kernel -> page_limit < (limine_memmap_request.response -> entries[ i ] -> base + limine_memmap_request.response -> entries[ i ] -> length) >> STD_SHIFT_PAGE ) kernel -> page_limit = (limine_memmap_request.response -> entries[ i ] -> base + limine_memmap_request.response -> entries[ i ] -> length) >> STD_SHIFT_PAGE;

		// keep number of pages that exist in binary memory map
		kernel -> page_total += limine_memmap_request.response -> entries[ i ] -> length >> STD_SHIFT_PAGE;

		// USABLE memory area?
		if( limine_memmap_request.response -> entries[ i ] -> type != LIMINE_MEMMAP_USABLE ) continue;	// no

		// for readability
		uint64_t base = limine_memmap_request.response -> entries[ i ] -> base >> STD_SHIFT_PAGE;
		uint64_t limit = limine_memmap_request.response -> entries[ i ] -> length >> STD_SHIFT_PAGE;

		// mark bits inside binary memory map by chunks (64 bit wide)

		// prefix
		if( base & STD_BIT_CONTROL_DWORD_bit ) {
			// amount of bits to the end of chunk
			uint64_t prefix = STD_SIZE_DWORD_bit - (base & STD_BIT_CONTROL_DWORD_bit);

			// if amount og bits larger than limit
			if( limit < prefix ) prefix = limit;	// do only limit
			for( uint64_t k = base; k < base + prefix; k++ ) kernel -> memory_base_address[ k >> STD_SHIFT_32 ] |= 1 << (k & STD_BIT_CONTROL_DWORD_bit);

			// no more bits of this limit?
			if( limit == prefix ) continue;	// yes

			// remove registered bits
			base += prefix;
			limit -= prefix;
		}

		// amount of full chunks
		uint64_t chunks = limit & (uint64_t) ~STD_BIT_CONTROL_DWORD_bit;

		// register them?
		if( chunks ) {
			// yes
			for( uint64_t k = base >> STD_SHIFT_32; k < (base + chunks) >> STD_SHIFT_32; k++ ) kernel -> memory_base_address[ k ] |= STD_MAX_unsigned;

			// remove registered chunks of bits
			base += chunks;
			limit -= chunks;
		}

		// suffix

		// there are some bits left?
		if( limit ) for( uint64_t k = base; k < base + limit; k++ ) kernel -> memory_base_address[ k >> STD_SHIFT_32 ] |= 1 << (k & STD_BIT_CONTROL_DWORD_bit);	// yes

		// added memory area to available memory
		kernel -> page_available += limine_memmap_request.response -> entries[ i ] -> length >> STD_SHIFT_PAGE;
	}

	// extend by modulo
	if( kernel -> page_limit % STD_SIZE_BYTE_bit ) kernel -> page_limit += (uint8_t) STD_SIZE_BYTE_bit - (kernel -> page_limit % STD_SIZE_BYTE_bit);

	// mark pages used by kernel environment global variables/functions/rountines and binary memory map itself as unavailable
	for( uint64_t i = ((uint64_t) kernel & ~KERNEL_MEMORY_mirror) >> STD_SHIFT_PAGE; i < (((uintptr_t) kernel -> memory_base_address + MACRO_PAGE_ALIGN_UP( (kernel -> page_limit >> STD_SHIFT_8) + 1 )) & ~KERNEL_MEMORY_mirror) >> STD_SHIFT_PAGE; i++ ) {
		// set as unavailable
		kernel -> memory_base_address[ i >> STD_SHIFT_32 ] &= ~(1 << (i & STD_BIT_CONTROL_DWORD_bit));

		// available pages removed
		kernel -> page_available--;
	}
}
