/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_env( void ) {
	// set information about framebuffer properties
	kernel -> framebuffer_base_address	= (uint32_t *) ((uintptr_t) limine_framebuffer_request.response -> framebuffers[ 0 ] -> address | KERNEL_PAGE_logical);
	kernel -> framebuffer_width_pixel	= limine_framebuffer_request.response -> framebuffers[ 0 ] -> width;
	kernel -> framebuffer_height_pixel	= limine_framebuffer_request.response -> framebuffers[ 0 ] -> height;
	kernel -> framebuffer_pitch_byte	= limine_framebuffer_request.response -> framebuffers[ 0 ] -> pitch;
	kernel -> framebuffer_pid		= EMPTY;	// by default: kernel

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

	// share Syscall management function
	kernel -> syscall_memory_alloc		= (void *) kernel_syscall_memory_alloc;

	// share memory management functions
	kernel -> task_active			= (void *) kernel_task_active;
	kernel -> task_pid			= (void *) kernel_task_pid;
	kernel -> task_by_id			= (void *) kernel_task_by_id;
}