/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void NEW_kernel_vfs_file_close( struct NEW_KERNEL_VFS_STRUCTURE *socket ) {
	// release socket
	socket -> mode = EMPTY;
}

struct NEW_KERNEL_VFS_STRUCTURE *NEW_kernel_vfs_file_open( uint8_t *path, uint64_t length, uint8_t mode ) {
	// remove all "white" characters from path
	length = lib_string_trim( path, length );

	// remove all SLASH characters from end of path
	while( path[ length - 1 ] == '/' ) length--;

	// if path is empty
	if( ! length ) return EMPTY;	// file not found

	// start from default directory
	struct EXCHANGE_LIB_VFS_STRUCTURE *vfs = (struct EXCHANGE_LIB_VFS_STRUCTURE *) kernel -> DEPRECATED_storage_base_address[ kernel -> NEW_storage_root ].device_block;

	// parse path
	while( TRUE ) {
		// start from current directory
		vfs = (struct EXCHANGE_LIB_VFS_STRUCTURE *) vfs -> offset;

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
			while( vfs -> type & NEW_STD_FILE_TYPE_link ) vfs = (struct EXCHANGE_LIB_VFS_STRUCTURE *) vfs -> offset;

			// open socket
			struct NEW_KERNEL_VFS_STRUCTURE *socket = (struct NEW_KERNEL_VFS_STRUCTURE *) &kernel -> NEW_vfs_base_address[ NEW_kernel_vfs_socket_add() ];

			// file located on definied storage
			socket -> storage = kernel -> NEW_storage_root;

			// file identificator
			socket -> knot = (uint64_t) vfs;

			// socket opened by process with ID
			socket -> pid = kernel -> task_pid();

			// type of operation on file
			socket -> mode = mode;

			// file found
			return socket;
		}

		// follow symbolic links (if possible)
		while( vfs -> type & NEW_STD_FILE_TYPE_link ) vfs = (struct EXCHANGE_LIB_VFS_STRUCTURE *) vfs -> offset;

		// if thats not a directory
		if( ! (vfs -> type & NEW_STD_FILE_TYPE_directory) ) return EMPTY;	// failed

		// remove parsed directory from path
		path += file_length; length -= file_length;
	}

	// file not found
	return EMPTY;
}

void NEW_kernel_vfs_file_properties( struct NEW_KERNEL_VFS_STRUCTURE *socket, struct NEW_KERNEL_VFS_STRUCTURE_PROPERTIES *properties ) {
	// properties of file
	struct EXCHANGE_LIB_VFS_STRUCTURE *file = (struct EXCHANGE_LIB_VFS_STRUCTURE *) socket -> knot;

	// retrun file size in Bytes
	properties -> byte = file -> byte;

	// return file name
	properties -> name_length = file -> name_length;
	for( uint64_t i = 0; i < file -> name_length; i++ ) properties -> name[ i ] = file -> name[ i ];
}

void NEW_kernel_vfs_file_read( struct NEW_KERNEL_VFS_STRUCTURE *socket, uint8_t *target, uint64_t seek, uint64_t length_byte ) {
	// properties of file
	struct EXCHANGE_LIB_VFS_STRUCTURE *file = (struct EXCHANGE_LIB_VFS_STRUCTURE *) socket -> knot;

	// invalid read request?
	if( seek + length_byte > file -> byte ) return;	// yes, ignore

	// copy content of file to destination
	uint8_t *source = (uint8_t *) file -> offset;
	for( uint64_t i = seek; i < seek + length_byte; i++ ) target[ i ] = source[ i ];
}

uint8_t	NEW_kernel_vfs_identify( uintptr_t base_address, uint64_t limit_byte ) {
	// file properties in chunks
	uint32_t *vfs = (uint32_t *) base_address;

	// at end of file, magic value exist?
	if( vfs[ (limit_byte >> STD_SHIFT_4) - 1 ] == NEW_LIB_VFS_magic ) return TRUE;	// yes

	// no
	return FALSE;
}

uint64_t NEW_kernel_vfs_socket_add( void ) {
	// lock exclusive access
	MACRO_LOCK( kernel -> NEW_vfs_semaphore );

	// return nereast socket
	uint64_t i = 0;

	// if entry found, secure it
	for( ; i < NEW_KERNEL_VFS_limit; i++ ) if( ! kernel -> NEW_vfs_base_address[ i ].mode ) { kernel -> NEW_vfs_base_address[ i ].mode = NEW_KERNEL_VFS_MODE_reserved; break; }

	// unlock access
	MACRO_UNLOCK( kernel -> NEW_vfs_semaphore );

	// return ID of socket
	return i;
}

// OLD ========================================================================

uint8_t DEPRECATED_kernel_vfs_check( uintptr_t address, uint64_t size_byte ) {
	// properties of file
	uint32_t *vfs = (uint32_t *) address;

	// magic value?
	if( vfs[ (size_byte >> STD_SHIFT_4) - 1 ] == DEPRECATED_KERNEL_VFS_magic ) return TRUE;	// yes

	// no
	return FALSE;
}

void DEPRECATED_kernel_vfs_file( struct EXCHANGE_LIB_VFS_STRUCTURE *vfs, struct DEPRECATED_STD_FILE_STRUCTURE *file ) {
	// path name index
	uint64_t i = 0;

	// path name length
	uint64_t length = file -> length;

	// remove all '/' from end of path
	while( file -> name[ length - 1 ] == '/' ) length--;

	// empty path?
	if( ! length ) {
		// return root directory properties

		// set file properties
		file -> id = (uint64_t) vfs;		// file identificator / pointer to content
		file -> length_byte = vfs -> byte;	// file size in Bytes
		file -> type = vfs -> type;		// file type

		// file found
		return;
	}

	// parse path
	while( TRUE ) {
		// start from current directory
		vfs = (struct EXCHANGE_LIB_VFS_STRUCTURE *) vfs -> offset;

		// remove leading '/'
		while( file -> name[ i ] == '/' ) { i++; length--; };

		// first file name
		uint64_t filename_length = lib_string_word_end( (uint8_t *) &file -> name[ i ], length, '/' );

		// select file from current directory structure
		do { if( vfs -> name_length == filename_length && lib_string_compare( (uint8_t *) &file -> name[ i ], (uint8_t *) vfs -> name, filename_length ) ) break;
		} while( (++vfs) -> name_length );

		// file found?
		if( ! vfs -> name_length ) return;	// no

		// last file from path is requested one?
		if( length == filename_length && lib_string_compare( (uint8_t *) &file -> name[ i ], (uint8_t *) vfs -> name, filename_length ) ) {
			// symbolic link selected?
			while( vfs -> type & DEPRECATED_STD_FILE_TYPE_symbolic_link ) vfs = (struct EXCHANGE_LIB_VFS_STRUCTURE *) vfs -> offset;

			// set file properties
			file -> id = (uint64_t) vfs;		// file identificator / pointer to content
			file -> length_byte = vfs -> byte;	// file size in Bytes
			file -> type = vfs -> type;		// file type

			// update name of selected file
			file -> length = vfs -> name_length;
			for( uint64_t j = 0; j < file -> length; j++ ) file -> name[ j ] = vfs -> name[ j ]; file -> name[ file -> length ] = STD_ASCII_TERMINATOR;

			// file found
			return;
		}

		// symbolic link selected?
		while( vfs -> type & DEPRECATED_STD_FILE_TYPE_symbolic_link ) vfs = (struct EXCHANGE_LIB_VFS_STRUCTURE *) vfs -> offset;

		// if thats not a directory or symbolic link
		if( ! (vfs -> type & DEPRECATED_STD_FILE_TYPE_directory) ) return;	// failed

		// remove parsed directory from path
		i += filename_length;
		length -= filename_length;
	}
}

void DEPRECATED_kernel_vfs_read( struct EXCHANGE_LIB_VFS_STRUCTURE *vfs, uintptr_t target_address ) {
	// copy content of file to destination
	uint8_t *source = (uint8_t *) vfs -> offset;
	uint8_t *target = (uint8_t *) target_address;
	for( uint64_t i = 0; i < vfs -> byte; i++ ) target[ i ] = source[ i ];
}

int64_t DEPRECATED_kernel_vfs_write( struct EXCHANGE_LIB_VFS_STRUCTURE *vfs, uintptr_t source_address, uint64_t length_byte ) {
	// current file allocation is enough?
	if( MACRO_PAGE_ALIGN_UP( vfs -> byte ) >= length_byte ) {
		// copy content of file to destination
		uint8_t *source = (uint8_t *) source_address;
		uint8_t *target = (uint8_t *) vfs -> offset;
		for( uint64_t i = 0; i < length_byte; i++ ) target[ i ] = source[ i ];

		// preserve new file length in Bytes
		vfs -> byte = length_byte;

		// done
		return EMPTY;
	}

	// assign new file area
	uint8_t *target = (uint8_t *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( length_byte ) >> STD_SHIFT_PAGE );

	// area assigned?
	if( ! target ) return STD_ERROR_memory_low;

	// copy content of file to destination
	uint8_t *source = (uint8_t *) source_address;
	for( uint64_t i = 0; i < length_byte; i++ ) target[ i ] = source[ i ];

	// release old file content
	kernel -> memory_release( vfs -> offset, MACRO_PAGE_ALIGN_UP( vfs -> byte ) >> STD_SHIFT_PAGE );

	// preserve new file length and content area
	vfs -> offset = (uintptr_t) target;
	vfs -> byte = length_byte;

	// done
	return EMPTY;
}