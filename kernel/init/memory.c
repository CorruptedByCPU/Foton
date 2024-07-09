/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_memory( void ) {
	// limine shared with us a memory map?
	if( limine_memmap_request.response == NULL || ! limine_memmap_request.response -> entry_count ) {	// no
		// Houston, we have a problem.
		kernel_log( (uint8_t *) "KERNEL: Memory map not available.\n" );

		// hold the door
		while( TRUE );
	}

	// remember largest chunk of physical memory
	uint64_t local_largest_byte = EMPTY;

	// search through all memory map entries
	for( uint64_t i = 0; i < limine_memmap_request.response -> entry_count; i++ ) {
		// USABLE memory area?
		if( limine_memmap_request.response -> entries[ i ] -> type != LIMINE_MEMMAP_USABLE ) continue;	// no
		
		// this area is larger than previous one?
		if( local_largest_byte > limine_memmap_request.response -> entries[ i ] -> length ) continue;	// no

		// keep logical address of largest contiguous memory area (reflected in Higher Half)
		kernel = (struct KERNEL *) (limine_memmap_request.response -> entries[ i ] -> base | KERNEL_PAGE_mirror);

		// keep size information
		local_largest_byte = limine_memmap_request.response -> entries[ i ] -> length;
	}

	// register kprintf function in global kernel environment variables/functions/rountines
	kernel -> log = (void *) kernel_log;

	// binary memory map base address will be placed after global kernel environment variables/functions/rountines
	kernel -> memory_base_address = (uint32_t *) (MACRO_PAGE_ALIGN_UP( (uintptr_t) kernel + sizeof( struct KERNEL ) ));

	// describe all memory areas marked as USBALE inside binary memory map
	for( uint64_t i = 0; i < limine_memmap_request.response -> entry_count; i++ ) {
		// USABLE, BOOTLOADER_RECLAIMABLE, KERNEL_AND_MODULES or ACPI_RECLAIMABLE memory area?
		if( limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_USABLE || limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_KERNEL_AND_MODULES || limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE || limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_ACPI_RECLAIMABLE ) {
			// calculate farthest part of memory area for use
			if( kernel -> page_limit < MACRO_PAGE_ALIGN_DOWN( limine_memmap_request.response -> entries[ i ] -> base + limine_memmap_request.response -> entries[ i ] -> length ) >> STD_SHIFT_PAGE ) kernel -> page_limit = MACRO_PAGE_ALIGN_DOWN( limine_memmap_request.response -> entries[ i ] -> base + limine_memmap_request.response -> entries[ i ] -> length ) >> STD_SHIFT_PAGE;

			// keep number of pages registered in the binary memory map
			kernel -> page_total += MACRO_PAGE_ALIGN_DOWN( limine_memmap_request.response -> entries[ i ] -> length ) >> STD_SHIFT_PAGE;

			// USABLE memory area?
			if( limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_USABLE ) {
				// add memory area to available memory
				kernel -> page_available += MACRO_PAGE_ALIGN_DOWN( limine_memmap_request.response -> entries[ i ] -> length ) >> STD_SHIFT_PAGE;

				// register pages that are part of USABLE memory area
				for( uint64_t j = limine_memmap_request.response -> entries[ i ] -> base >> STD_SHIFT_PAGE; j < (limine_memmap_request.response -> entries[ i ] -> base + limine_memmap_request.response -> entries[ i ] -> length) >> STD_SHIFT_PAGE; j++ )
					kernel -> memory_base_address[ j >> STD_SHIFT_32 ] |= 1 << (j & 0b00011111);
			}

			// debug
			kernel -> log( (uint8_t *) "0x%16X - 0x%16X ", limine_memmap_request.response -> entries[ i ] -> base, limine_memmap_request.response -> entries[ i ] -> base + (limine_memmap_request.response -> entries[ i ] -> length - 1) );
			switch( limine_memmap_request.response -> entries[ i ] -> type ) {
				case LIMINE_MEMMAP_USABLE:			{ kernel -> log( (uint8_t *) "Usable\n" ); break; }
				case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:	{ kernel -> log( (uint8_t *) "Bootloader Reclaimable\n" ); break; }
				case LIMINE_MEMMAP_ACPI_RECLAIMABLE:		{ kernel -> log( (uint8_t *) "ACPI Reclaimable\n" ); break; }
				case LIMINE_MEMMAP_KERNEL_AND_MODULES:		{ kernel -> log( (uint8_t *) "Kernel and Modules\n" ); break; }
				default:					{ kernel -> log( (uint8_t *) "{UNKNOWN}\n" ); break; }
			}
		}
	}

	// mark pages used by global kernel environment variables/functions/rountines and binary memory map itself as unavailable
	for( uint64_t i = ((uintptr_t) kernel & ~KERNEL_PAGE_mirror) >> STD_SHIFT_PAGE; i < MACRO_PAGE_ALIGN_UP( (((uintptr_t) kernel -> memory_base_address & ~KERNEL_PAGE_mirror) + (kernel -> page_limit >> STD_SHIFT_8)) ) >> STD_SHIFT_PAGE; i++ ) {
		// mark page as unavailable
		kernel -> memory_base_address[ i >> STD_SHIFT_32 ] &= ~(1 << (i & 0b00011111));

		// available pages
		kernel -> page_available--;
	}

	// // extend binary memory map with a semaphore
	// kernel -> page_limit += STD_SIZE_BYTE_bit;	// semaphore is 1 Byte wide

	// // define memory semaphore location
	// uint8_t *semaphore = (uint8_t *) kernel -> memory_base_address + MACRO_PAGE_ALIGN_UP( kernel -> page_limit >> STD_SHIFT_8 ) - STD_SIZE_BYTE_byte;

	// // unlock access to binary memory map
	// MACRO_UNLOCK( *semaphore );
}