/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_memory() {
	// limine shared with us a memory map?
	if( limine_memmap_request.response == NULL || ! limine_memmap_request.response -> entry_count ) {	// no
		// show an error
		lib_terminal_string( &kernel_terminal, "Houston, we have a problem.\n", 28 );

		// hold the door
		while( TRUE );
	}

	// find largest available area
	uint64_t local_largest_byte = EMPTY;

	// all available memory areas should be clean at kernel initialization
	lib_terminal_string( &kernel_terminal, "Memory map:\n", 14 );
	for( uint64_t i = 0; i < limine_memmap_request.response -> entry_count; i++ ) {
		// show memory range and type
		lib_terminal_printf( &kernel_terminal, " %16X - %16X ", limine_memmap_request.response -> entries[ i ] -> base, limine_memmap_request.response -> entries[ i ] -> base + limine_memmap_request.response -> entries[ i ] -> length - 1 );
		switch( limine_memmap_request.response -> entries[ i ] -> type ) {
			case LIMINE_MEMMAP_USABLE:			{ lib_terminal_printf( &kernel_terminal, "Usable\n" ); break; }
			case LIMINE_MEMMAP_RESERVED:			{ lib_terminal_printf( &kernel_terminal, "Reserved\n" ); break; }
			case LIMINE_MEMMAP_ACPI_RECLAIMABLE:		{ lib_terminal_printf( &kernel_terminal, "ACPI Reclaimable\n" ); break; }
			case LIMINE_MEMMAP_ACPI_NVS:			{ lib_terminal_printf( &kernel_terminal, "ACPI NVS\n" ); break; }
			case LIMINE_MEMMAP_BAD_MEMORY:			{ lib_terminal_printf( &kernel_terminal, "Corrupted Memory\n" ); break; }
			case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:	{ lib_terminal_printf( &kernel_terminal, "Bootloader Reclaimable\n" ); break; }
			case LIMINE_MEMMAP_KERNEL_AND_MODULES:		{ lib_terminal_printf( &kernel_terminal, "Kernel or Modules\n" ); break; }
			case LIMINE_MEMMAP_FRAMEBUFFER:			{ lib_terminal_printf( &kernel_terminal, "Framebuffer\n" ); break; }
			default:					{ lib_terminal_printf( &kernel_terminal, "{unknown}\n" ); }
		}

		// USABLE memory area?
		if( limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_USABLE ) {
			// clean it up
			kernel_page_clean( limine_memmap_request.response -> entries[ i ] -> base, limine_memmap_request.response -> entries[ i ] -> length >> STD_SHIFT_PAGE );

			// inside largest continous memory area we will create kernel environment metadata and a binary memory map next to it

			// this area is larger than previous one?
			if( local_largest_byte < limine_memmap_request.response -> entries[ i ] -> length ) {
				// keep logical address of largest continous memory area (reflected in Higher Half)
				kernel = (struct KERNEL *) (limine_memmap_request.response -> entries[ i ] -> base | KERNEL_PAGE_mirror);

				// keep size information
				local_largest_byte = limine_memmap_request.response -> entries[ i ] -> length;
			}
		}
	}

	// binary memory map base address will be placed after kernel environment variables/functions/rountines
	kernel -> memory_base_address = (uint64_t *) (MACRO_PAGE_ALIGN_UP( (uintptr_t) kernel + sizeof( struct KERNEL ) ));

	// describe all memory areas marked as USBALE inside binary memory map
	for( uint64_t i = 0; i < limine_memmap_request.response -> entry_count; i++ ) {
		// USABLE or BOOTLOADER_RECLAIMABLE memory area?
		if( limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_USABLE || limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE ) {
			// calculate farthest part of memory area for use
			if( kernel -> page_limit < MACRO_PAGE_ALIGN_DOWN( limine_memmap_request.response -> entries[ i ] -> base + limine_memmap_request.response -> entries[ i ] -> length ) >> STD_SHIFT_PAGE ) kernel -> page_limit = MACRO_PAGE_ALIGN_DOWN( limine_memmap_request.response -> entries[ i ] -> base + limine_memmap_request.response -> entries[ i ] -> length ) >> STD_SHIFT_PAGE;

			// keep number of pages registered in the binary memory map
			kernel -> page_total += MACRO_PAGE_ALIGN_DOWN( limine_memmap_request.response -> entries[ i ] -> length ) >> STD_SHIFT_PAGE;

			// only USABLE memory?
			if( limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_USABLE ) {
				// add memory area to available memory
				kernel -> page_available += MACRO_PAGE_ALIGN_DOWN( limine_memmap_request.response -> entries[ i ] -> length ) >> STD_SHIFT_PAGE;

				// register pages that are part of USABLE memory area
				for( uint64_t j = limine_memmap_request.response -> entries[ i ] -> base >> STD_SHIFT_PAGE; j < (limine_memmap_request.response -> entries[ i ] -> base + limine_memmap_request.response -> entries[ i ] -> length) >> STD_SHIFT_PAGE; j++ )
					kernel -> memory_base_address[ j / 64 ] |= 1 << j % 64;
			}
		}
	}

	// mark pages used by kernel environment variables/functions and binary memory map itself as mark as unavailable
	for( uint64_t i = ((uint64_t) kernel & ~KERNEL_PAGE_mirror) >> STD_SHIFT_PAGE; i < MACRO_PAGE_ALIGN_UP( (((uint64_t) kernel -> memory_base_address & ~KERNEL_PAGE_mirror) + (kernel -> page_limit >> STD_SHIFT_8)) ) >> STD_SHIFT_PAGE; i++ ) {
		// mark page as unavailable
		kernel -> memory_base_address[ i / 64 ] &= ~(1 << i % 64);

		// available pages
		kernel -> page_available--;
	}

	// inform about total memory area in KiB
	lib_terminal_printf( &kernel_terminal, "Memory size: %u KiB (%u KiB free)\n\n", kernel -> page_total << STD_SHIFT_4, kernel -> page_available << STD_SHIFT_4 );
}