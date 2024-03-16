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

// OLD ========================================================================

struct DEPRECATED_KERNEL_STORAGE_STRUCTURE *DEPRECATED_kernel_storage_register( uint8_t type ) {
	// block modification of storage list by anyone else
	MACRO_LOCK( kernel -> DEPRECATED_storage_semaphore );

	// find an empty entry
	for( uint64_t i = 0; i < DEPRECATED_KERNEL_STORAGE_limit; i++ )
		// empty entry?
		if( ! kernel -> DEPRECATED_storage_base_address[ i ].device_type ) {
			// mark slot as used
			kernel -> DEPRECATED_storage_base_address[ i ].device_type = type;

			// unlock access
			MACRO_UNLOCK( kernel -> DEPRECATED_storage_semaphore );

			// return pointer to device slot
			return (struct DEPRECATED_KERNEL_STORAGE_STRUCTURE *) &kernel -> DEPRECATED_storage_base_address[ i ];
		}

	// unlock access
	MACRO_UNLOCK( kernel -> DEPRECATED_storage_semaphore );

	// no available space
	return EMPTY;
}

void DEPRECATED_kernel_storage_file( struct DEPRECATED_STD_FILE_STRUCTURE *file ) {
	// file not found
	file -> id = EMPTY;

	// different approach, regarded of device type
	switch( kernel -> DEPRECATED_storage_base_address[ file -> id_storage ].device_type ) {
		case DEPRECATED_KERNEL_STORAGE_TYPE_vfs: {
			// properties of root Virtual File System
			struct EXCHANGE_LIB_VFS_STRUCTURE *vfs = (struct EXCHANGE_LIB_VFS_STRUCTURE *) kernel -> DEPRECATED_storage_base_address[ file -> id_storage ].device_block;

			// search from root directory?
			if( file -> name[ 0 ] != '/' ) {
				// properties of task
				struct KERNEL_TASK_STRUCTURE *task = kernel_task_active();

				// choose task current directory
				vfs = (struct EXCHANGE_LIB_VFS_STRUCTURE *) task -> directory;
			}

			// return properties of file
			DEPRECATED_kernel_vfs_file( vfs, file );
		}
	}
};

void DEPRECATED_kernel_storage_read( struct DEPRECATED_STD_FILE_STRUCTURE *file, uintptr_t target_address ) {
	// different approach, regarded of device type
	switch( kernel -> DEPRECATED_storage_base_address[ file -> id_storage ].device_type ) {
		case DEPRECATED_KERNEL_STORAGE_TYPE_vfs: {
			// read file content
			DEPRECATED_kernel_vfs_read( (struct EXCHANGE_LIB_VFS_STRUCTURE *) file -> id, target_address );

			// done
			break;
		}
	}
};

void DEPRECATED_kernel_storage_write( struct DEPRECATED_STD_FILE_STRUCTURE *file, uintptr_t source_address, uint64_t byte ) {
	// different approach, regarded of device type
	switch( kernel -> DEPRECATED_storage_base_address[ file -> id_storage ].device_type ) {
		case DEPRECATED_KERNEL_STORAGE_TYPE_vfs: {
			// write new file content
			DEPRECATED_kernel_vfs_write( (struct EXCHANGE_LIB_VFS_STRUCTURE *) file -> id, source_address, byte );

			// done
			break;
		}
	}
};