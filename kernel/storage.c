/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct KERNEL_STRUCTURE_STORAGE *kernel_storage_add( uint8_t class ) {
	// lock exclusive access
	MACRO_LOCK( kernel -> storage_semaphore );

	// check for available storage entry
	for( uint64_t i = 0; i < KERNEL_STORAGE_limit; i++ ) {
		// entry available?
		if( kernel -> storage_base_address[ i ].device_class ) continue;	// no

		// mark entry as occupied
		kernel -> storage_base_address[ i ].device_class = class;

		// unlock access
		MACRO_UNLOCK( kernel -> storage_semaphore );

		// return pointer to device entry
		return (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ i ];
	}

	// unlock access
	MACRO_UNLOCK( kernel -> storage_semaphore );

	// no available entry
	return EMPTY;
}
