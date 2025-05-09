/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_env( void ) {
	// look for address of the largest chunk of physical memory (RAM) and the furthest
	uint64_t limit = 0;
	uint64_t max = 0;

	// search through all memory map entries provided by Limine Bootloader
	for( uint64_t i = 0; i < limine_memmap_request.response -> entry_count; i++ ) {
		// ignore irrelevant entries
		if( limine_memmap_request.response -> entries[ i ] -> type != LIMINE_MEMMAP_USABLE && limine_memmap_request.response -> entries[ i ] -> type != LIMINE_MEMMAP_KERNEL_AND_MODULES && limine_memmap_request.response -> entries[ i ] -> type != LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE && limine_memmap_request.response -> entries[ i ] -> type != LIMINE_MEMMAP_ACPI_RECLAIMABLE ) continue;

		// the farthest part of memory area?
		if( limit < (limine_memmap_request.response -> entries[ i ] -> base + limine_memmap_request.response -> entries[ i ] -> length) >> STD_SHIFT_PAGE ) limit = (limine_memmap_request.response -> entries[ i ] -> base + limine_memmap_request.response -> entries[ i ] -> length) >> STD_SHIFT_PAGE;

		// unUSABLE memory?
		if( limine_memmap_request.response -> entries[ i ] -> type != LIMINE_MEMMAP_USABLE ) continue;	// yes

		// this area is larger than previous one?
		if( max > limine_memmap_request.response -> entries[ i ] -> length ) continue;	// no

		// remember size for later use
		max = limine_memmap_request.response -> entries[ i ] -> length;

		// set kernel environment global variables/functions/rountines inside largest contiguous memory area (reflected in Higher Half)
		kernel = (struct KERNEL *) (limine_memmap_request.response -> entries[ i ] -> base | KERNEL_MEMORY_mirror);
	}

	//----------------------------------------------------------------------

	// clean'up, area required by kernel environment global variables/functions/rountines and binary memory map
	if( limit % 32768 ) limit += 32768 - (limit % 32767);
	kernel_memory_clean( (uint64_t *) kernel, limit >> STD_SHIFT_32768 );

	//----------------------------------------------------------------------

	kernel -> framebuffer_base_address	= (uint32_t *) ((uintptr_t) limine_framebuffer_request.response -> framebuffers[ 0 ] -> address | KERNEL_MEMORY_mirror);
	kernel -> framebuffer_width_pixel	= limine_framebuffer_request.response -> framebuffers[ 0 ] -> width;
	kernel -> framebuffer_height_pixel	= limine_framebuffer_request.response -> framebuffers[ 0 ] -> height;
	kernel -> framebuffer_pitch_byte	= limine_framebuffer_request.response -> framebuffers[ 0 ] -> pitch;
	kernel -> framebuffer_pid		= EMPTY;	// by default: kernel

	kernel -> device_mouse_x		= kernel -> framebuffer_width_pixel >> STD_SHIFT_2;
	kernel -> device_mouse_y		= kernel -> framebuffer_height_pixel >> STD_SHIFT_2;
	kernel -> device_mouse_z		= EMPTY;

	kernel -> idt_attach			= kernel_idt_attach;

	kernel -> io_apic_attach		= kernel_io_apic_attach;

	// kernel -> memory_alloc			= kernel_memory_alloc;
	kernel -> memory_alloc_low		= kernel_memory_alloc_low;
	kernel -> memory_clean			= kernel_memory_clean;
	kernel -> memory_release		= kernel_memory_release;

	kernel -> module_thread			= kernel_module_thread;

	kernel -> page_deconstruct		= kernel_page_deconstruct;
	kernel -> page_map			= kernel_page_map;

	kernel -> serial			= driver_serial;

	kernel -> task_by_id			= kernel_task_by_id;

	kernel -> time_sleep			= kernel_time_sleep;
}
