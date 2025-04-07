/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_clean( void ) {
	// all memory areas used by bootloader (limine), can now be registered for system purposes
	for( uint64_t i = 0; i < limine_memmap_request.response -> entry_count; i++ ) {
		// for now we only need space below 1 MiB of physical memory
		if( ! (limine_memmap_request.response -> entries[ i ] -> base < 0x100000) ) continue;	// behind

		// BOOTLOADER_RECLAIMABLE memory area?
		if( limine_memmap_request.response -> entries[ i ] -> type != LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE ) continue;	// no

		// add memory area to available memory
		kernel -> page_available += MACRO_PAGE_ALIGN_DOWN( limine_memmap_request.response -> entries[ i ] -> length ) >> STD_SHIFT_PAGE;

		// register pages that are part of USABLE memory area
		kernel_memory_dispose( kernel -> memory_base_address, limine_memmap_request.response -> entries[ i ] -> base >> STD_SHIFT_PAGE, MACRO_PAGE_ALIGN_DOWN( limine_memmap_request.response -> entries[ i ] -> length ) >> STD_SHIFT_PAGE );
	}
}
