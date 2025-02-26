/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_storage( void ) {
	// never ending story
	while( TRUE ) {
		// check for unknown storage file system
		for( uint64_t i = 0; i < KERNEL_STORAGE_limit; i++ ) {
			// storage active?
			if( ! (kernel -> storage_base_address[ i ].flags & KERNEL_STORAGE_FLAGS_active) ) continue;	// no

			// file system already definied?
			if( kernel -> storage_base_address[ i ].device_fs ) continue;	// yes

			// try to recognize file system
			if( ! kernel_qfs_identify( i ) ) kernel_qfs_format( i );

			// debug
			kernel -> storage_base_address[ i ].device_fs = KERNEL_STORAGE_FS_qfs;	// QuarkFS
		}

		// release ap time
		kernel -> time_sleep( TRUE );
	}
}

uint64_t kernel_storage_add( void ) {
	// lock exclusive access
	MACRO_LOCK( kernel -> storage_semaphore );

	// check for available storage entry
	for( uint64_t i = 0; i < KERNEL_STORAGE_limit; i++ ) {
		// entry available?
		if( kernel -> storage_base_address[ i ].flags ) continue;	// no

		// mark entry as occupied
		kernel -> storage_base_address[ i ].flags = KERNEL_STORAGE_FLAGS_reserved;

		// unlock access
		MACRO_UNLOCK( kernel -> storage_semaphore );

		// return pointer to device entry
		return i;
	}

	// unlock access
	MACRO_UNLOCK( kernel -> storage_semaphore );

	// no available entry
	return EMPTY;
}
