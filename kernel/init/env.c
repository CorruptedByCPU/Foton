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
}