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