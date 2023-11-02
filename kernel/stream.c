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
			kernel -> stream_base_address[ i ].base_address	= (uint8_t *) kernel_memory_alloc( STD_STREAM_SIZE_page );

			// there is currently no data in stream
			kernel -> stream_base_address[ i ].length_byte	= EMPTY;

			// start and end mark cleared
			kernel -> stream_base_address[ i ].start	= EMPTY;
			kernel -> stream_base_address[ i ].end		= EMPTY;		

			// number of processes assigned to stream
			kernel -> stream_base_address[ i ].lock		= TRUE;

			// unlock access to function
			kernel -> stream_semaphore			= UNLOCK;

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

	// stream closed or empty?
	if( task -> stream_in -> flags & KERNEL_STREAM_FLAG_closed || ! task -> stream_in -> length_byte ) return EMPTY;	// yes

	// block access to stream modification
	while( __sync_val_compare_and_swap( &task -> stream_in -> semaphore, UNLOCK, LOCK ) );

	// passed data from stream
	uint64_t length = EMPTY;

	// send data from start marker to end of stream
	if( task -> stream_in -> start >= task -> stream_in -> end ) {
		while( task -> stream_in -> start < (STD_STREAM_SIZE_page << STD_SHIFT_PAGE) )
			target[ length++ ] = task -> stream_in -> base_address[ task -> stream_in -> start++ ];

		// move start marker at beginning of stream content
		if( task -> stream_in -> start == (STD_STREAM_SIZE_page << STD_SHIFT_PAGE) || length ) task -> stream_in -> start = EMPTY;
	}

	// send data from start marker to end marker
	while( task -> stream_in -> start != task -> stream_in -> end )
		target[ length++ ] = task -> stream_in -> base_address[ task -> stream_in -> start++ ];

	// stream empty
	task -> stream_in -> length_byte = EMPTY;

	// unlock stream modification
	task -> stream_in -> semaphore = UNLOCK;

	// return amount of transferred data in Bytes
	return length;
}

uint8_t kernel_stream_out( uint8_t *string, uint64_t length ) {
	// retrieve stream output of current task
	struct KERNEL_TASK_STRUCTURE *task = (struct KERNEL_TASK_STRUCTURE *) kernel_task_active();

	// stream closed or full?
	if( task -> stream_out -> flags & KERNEL_STREAM_FLAG_closed || task -> stream_out -> length_byte == STD_STREAM_SIZE_page << STD_SHIFT_PAGE ) return FALSE;	// yes

	// block access to stream modification
	while( __sync_val_compare_and_swap( &task -> stream_out -> semaphore, UNLOCK, LOCK ) );

	// amount of data inside stream after operation
	task -> stream_out -> length_byte += length;

	// can we fit it?
	if( length > (STD_STREAM_SIZE_page << STD_SHIFT_PAGE) - task -> stream_out -> length_byte ) {	// no
		// unlock stream access
		task -> stream_out -> semaphore = UNLOCK;

		// unable to fit string
		return FALSE;
	}

	// insert data from end marker to end of stream
	if( task -> stream_out -> end >= task -> stream_out -> start ) {
		while( length && task -> stream_out -> end != (STD_STREAM_SIZE_page << STD_SHIFT_PAGE) ) {
			task -> stream_out -> base_address[ task -> stream_out -> end++ ] = *(string++);

			// character inserted
			length--;
		}

		// move end marker at beginning of stream content
		if( task -> stream_out -> end == (STD_STREAM_SIZE_page << STD_SHIFT_PAGE) || length ) task -> stream_out -> end = EMPTY;
	}

	// insert data from end marker to start marker
	while( length && task -> stream_out -> end != task -> stream_out -> start ) {
		task -> stream_out -> base_address[ task -> stream_out -> end++ ] = *(string++);

		// character inserted
		length--;
	}

	// string content is modified
	task -> stream_out -> flags |= KERNEL_STREAM_FLAG_modified;

	// unlock stream access
	task -> stream_out -> semaphore = UNLOCK;

	// string sended
	return TRUE;
}