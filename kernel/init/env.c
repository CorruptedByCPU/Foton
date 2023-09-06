/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_env( void ) {
	// set information about framebuffer properties
	kernel -> framebuffer_base_address	= (uint32_t *) ((uintptr_t) limine_framebuffer_request.response -> framebuffers[ 0 ] -> address | KERNEL_PAGE_logical);
	kernel -> framebuffer_width_pixel	= limine_framebuffer_request.response -> framebuffers[ 0 ] -> width;
	kernel -> framebuffer_height_pixel	= limine_framebuffer_request.response -> framebuffers[ 0 ] -> height;
	kernel -> framebuffer_pitch_byte	= limine_framebuffer_request.response -> framebuffers[ 0 ] -> pitch;
	kernel -> framebuffer_owner_pid		= EMPTY;	// by default: kernel

	// share HPET management functions
	kernel -> time_sleep			= (void *) kernel_time_sleep;

	// share I/O APIC management functions
	kernel -> io_apic_line_acquire		= (void *) kernel_io_apic_line_acquire;

	// share kernel early printf function
	kernel -> log				= (void *) kernel_log;

	// share memory management functions
	kernel -> memory_alloc			= (void *) kernel_memory_alloc;

	// share memory management functions
	kernel -> task_active			= (void *) kernel_task_active;
	kernel -> task_pid			= (void *) kernel_task_pid;
	kernel -> task_by_id			= (void *) kernel_task_by_id;
}