/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct KERNEL_STORAGE_STRUCTURE *kernel_storage_register( uint8_t type ) {
	// block modification of storage list by anyone else
	while( __sync_val_compare_and_swap( &kernel -> storage_semaphore, UNLOCK, LOCK ) );

	// find an empty entry
	for( uint64_t i = 0; i < KERNEL_STORAGE_limit; i++ )
		// empty entry?
		if( ! kernel -> storage_base_address[ i ].device_type ) {
			// mark slot as used
			kernel -> storage_base_address[ i ].device_type = type;

			// unlock access
			kernel -> storage_semaphore = UNLOCK;

			// return pointer to device slot
			return (struct KERNEL_STORAGE_STRUCTURE *) &kernel -> storage_base_address[ i ];
		}

	// unlock access
	kernel -> storage_semaphore = UNLOCK;

	// no available space
	return EMPTY;
}