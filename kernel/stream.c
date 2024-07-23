/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct KERNEL_STREAM_STRUCTURE *kernel_stream( void ) {
	// deny access, only one logical processor at a time
	MACRO_LOCK( kernel -> stream_semaphore );

	// search stream table for free entry
	for( uint64_t i = 0; i < KERNEL_STREAM_limit; i++ ) {
		// free entry?
		if( kernel -> stream_base_address[ i ].base_address ) continue;	// no

		// prepare area for stream content
		kernel -> stream_base_address[ i ].base_address	= (uint8_t *) kernel_memory_alloc( STD_STREAM_SIZE_page );

		// there is currently no data inside stream
		kernel -> stream_base_address[ i ].length_byte	= EMPTY;

		// start and end mark cleared
		kernel -> stream_base_address[ i ].start	= EMPTY;
		kernel -> stream_base_address[ i ].end		= EMPTY;		

		// number of processes assigned to stream
		kernel -> stream_base_address[ i ].lock		= TRUE;

		// unlock access to function
		MACRO_UNLOCK( kernel -> stream_semaphore );

		// return stream id
		return (struct KERNEL_STREAM_STRUCTURE *) &kernel -> stream_base_address[ i ];
	}

	// unlock access to function
	MACRO_UNLOCK( kernel -> stream_semaphore );

	// no free entry
	return	EMPTY;
}

void kernel_stream_release( struct KERNEL_STREAM_STRUCTURE *stream ) {
	// it was last process for that stream?
	if( ! --stream -> lock ) {
		// free up stream space
		kernel_memory_release( (uintptr_t) stream -> base_address, STD_STREAM_SIZE_page );

		// free entry
		stream -> base_address = EMPTY;
	}
}