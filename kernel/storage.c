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

void kernel_storage_file( struct STD_FILE_STRUCTURE *file ) {
	// file not found
	file -> id = EMPTY;

	// different approach, regarded of device type
	switch( kernel -> storage_base_address[ file -> id_storage ].device_type ) {
		case KERNEL_STORAGE_TYPE_vfs: {
			// properties of root Virtual File System
			struct LIB_VFS_STRUCTURE *vfs = (struct LIB_VFS_STRUCTURE *) kernel -> storage_base_address[ file -> id_storage ].device_block;

			// search from current task directory?
			if( file -> name[ 0 ] != '/' ) {
				// properties of task
				struct KERNEL_TASK_STRUCTURE *task = kernel_task_active();

				// choose task directory
				vfs = task -> directory;
			}

			// return properties of file
			lib_vfs_file( vfs, file );

			// substitute of superblock
			// ------------------------
			// if returned properties are about symbolic link of "." or ".."
			if( lib_string_compare( (uint8_t *) ".", file -> name, file -> length ) || lib_string_compare( (uint8_t *) "..", file -> name, file -> length ) )
				// change file type to DIRECTORY
				file -> type = STD_FILE_TYPE_directory;
		}
	}
};

void kernel_storage_read( struct STD_FILE_STRUCTURE *file, uintptr_t target_address ) {
	// different approach, regarded of device type
	switch( kernel -> storage_base_address[ file -> id_storage ].device_type ) {
		case KERNEL_STORAGE_TYPE_vfs:
			// return properties of file
			return lib_vfs_read( (struct LIB_VFS_STRUCTURE *) file -> id, target_address );
	}
};