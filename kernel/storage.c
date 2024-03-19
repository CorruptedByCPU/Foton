/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct NEW_KERNEL_STORAGE_STRUCTURE *NEW_kernel_storage_register( uint8_t type ) {
	// lock exclusive access
	MACRO_LOCK( kernel -> NEW_storage_semaphore );

	// check for available storage entry
	for( uint64_t i = 0; i < NEW_KERNEL_STORAGE_limit; i++ )
		// available?
		if( ! kernel -> NEW_storage_base_address[ i ].device_type ) {
			// mark entry as ocupied
			kernel -> NEW_storage_base_address[ i ].device_type = type;

			// unlock access
			MACRO_UNLOCK( kernel -> NEW_storage_semaphore );

			// return pointer to device entry
			return (struct NEW_KERNEL_STORAGE_STRUCTURE *) &kernel -> NEW_storage_base_address[ i ];
		}

	// unlock access
	MACRO_UNLOCK( kernel -> NEW_storage_semaphore );

	// no available space
	return EMPTY;
}