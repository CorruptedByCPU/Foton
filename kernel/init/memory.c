/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_memory( void ) {
	// limine shared with us a memory map?
	if( limine_memmap_request.response == NULL || ! limine_memmap_request.response -> entry_count ) {	// no
		#ifdef	DEBUG
			kernel -> log( (uint8_t *) "Houston, we have a problem.\n" );
		#endif

		// hold the door
		while( TRUE );
	}

	// find largest available area
	uint64_t local_largest_byte = EMPTY;

	// kernel -> log( (uint8_t *) "Memory map:\n" );

	// all available memory areas should be clean at kernel initialization
	for( uint64_t i = 0; i < limine_memmap_request.response -> entry_count; i++ ) {
			// kernel -> log( (uint8_t *) " %16X - %16X ", limine_memmap_request.response -> entries[ i ] -> base, limine_memmap_request.response -> entries[ i ] -> base + limine_memmap_request.response -> entries[ i ] -> length - 1 );
			// switch( limine_memmap_request.response -> entries[ i ] -> type ) {
			// 	case LIMINE_MEMMAP_USABLE:			{ kernel -> log( (uint8_t *) "Usable\n" ); break; }
			// 	case LIMINE_MEMMAP_RESERVED:			{ kernel -> log( (uint8_t *) "Reserved\n" ); break; }
			// 	case LIMINE_MEMMAP_ACPI_RECLAIMABLE:		{ kernel -> log( (uint8_t *) "ACPI Reclaimable\n" ); break; }
			// 	case LIMINE_MEMMAP_ACPI_NVS:			{ kernel -> log( (uint8_t *) "ACPI NVS\n" ); break; }
			// 	case LIMINE_MEMMAP_BAD_MEMORY:			{ kernel -> log( (uint8_t *) "Corrupted Memory\n" ); break; }
			// 	case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:	{ kernel -> log( (uint8_t *) "Bootloader Reclaimable\n" ); break; }
			// 	case LIMINE_MEMMAP_KERNEL_AND_MODULES:		{ kernel -> log( (uint8_t *) "Kernel or Modules\n" ); break; }
			// 	case LIMINE_MEMMAP_FRAMEBUFFER:			{ kernel -> log( (uint8_t *) "Framebuffer\n" ); break; }
			// 	default:					{ kernel -> log( (uint8_t *) "{unknown}\n" ); }
			// }

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
		}
	}

	// initialize crucial variables
	kernel_page_clean( (uintptr_t) kernel, MACRO_PAGE_ALIGN_UP( sizeof( struct KERNEL ) ) >> STD_SHIFT_PAGE );

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

	// kernel -> log( (uint8_t *) "Memory size: %u KiB (%u KiB free)\n", kernel -> page_total << STD_SHIFT_4, kernel -> page_available << STD_SHIFT_4 );
}