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
			if( ! kernel_qfs_identify( (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ i ] ) ) kernel_qfs_format( (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ i ] );

			// connect essential functions of QFS
			kernel -> storage_base_address[ i ].fs.root_directory_id = kernel -> storage_base_address[ i ].device_block;
			kernel -> storage_base_address[ i ].fs.dir = (void *) kernel_qfs_dir;
			kernel -> storage_base_address[ i ].fs.touch = (void *) kernel_qfs_touch;
			kernel -> storage_base_address[ i ].fs.open = (void *) kernel_qfs_open;
			kernel -> storage_base_address[ i ].fs.file = (void *) kernel_qfs_properties_by_socket;
			kernel -> storage_base_address[ i ].fs.close = (void *) kernel_qfs_close;
			// kernel -> storage_base_address[ i ].fs.write = (void *) kernel_qfs_write;
			// kernel -> storage_base_address[ i ].fs.read = (void *) kernel_qfs_read;

			// debug
			uint8_t file[] = "file.txt";
			uint8_t directory[] = "directory";
			kernel -> storage_base_address[ i ].fs.touch( (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ i ], (uint8_t *) &file, 8, STD_FILE_TYPE_file );
			kernel -> storage_base_address[ i ].fs.touch( (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ i ], (uint8_t *) &directory, 9, STD_FILE_TYPE_directory );

			// uint8_t text[] = "simple text line.";
			// struct KERNEL_STRUCTURE_VFS *socket = kernel -> storage_base_address[ i ].fs.open( (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ i ], (uint8_t *) &file, 8, EMPTY );
			// kernel -> storage_base_address[ i ].fs.write( socket, (uint8_t *) text, EMPTY, sizeof( text ) - 1 );

			// set file system type
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
