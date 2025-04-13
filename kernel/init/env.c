/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_env( void ) {
	// look for address of the largest chunk of physical memory (RAM)
	uint64_t limit = INIT;

	// search through all memory map entries provided by Limine Bootloader
	for( uint64_t i = INIT; i < limine_memmap_request.response -> entry_count; i++ ) {
		// unUSABLE memory?
		if( limine_memmap_request.response -> entries[ i ] -> type != LIMINE_MEMMAP_USABLE ) continue;	// yes

		// this area is larger than previous one?
		if( limit > limine_memmap_request.response -> entries[ i ] -> length ) continue;	// no

		// remember size for later use
		limit = limine_memmap_request.response -> entries[ i ] -> length;

		// set kernel environment global variables/functions/rountines inside largest contiguous memory area (reflected in Higher Half)
		kernel = (struct KERNEL *) (limine_memmap_request.response -> entries[ i ] -> base | KERNEL_MEMORY_mirror);
	}

	//----------------------------------------------------------------------

	// clean'up, there will be binary memory map too!
	kernel_memory_clean( (uint64_t *) kernel, limit >> STD_SHIFT_PAGE );

	// retrieve properties of framebuffer
	kernel -> framebuffer_base_address	= (uint32_t *) ((uintptr_t) limine_framebuffer_request.response -> framebuffers[ 0 ] -> address | KERNEL_MEMORY_mirror);
	kernel -> framebuffer_width_pixel	= limine_framebuffer_request.response -> framebuffers[ 0 ] -> width;
	kernel -> framebuffer_height_pixel	= limine_framebuffer_request.response -> framebuffers[ 0 ] -> height;
	kernel -> framebuffer_pitch_byte	= limine_framebuffer_request.response -> framebuffers[ 0 ] -> pitch;
	kernel -> framebuffer_pid		= EMPTY;	// by default: kernel

	// initial position of mouse device
	kernel -> device_mouse_x		= kernel -> framebuffer_width_pixel >> STD_SHIFT_2;
	kernel -> device_mouse_y		= kernel -> framebuffer_height_pixel >> STD_SHIFT_2;

	//----------------------------------------------------------------------

	// kernel -> memory_alloc			= kernel_memory_alloc;
	kernel -> memory_alloc_low			= kernel_memory_alloc_low;
	kernel -> memory_clean				= kernel_memory_clean;
	kernel -> memory_release			= kernel_memory_release;

	kernel -> page_map					= kernel_page_map;

	kernel -> serial				= driver_serial;

	kernel -> time_sleep			= kernel_time_sleep;
}
