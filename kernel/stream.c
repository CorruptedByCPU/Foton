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
