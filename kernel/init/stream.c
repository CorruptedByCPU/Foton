/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_stream( void ) {
	// prepare stream area
	kernel -> stream_base_address = (struct KERNEL_STREAM_STRUCTURE *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( KERNEL_STREAM_limit * sizeof( struct KERNEL_STREAM_STRUCTURE ) ) >> STD_SHIFT_PAGE );

	// prepare stream[s] for kernel process
	struct KERNEL_STREAM_STRUCTURE *stream = kernel_stream();

	// as a kernel, both stream[s] are of type null
	stream -> flags = KERNEL_STREAM_FLAG_null;

	// assign stream[s] to kernel task
	kernel -> task_base_address -> stream_out = stream;
	kernel -> task_base_address -> stream_in = stream;
}