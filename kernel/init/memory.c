/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_memory( void ) {
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

			// // debug
			// kernel_log( (uint8_t *) "0x%16X - 0x%16X ", limine_memmap_request.response -> entries[ i ] -> base, limine_memmap_request.response -> entries[ i ] -> base + (limine_memmap_request.response -> entries[ i ] -> length - 1) );
			// switch( limine_memmap_request.response -> entries[ i ] -> type ) {
			// 	case LIMINE_MEMMAP_USABLE:			{ kernel_log( (uint8_t *) "Usable\n" ); break; }
			// 	case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:	{ kernel_log( (uint8_t *) "Bootloader Reclaimable\n" ); break; }
			// 	case LIMINE_MEMMAP_ACPI_RECLAIMABLE:		{ kernel_log( (uint8_t *) "ACPI Reclaimable\n" ); break; }
			// 	case LIMINE_MEMMAP_KERNEL_AND_MODULES:		{ kernel_log( (uint8_t *) "Kernel and Modules\n" ); break; }
			// 	default:					{ kernel_log( (uint8_t *) "{UNKNOWN}\n" ); break; }
			// }
		}
	}

	// round up kernel page limit up to Byte
	if( kernel -> page_limit % STD_MOVE_BYTE ) kernel -> page_limit += STD_MOVE_BYTE - (kernel -> page_limit % STD_MOVE_BYTE);

	// mark pages used by global kernel environment variables/functions/rountines and binary memory map itself as unavailable
	for( uint64_t i = ((uintptr_t) kernel & ~KERNEL_PAGE_mirror) >> STD_SHIFT_PAGE; i < (((uintptr_t) kernel -> memory_base_address & ~KERNEL_PAGE_mirror) + MACRO_PAGE_ALIGN_UP( (kernel -> page_limit >> STD_SHIFT_8) + TRUE )) >> STD_SHIFT_PAGE; i++ ) {
		// mark page as unavailable
		kernel -> memory_base_address[ i >> STD_SHIFT_32 ] &= ~(1 << (i & 0b00011111));

		// available pages
		kernel -> page_available--;
	}
}