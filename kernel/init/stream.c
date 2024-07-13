/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_stream( void ) {
	// prepare streams registry
	kernel -> stream_base_address = (struct KERNEL_STREAM_STRUCTURE *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( KERNEL_STREAM_limit * sizeof( struct KERNEL_STREAM_STRUCTURE ) ) >> STD_SHIFT_PAGE );
}