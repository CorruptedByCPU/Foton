/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_ipc( void ) {
	// assign area for IPC messages
	kernel -> ipc_base_address = (struct STD_STRUCTURE_IPC *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( KERNEL_IPC_limit * sizeof( struct STD_STRUCTURE_IPC ) ) >> STD_SHIFT_PAGE );
}
