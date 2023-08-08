/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_syscall_exit( void ) {
	// properties of current task
	struct KERNEL_TASK_STRUCTURE *task = kernel -> task_cpu_address[ kernel_lapic_id() ];

	// mark task as not active and ready to close
	task -> flags &= ~KERNEL_TASK_FLAG_active;
	task -> flags |= KERNEL_TASK_FLAG_close;

	// release left BS/A time
	__asm__ volatile( "int $0x20" );
}

void kernel_syscall_framebuffer( struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER *framebuffer ) {
	// return information about existing framebuffer
	framebuffer -> base_address	= kernel -> framebuffer_base_address;
	framebuffer -> width_pixel	= kernel -> framebuffer_width_pixel;
	framebuffer -> height_pixel	= kernel -> framebuffer_height_pixel;
	framebuffer -> pitch_byte	= kernel -> framebuffer_pitch_byte;

	// change framebuffer owner if possible
	__sync_val_compare_and_swap( &kernel -> framebuffer_owner_pid, EMPTY, kernel_task_pid() );

	// return information about framebuffer owner
	framebuffer -> owner_pid	= kernel -> framebuffer_owner_pid;
}