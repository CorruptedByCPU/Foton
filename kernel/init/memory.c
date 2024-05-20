/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_memory( void ) {
	// limine shared with us a memory map?
	if( limine_memmap_request.response == NULL || ! limine_memmap_request.response -> entry_count ) {	// no
		// Houston, we have a problem.
		kernel -> log( (uint8_t *) "Houston, we have a problem.\n" );

		// hold the door
		while( TRUE );
	}

	// find largest available area
	uint64_t local_largest_byte = EMPTY;

	// all available memory areas should be clean at kernel initialization
	for( uint64_t i = 0; i < limine_memmap_request.response -> entry_count; i++ ) {
		// USABLE memory area?
		if( limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_USABLE ) {
			// inside largest continous memory area we will create kernel environment metadata and a binary memory map next to it

			// this area is larger than previous one?
			if( local_largest_byte < limine_memmap_request.response -> entries[ i ] -> length ) {
				// keep logical address of largest continous memory area (reflected in Higher Half)
				kernel = (struct KERNEL *) (limine_memmap_request.response -> entries[ i ] -> base | KERNEL_PAGE_logical);

				// keep size information
				local_largest_byte = limine_memmap_request.response -> entries[ i ] -> length;
			}

			// we guarantee clean memory area at first use
			kernel_memory_clean( (uint64_t *) MACRO_PAGE_ALIGN_DOWN( limine_memmap_request.response -> entries[ i ] -> base | KERNEL_PAGE_logical ), MACRO_PAGE_ALIGN_DOWN( limine_memmap_request.response -> entries[ i ] -> length ) >> STD_SHIFT_PAGE );
		}
	}

	// binary memory map base address will be placed after kernel environment variables/functions/rountines
	kernel -> memory_base_address = (uint32_t *) (MACRO_PAGE_ALIGN_UP( (uintptr_t) kernel + sizeof( struct KERNEL ) ));

	// describe all memory areas marked as USBALE inside binary memory map
	for( uint64_t i = 0; i < limine_memmap_request.response -> entry_count; i++ ) {
		// USABLE, BOOTLOADER_RECLAIMABLE, KERNEL_AND_MODULES or ACPI_RECLAIMABLE memory area?
		if( limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_USABLE || limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_KERNEL_AND_MODULES || limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE || limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_ACPI_RECLAIMABLE ) {
			// calculate farthest part of memory area for use
			if( kernel -> page_limit < MACRO_PAGE_ALIGN_DOWN( limine_memmap_request.response -> entries[ i ] -> base + limine_memmap_request.response -> entries[ i ] -> length ) >> STD_SHIFT_PAGE ) kernel -> page_limit = MACRO_PAGE_ALIGN_DOWN( limine_memmap_request.response -> entries[ i ] -> base + limine_memmap_request.response -> entries[ i ] -> length ) >> STD_SHIFT_PAGE;

			// keep number of pages registered in the binary memory map
			kernel -> page_total += MACRO_PAGE_ALIGN_DOWN( limine_memmap_request.response -> entries[ i ] -> length ) >> STD_SHIFT_PAGE;

			// only USABLE memory area?
			if( limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_USABLE ) {
				// add memory area to available memory
				kernel -> page_available += MACRO_PAGE_ALIGN_DOWN( limine_memmap_request.response -> entries[ i ] -> length ) >> STD_SHIFT_PAGE;

				// register pages that are part of USABLE memory area
				for( uint64_t j = limine_memmap_request.response -> entries[ i ] -> base >> STD_SHIFT_PAGE; j < (limine_memmap_request.response -> entries[ i ] -> base + limine_memmap_request.response -> entries[ i ] -> length) >> STD_SHIFT_PAGE; j++ )
					kernel -> memory_base_address[ j >> STD_SHIFT_32 ] |= 1 << (j & 0b0011111);
			}
		}
	}

	// mark pages used by kernel environment variables/functions and binary memory map itself as unavailable
	for( uint64_t i = ((uint64_t) kernel & ~KERNEL_PAGE_logical) >> STD_SHIFT_PAGE; i < MACRO_PAGE_ALIGN_UP( (((uint64_t) kernel -> memory_base_address & ~KERNEL_PAGE_logical) + (kernel -> page_limit >> STD_SHIFT_8)) ) >> STD_SHIFT_PAGE; i++ ) {
		// mark page as unavailable
		kernel -> memory_base_address[ i >> STD_SHIFT_32 ] &= ~(1 << (i & 0b0011111));

		// available pages
		kernel -> page_available--;
	}

	// extend binary memory map with a semaphore
	kernel -> page_limit += STD_SIZE_BYTE_bit;	// semaphore is 1 Byte wide

	// define memory semaphore location
	uint8_t *semaphore = (uint8_t *) kernel -> memory_base_address + MACRO_PAGE_ALIGN_UP( kernel -> page_limit >> STD_SHIFT_8 ) - STD_SIZE_BYTE_byte;

	// unlock access to binary memory map
	MACRO_UNLOCK( *semaphore );
}