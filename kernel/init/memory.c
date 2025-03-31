/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_memory( void ) {
	// binary memory map will be placed right after kernel environment global variables/functions/rountines
	kernel -> memory_base_address = (uint32_t *) (MACRO_PAGE_ALIGN_UP( (uintptr_t) kernel + sizeof( struct KERNEL ) ));

	// describe all memory areas marked as USBALE inside binary memory map
	for( uint64_t i = 0; i < limine_memmap_request.response -> entry_count; i++ ) {
		// debug
		// if( limine_memmap_request.response -> entries[ i ] -> base > ((uintptr_t) kernel & ~KERNEL_MEMORY_mirror) ) break;	// yes

		// ignore irrelevant entries
		if( limine_memmap_request.response -> entries[ i ] -> type != LIMINE_MEMMAP_USABLE && limine_memmap_request.response -> entries[ i ] -> type != LIMINE_MEMMAP_KERNEL_AND_MODULES && limine_memmap_request.response -> entries[ i ] -> type != LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE && limine_memmap_request.response -> entries[ i ] -> type != LIMINE_MEMMAP_ACPI_RECLAIMABLE ) continue;

		// ignore not aligned entries
		if( limine_memmap_request.response -> entries[ i ] -> base & ~STD_PAGE_mask ) continue;

		// debug
		kernel_log( (uint8_t *) "\r0x%16X:0x%16X ", limine_memmap_request.response -> entries[ i ] -> base, limine_memmap_request.response -> entries[ i ] -> base + limine_memmap_request.response -> entries[ i ] -> length - 1 );
		switch( limine_memmap_request.response -> entries[ i ] -> type ) {
			case LIMINE_MEMMAP_USABLE: { kernel_log( (uint8_t *) "Usable\n" ); break; }
			case LIMINE_MEMMAP_KERNEL_AND_MODULES: { kernel_log( (uint8_t *) "Kernel and Modules\n" ); break; }
			case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE: { kernel_log( (uint8_t *) "Bootloader\n" ); break; }
			case LIMINE_MEMMAP_ACPI_RECLAIMABLE: { kernel_log( (uint8_t *) "ACPI (reclaimable)\n" ); break; }
			default: kernel_log( (uint8_t *) "\n" );
		}

		// retrieve the farthest part of memory area for use
		if( kernel -> page_limit < (limine_memmap_request.response -> entries[ i ] -> base + limine_memmap_request.response -> entries[ i ] -> length) >> STD_SHIFT_PAGE ) kernel -> page_limit = (limine_memmap_request.response -> entries[ i ] -> base + limine_memmap_request.response -> entries[ i ] -> length) >> STD_SHIFT_PAGE;

		kernel -> page_total += limine_memmap_request.response -> entries[ i ] -> length >> STD_SHIFT_PAGE;

		if( limine_memmap_request.response -> entries[ i ] -> type != LIMINE_MEMMAP_USABLE ) continue;

		uint64_t base = limine_memmap_request.response -> entries[ i ] -> base >> STD_SHIFT_PAGE;
		uint64_t limit = limine_memmap_request.response -> entries[ i ] -> length >> STD_SHIFT_PAGE;

		// prefix
		if( base & 0b00011111 ) {
			uint64_t prefix = STD_SIZE_DWORD_bit - (base & 0b00011111);

			if( limit < prefix ) prefix = limit;
			for( uint64_t k = base; k < base + prefix; k++ ) kernel -> memory_base_address[ k >> STD_SHIFT_32 ] |= 1 << (k & 0b00011111);

			if( limit == prefix ) continue;

			// cut parsed part
			base += prefix;
			limit -= prefix;
		}

		// middle
		uint64_t middle = limit & (uint64_t) ~0b00011111;
		if( middle ) {
			for( uint64_t k = base >> STD_SHIFT_32; k < (base + middle) >> STD_SHIFT_32; k++ ) kernel -> memory_base_address[ k ] |= STD_MAX_unsigned;

			// cut parsed part
			base += middle;
			limit -= middle;
		}

		// suffix
		if( limit ) for( uint64_t k = base; k < base + limit; k++ ) kernel -> memory_base_address[ k >> STD_SHIFT_32 ] |= 1 << (k & 0b00011111);

		kernel -> page_available += limine_memmap_request.response -> entries[ i ] -> length >> STD_SHIFT_PAGE;
	}

	// extend by modulo
	if( kernel -> page_limit % STD_SIZE_BYTE_bit ) kernel -> page_limit += (uint8_t) STD_SIZE_BYTE_bit - (kernel -> page_limit % STD_SIZE_BYTE_bit);

	// mark pages used by kernel environment global variables/functions/rountines and binary memory map itself as unavailable
	for( uint64_t i = ((uint64_t) kernel & ~KERNEL_MEMORY_mirror) >> STD_SHIFT_PAGE; i < (((uintptr_t) kernel -> memory_base_address + MACRO_PAGE_ALIGN_UP( (kernel -> page_limit >> STD_SHIFT_8) + 1 )) & ~KERNEL_MEMORY_mirror) >> STD_SHIFT_PAGE; i++ ) {
		// set unavailable
		kernel -> memory_base_address[ i >> STD_SHIFT_32 ] &= ~(1 << (i & 0b00011111));

		kernel -> page_available--;
	}
}
