/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct KERNEL_STRUCTURE_STORAGE *kernel_storage_create( void ) {
	// lock exclusive access
	MACRO_LOCK( kernel -> storage_lock );

	// search through storage list
	for( uint64_t i = INIT; i < kernel -> storage_limit; i++ ) {
		// available entry?
		if( kernel -> storage_base_address[ i ].flags ) continue;	// no	// no

		// mark entry
		kernel -> storage_base_address[ i ].flags = KERNEL_STORAGE_FLAGS_secure;

		// unlock access
		MACRO_UNLOCK( kernel -> storage_lock );

		// storage properties
		return (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ i ];
	}

	// unlock access
	MACRO_UNLOCK( kernel -> storage_lock );

	// no available entry
	return (struct KERNEL_STRUCTURE_STORAGE *) EMPTY;
}
