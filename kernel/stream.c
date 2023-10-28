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
			kernel -> stream_base_address[ i ].base_address = (uint8_t *) kernel_memory_alloc( KERNEL_STREAM_SIZE_page );

			// there is currently no data in stream
			kernel -> stream_base_address[ i ].start = EMPTY;
			kernel -> stream_base_address[ i ].end = EMPTY;

			// whole stream available
			kernel -> stream_base_address[ i ].available_byte = STD_PAGE_byte;

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

uint8_t kernel_stream_out( uint8_t *string, uint64_t length ) {
	// retrieve stream output of current task
	struct KERNEL_TASK_STRUCTURE *task = (struct KERNEL_TASK_STRUCTURE *) kernel_task_active();

	// stream closed?
	if( task -> stream_out -> flags & KERNEL_STREAM_FLAG_closed ) return FALSE;	// yes

	// block access to stream modification
	while( __sync_val_compare_and_swap( &task -> stream_out -> semaphore, UNLOCK, LOCK ) );

	// can we fit content?
	if( length > task -> stream_out -> available_byte ) {
		// unlock stream access
		task -> stream_out -> semaphore = UNLOCK;

		// unable to fit content
		return FALSE;
	}

	// string index
	uint16_t i = 0;

	// if end marker is behind of start marker
	if( task -> stream_out -> end >= task -> stream_out -> start ) {
		// send data from end marker to end of stream
		while( i < length && task -> stream_out -> end < (KERNEL_STREAM_SIZE_page << STD_SHIFT_PAGE) ) task -> stream_out -> base_address[ task -> stream_out -> end++ ] = string[ i++ ];

		// if not whole string was uploaded
		if( i < length ) 
			// set new end marker
			task -> stream_out -> end = EMPTY;
	}

	// send data from stream start to end marker
	while( i < length ) task -> stream_out -> base_address[ task -> stream_out -> end++ ] = string[ i++ ];

	// available area in stream
	task -> stream_out -> available_byte -= length;

	// string content is modified
	task -> stream_out -> flags |= KERNEL_STREAM_FLAG_modified;

	// unlock stream access
	task -> stream_out -> semaphore = UNLOCK;

	// string send
	return TRUE;
}