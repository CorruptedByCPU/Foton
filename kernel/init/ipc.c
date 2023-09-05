/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_ipc( void ) {
	// prepare area for IPC messages
	kernel -> ipc_base_address = (struct STD_IPC_STRUCTURE *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( KERNEL_IPC_limit * sizeof( struct STD_IPC_STRUCTURE ) ) >> STD_SHIFT_PAGE );

	// unlock access to IPC exchange
	kernel -> ipc_semaphore = UNLOCK;
}