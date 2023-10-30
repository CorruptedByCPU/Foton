/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct KERNEL_STREAM_STRUCTURE *kernel_stream( void ) {
	// deny access, only one logical processor at a time
	while( __sync_val_compare_and_swap( &kernel -> stream_semaphore, UNLOCK, LOCK ) );

	// search stream table for free entry
	for( uint64_t i = 0; i < KERNEL_STREAM_limit; i++ ) {
		// free entry?
		if( ! kernel -> stream_base_address[ i ].base_address ) {
			// prepare area for stream content
			kernel -> stream_base_address[ i ].base_address = (uint8_t *) kernel_memory_alloc( STD_STREAM_SIZE_page );

			// there is currently no data in stream
			kernel -> stream_base_address[ i ].length = EMPTY;

			// number of processes assigned to stream
			kernel -> stream_base_address[ i ].lock = TRUE;

			// unlock access to function
			kernel -> stream_semaphore = UNLOCK;

			// return stream id
			return (struct KERNEL_STREAM_STRUCTURE *) &kernel -> stream_base_address[ i ];
		}
	}

	// unlock access to function
	kernel -> stream_semaphore = UNLOCK;

	// no free entry
	return	EMPTY;
}

uint64_t kernel_stream_in( uint8_t *target ) {
	// get the process output stream id
	struct KERNEL_TASK_STRUCTURE *task = (struct KERNEL_TASK_STRUCTURE *) kernel_task_active();

	// stream closed?
	if( task -> stream_out -> flags & KERNEL_STREAM_FLAG_closed ) return EMPTY;	// yes

	// retrieve stream length
	uint64_t length = task -> stream_in -> length;

	// stream contains data?
	if( ! length ) return EMPTY;	// no

	// send data from stream start to end marker
	for( uint64_t i = 0; i < length; i++ )
		target[ i ] = task -> stream_in -> base_address[ i ];

	// all flux space free
	task -> stream_in -> length = EMPTY;

	// return amount of transferred data in Bytes
	return length;
}

uint8_t kernel_stream_out( uint8_t *string, uint64_t length ) {
	// retrieve stream output of current task
	struct KERNEL_TASK_STRUCTURE *task = (struct KERNEL_TASK_STRUCTURE *) kernel_task_active();

	// stream closed?
	if( task -> stream_out -> flags & KERNEL_STREAM_FLAG_closed ) return FALSE;	// yes

	// block access to stream modification
	while( __sync_val_compare_and_swap( &task -> stream_out -> semaphore, UNLOCK, LOCK ) );

	// can we fit it?
	if( length > (STD_STREAM_SIZE_page << STD_SHIFT_PAGE) ) {	// no
		// unlock stream access
		task -> stream_out -> semaphore = UNLOCK;

		// unable to fit string
		return FALSE;
	}

	// wait for stream to be empty
	while( task -> stream_out -> length );

	// upload string to stream
	for( uint64_t i = 0; i < length; i++ ) task -> stream_out -> base_address[ i ] = string[ i ];

	// available area in stream	// send data from stream start to end marker
	task -> stream_out -> length = length;

	// string content is modified
	task -> stream_out -> flags |= KERNEL_STREAM_FLAG_modified;

	// unlock stream access
	task -> stream_out -> semaphore = UNLOCK;

	// string send
	return TRUE;
}