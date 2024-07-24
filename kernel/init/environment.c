/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_environment( void ) {
	// remember largest chunk of physical memory
	uint64_t local_entry_length = EMPTY;

	// search through all memory map entries
	for( uint64_t i = 0; i < limine_memmap_request.response -> entry_count; i++ ) {
		// USABLE memory area?
		if( limine_memmap_request.response -> entries[ i ] -> type != LIMINE_MEMMAP_USABLE ) continue;	// no

		// this area is larger than previous one?
		if( local_entry_length > limine_memmap_request.response -> entries[ i ] -> length ) continue;	// no

		// remember size for later use
		local_entry_length = limine_memmap_request.response -> entries[ i ] -> length;

		// set kernel environment variables/functions/routines inside largest contiguous memory area (reflected in Higher Half)
		kernel = (struct KERNEL *) (limine_memmap_request.response -> entries[ i ] -> base | KERNEL_PAGE_mirror);
	}

	//----------------------------------------------------------------------

	// set information about framebuffer properties
	kernel -> framebuffer_base_address	= (uint32_t *) ((uintptr_t) limine_framebuffer_request.response -> framebuffers[ 0 ] -> address | KERNEL_PAGE_mirror);
	kernel -> framebuffer_width_pixel	= limine_framebuffer_request.response -> framebuffers[ 0 ] -> width;
	kernel -> framebuffer_height_pixel	= limine_framebuffer_request.response -> framebuffers[ 0 ] -> height;
	kernel -> framebuffer_pitch_byte	= limine_framebuffer_request.response -> framebuffers[ 0 ] -> pitch;
	kernel -> framebuffer_pid		= EMPTY;	// by default: kernel

	//----------------------------------------------------------------------

#ifdef LIB_TERMINAL
	// initialize terminal library
	kernel -> terminal.width		= kernel -> framebuffer_width_pixel;
	kernel -> terminal.height		= kernel -> framebuffer_height_pixel;
	kernel -> terminal.base_address		= kernel -> framebuffer_base_address;
	kernel -> terminal.scanline_pixel	= kernel -> framebuffer_pitch_byte >> STD_VIDEO_DEPTH_shift;
	kernel -> terminal.alpha		= EMPTY;	// 0 - not transparent, 255 - fully transparent
	kernel -> terminal.color_foreground	= lib_color( 7 );
	kernel -> terminal.color_background	= lib_color( 0 );
	lib_terminal( (struct LIB_TERMINAL_STRUCTURE *) &kernel -> terminal );

	// we do not need that feature inside kernels terminal
	lib_terminal_cursor_disable( (struct LIB_TERMINAL_STRUCTURE *) &kernel -> terminal );

	// terminal prepared
	kernel -> terminal_semaphore = TRUE;
#endif

	//----------------------------------------------------------------------

	// share HPET management functions
	kernel -> time_sleep			= (void *) kernel_time_sleep;

	// share IDT management functions
	kernel -> idt_mount			= (void *) kernel_idt_mount;

	// share I/O APIC management functions
	kernel -> io_apic_line_acquire		= (void *) kernel_io_apic_line_acquire;
	kernel -> io_apic_connect		= (void *) kernel_io_apic_connect;

	// share APIC management functions
	kernel -> lapic_accept			= (void *) kernel_lapic_accept;

	// share kernel early printf function
	kernel -> log				= (void *) kernel_log;

	// share memory management functions
	kernel -> memory_alloc			= (void *) kernel_memory_alloc;
	kernel -> memory_alloc_page		= (void *) kernel_memory_alloc_page;
	kernel -> memory_clean			= (void *) kernel_memory_clean;
	kernel -> memory_release		= (void *) kernel_memory_release;
	kernel -> memory_release_page		= (void *) kernel_memory_release_page;

	// share Module management function
	kernel -> module_thread			= (void *) kernel_module_thread;

	// share page management functions
	kernel -> page_deconstruct		= (void *) kernel_page_deconstruct;
	kernel -> page_map			= (void *) kernel_page_map;
	kernel -> page_release			= (void *) kernel_page_release;

	// share Stream management function
	kernel -> stream_release		= (void *) kernel_stream_release;

	// share memory management functions
	kernel -> task_active			= (void *) kernel_task_active;
	kernel -> task_pid			= (void *) kernel_task_pid;
	kernel -> task_by_id			= (void *) kernel_task_by_id;
}