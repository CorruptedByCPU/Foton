/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint64_t kernel_storage_register( uint8_t type ) {
	// block modification of storage list by anyone else
	MACRO_LOCK( kernel -> storage_semaphore );

	// keep storages as stack
	uint64_t i = KERNEL_STORAGE_limit;

	// if entry found, secure it
	while( --i ) if( ! kernel -> storage_base_address[ i ].type ) { kernel -> storage_base_address[ i ].type = type; break; }

	// unlock access
	MACRO_UNLOCK( kernel -> storage_semaphore );

	// return ID of storage
	return i;
}

// // OLD ========================================================================

struct KERNEL_STORAGE_OLD_STRUCTURE *kernel_storage_old_register( uint8_t type ) {
// 	// block modification of storage list by anyone else
// 	MACRO_LOCK( kernel -> storage_semaphore );

// 	// find an empty entry
// 	for( uint64_t i = 0; i < KERNEL_STORAGE_OLD_limit; i++ )
// 		// empty entry?
// 		if( ! kernel -> storage_old_base_address[ i ].device_type ) {
// 			// mark slot as used
// 			kernel -> storage_old_base_address[ i ].device_type = type;

// 			// unlock access
// 			MACRO_UNLOCK( kernel -> storage_semaphore );

// 			// return pointer to device slot
// 			return (struct KERNEL_STORAGE_OLD_STRUCTURE *) &kernel -> storage_old_base_address[ i ];
// 		}

// 	// unlock access
// 	MACRO_UNLOCK( kernel -> storage_semaphore );

	// no available space
	return EMPTY;
}

void kernel_storage_old_file( struct STD_FILE_OLD_STRUCTURE *file ) {
// 	// file not found
// 	file -> id = EMPTY;

// 	// different approach, regarded of device type
// 	switch( kernel -> storage_old_base_address[ file -> id_storage ].device_type ) {
// 		case KERNEL_STORAGE_OLD_TYPE_vfs: {
// 			// properties of root Virtual File System
// 			struct LIB_VFS_STRUCTURE *vfs = (struct LIB_VFS_STRUCTURE *) kernel -> storage_old_base_address[ file -> id_storage ].device_block;

// 			// search from root directory?
// 			if( file -> name[ 0 ] != '/' ) {
// 				// properties of task
// 				struct KERNEL_TASK_STRUCTURE *task = kernel_task_active();

// 				// choose task current directory
// 				vfs = (struct LIB_VFS_STRUCTURE *) task -> directory;
// 			}

// 			// return properties of file
// 			kernel_vfs_old_file( vfs, file );
// 		}
// 	}
};

void kernel_storage_old_read( struct STD_FILE_OLD_STRUCTURE *file, uintptr_t target_address ) {
// 	// different approach, regarded of device type
// 	switch( kernel -> storage_old_base_address[ file -> id_storage ].device_type ) {
// 		case KERNEL_STORAGE_OLD_TYPE_vfs: {
// 			// read file content
// 			kernel_vfs_old_read( (struct LIB_VFS_STRUCTURE *) file -> id, target_address );

// 			// done
// 			break;
// 		}
// 	}
};

void kernel_storage_old_write( struct STD_FILE_OLD_STRUCTURE *file, uintptr_t source_address, uint64_t byte ) {
// 	// different approach, regarded of device type
// 	switch( kernel -> storage_old_base_address[ file -> id_storage ].device_type ) {
// 		case KERNEL_STORAGE_OLD_TYPE_vfs: {
// 			// write new file content
// 			kernel_vfs_old_write( (struct LIB_VFS_STRUCTURE *) file -> id, source_address, byte );

// 			// done
// 			break;
// 		}
// 	}
};