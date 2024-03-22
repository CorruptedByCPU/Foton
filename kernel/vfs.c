/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_vfs_file_close( struct KERNEL_VFS_STRUCTURE *socket ) {
	// can we close file?
	if( socket -> pid != kernel_task_pid() ) return;	// no! TODO: something nasty

	// release socket
	if( socket -> lock ) socket -> lock--;
}

struct KERNEL_VFS_STRUCTURE *kernel_vfs_file_open( uint8_t *path, uint64_t length ) {
	// remove all "white" characters from path
	length = lib_string_trim( path, length );

	// properties of current directory
	struct LIB_VFS_STRUCTURE *vfs;

	// start from current directory?
	if( *path != STD_ASCII_SLASH ) {
		// properties of task
		struct KERNEL_TASK_STRUCTURE *task = kernel_task_active();
	
		// choose task current directory
		vfs = (struct LIB_VFS_STRUCTURE *) task -> directory;
	} else
		// start from default directory
		vfs = (struct LIB_VFS_STRUCTURE *) kernel -> storage_base_address[ kernel -> storage_root ].device_block;

	// remove all SLASH characters from end of path
	while( path[ length - 1 ] == '/' ) length--;

	// if path is empty
	if( ! length ) {
		// open socket
		struct KERNEL_VFS_STRUCTURE *socket = (struct KERNEL_VFS_STRUCTURE *) &kernel -> vfs_base_address[ kernel_vfs_socket_add() ];

		// file located on definied storage
		socket -> storage = kernel -> storage_root;

		// file identificator
		socket -> knot = (uint64_t) vfs;

		// socket opened by process with ID
		socket -> pid = kernel -> task_pid();

		// file found
		return socket;
	}

	// parse path
	while( TRUE ) {
		// start from current directory
		vfs = (struct LIB_VFS_STRUCTURE *) vfs -> offset;

		// remove leading '/', if exist
		while( *path == '/' ) { path++; length--; }

		// select file name from path
		uint64_t file_length = lib_string_word_end( path, length, '/' );

		// search file inside current directory
		do { if( file_length == vfs -> name_length && lib_string_compare( path, (uint8_t *) vfs -> name, file_length ) ) break;
		} while( (++vfs) -> name_length );

		// file found?
		if( ! vfs -> name_length ) return EMPTY;	// no

		// last file from path and requested one?
		if( length == file_length ) {
			// follow symbolic links (if possible)
			while( vfs -> type & STD_FILE_TYPE_link ) vfs = (struct LIB_VFS_STRUCTURE *) vfs -> offset;

			// open socket
			struct KERNEL_VFS_STRUCTURE *socket = (struct KERNEL_VFS_STRUCTURE *) &kernel -> vfs_base_address[ kernel_vfs_socket_add() ];

			// file located on definied storage
			socket -> storage = kernel -> storage_root;

			// file identificator
			socket -> knot = (uint64_t) vfs;

			// socket opened by process with ID
			socket -> pid = kernel -> task_pid();

			// file found
			return socket;
		}

		// follow symbolic links (if possible)
		while( vfs -> type & STD_FILE_TYPE_link ) vfs = (struct LIB_VFS_STRUCTURE *) vfs -> offset;

		// if thats not a directory
		if( ! (vfs -> type & STD_FILE_TYPE_directory) ) return EMPTY;	// failed

		// remove parsed directory from path
		path += file_length; length -= file_length;
	}

	// file not found
	return EMPTY;
}

void kernel_vfs_file_properties( struct KERNEL_VFS_STRUCTURE *socket, struct KERNEL_VFS_STRUCTURE_PROPERTIES *properties ) {
	// properties of file
	struct LIB_VFS_STRUCTURE *file = (struct LIB_VFS_STRUCTURE *) socket -> knot;

	// retrun file size in Bytes
	properties -> byte = file -> byte;

	// return file name
	properties -> name_length = file -> name_length;
	for( uint64_t i = 0; i < file -> name_length; i++ ) properties -> name[ i ] = file -> name[ i ];
}

void kernel_vfs_file_read( struct KERNEL_VFS_STRUCTURE *socket, uint8_t *target, uint64_t seek, uint64_t byte ) {
	// properties of file
	struct LIB_VFS_STRUCTURE *file = (struct LIB_VFS_STRUCTURE *) socket -> knot;

	// invalid read request?
	if( seek + byte > file -> byte ) return;	// yes, ignore

	// copy content of file to destination
	uint8_t *source = (uint8_t *) file -> offset + seek;
	for( uint64_t i = 0; i < byte; i++ ) target[ i ] = source[ i ];
}

void kernel_vfs_file_write( struct KERNEL_VFS_STRUCTURE *socket, uint8_t *source, uint64_t seek, uint64_t byte ) {
	// lock exclusive access
	MACRO_LOCK( socket -> semaphore );

	// properties of file
	struct LIB_VFS_STRUCTURE *file = (struct LIB_VFS_STRUCTURE *) socket -> knot;

	// invalid write request?
	if( seek + byte > MACRO_PAGE_ALIGN_UP( file -> byte ) ) {
		// prepare new file content area
		uint8_t *new = (uint8_t *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( seek + byte ) >> STD_SHIFT_PAGE );
		uint8_t *old = (uint8_t *) file -> offset;

		// copy current content to new location only if current file content will not be truncated
		if( ! seek ) for( uint64_t i = 0; i < file -> byte; i++ ) new[ i ] = old[ i ];

		// release old file content
		kernel -> memory_release( file -> offset, MACRO_PAGE_ALIGN_UP( file -> byte ) >> STD_SHIFT_PAGE );

		// update file properties with new content location
		file -> offset = (uintptr_t) new;
	}

	// INFO: writing to file from seek == EMPTY, means the same as create new content

	// copy content of memory to file
	uint8_t *target = (uint8_t *) file -> offset + seek;
	for( uint64_t i = 0; i < byte; i++ ) target[ i ] = source[ i ];

	// truncate file size?
	if( ! seek && MACRO_PAGE_ALIGN_UP( file -> byte ) > MACRO_PAGE_ALIGN_UP( byte ) ) {
		// locate amount of file block to truncate
		uint64_t blocks = (MACRO_PAGE_ALIGN_UP( file -> byte ) - MACRO_PAGE_ALIGN_UP( byte )) >> STD_SHIFT_PAGE;

		// remove unused blocks from file
		kernel -> memory_release( file -> offset + (MACRO_PAGE_ALIGN_UP( file -> byte ) - (blocks << STD_SHIFT_PAGE)), blocks );

		// new file size
		file -> byte = byte;
	} else
		// file size will change after overwrite?
		if( seek + byte > file -> byte ) file -> byte = seek + byte;	// yes
	
	// unlock access
	MACRO_UNLOCK( socket -> semaphore );
}

uint8_t	kernel_vfs_identify( uintptr_t base_address, uint64_t limit_byte ) {
	// file properties in chunks
	uint32_t *vfs = (uint32_t *) base_address;

	// at end of file, magic value exist?
	if( vfs[ (limit_byte >> STD_SHIFT_4) - 1 ] == LIB_VFS_magic ) return TRUE;	// yes

	// no
	return FALSE;
}

uint64_t kernel_vfs_socket_add( void ) {
	// lock exclusive access
	MACRO_LOCK( kernel -> vfs_semaphore );

	// return nereast socket
	uint64_t i = 0;

	// if entry found, secure it
	for( ; i < KERNEL_VFS_limit; i++ ) if( ! kernel -> vfs_base_address[ i ].lock ) { kernel -> vfs_base_address[ i ].lock++; break; }

	// unlock access
	MACRO_UNLOCK( kernel -> vfs_semaphore );

	// return ID of socket
	return i;
}