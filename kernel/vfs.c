/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t kernel_vfs_identify( uintptr_t base_address, uint64_t byte ) {
	// file properties in chunks
	uint32_t *vfs = (uint32_t *) base_address;

	// at end of file, magic value exist?
	if( vfs[ (byte >> STD_SHIFT_4) - 1 ] == LIB_VFS_MAGIC ) return TRUE;	// yes

	// no
	return FALSE;
}

struct KERNEL_VFS_STRUCTURE_PROPERTIES kernel_vfs_file_properties( struct KERNEL_VFS_STRUCTURE *socket ) {
	// properties to return
	struct KERNEL_VFS_STRUCTURE_PROPERTIES properties = { EMPTY };

	// properties of file
	struct LIB_VFS_STRUCTURE *file = (struct LIB_VFS_STRUCTURE *) socket -> id;

	// retrun file size in Bytes
	properties.byte = file -> byte;

	// return file name
	properties.name_length = file -> name_length;
	for( uint64_t i = 0; i < file -> name_length; i++ ) properties.name[ i ] = file -> name[ i ];

	// return properties of file
	return properties;
}

struct KERNEL_VFS_STRUCTURE *kernel_vfs_file_open( uint8_t *path, uint64_t length, uint8_t mode ) {
	// remove all "white" characters from path
	length = lib_string_trim( path, length );

	// remove all SLASH characters from end of path
	while( path[ length - 1 ] == '/' ) length--;

	// if path is empty
	if( ! length ) return EMPTY;	// file not found

	// start from default directory
	struct LIB_VFS_STRUCTURE *vfs = (struct LIB_VFS_STRUCTURE *) kernel -> storage_base_address[ kernel -> storage_root ].root;

	// parse path
	while( TRUE ) {
		// start from current directory
		vfs = (struct LIB_VFS_STRUCTURE *) vfs -> offset;

		// remove leading '/', if exist
		while( *path == '/' ) { path++; length--; }

kernel -> log( (uint8_t *) "%s\n", path );

		// select file name from path
		uint64_t file_length = lib_string_word_end( path, length, '/' );

		// search file inside current directory
		do { if( file_length == vfs -> name_length && lib_string_compare( path, (uint8_t *) vfs -> name, file_length ) ) break;
		} while( (++vfs) -> name_length );

		// file found?
		if( ! vfs -> name_length ) return EMPTY;	// no

		// last file from path is requested one?
		if( length == file_length && lib_string_compare( path, (uint8_t *) vfs -> name, file_length ) ) {
			// follow symbolic links (if possible)
			while( vfs -> type & STD_FILE_TYPE_symbolic_link ) vfs = (struct LIB_VFS_STRUCTURE *) vfs -> offset;

			// open socket
			struct KERNEL_VFS_STRUCTURE *socket = (struct KERNEL_VFS_STRUCTURE *) &kernel -> vfs_base_address[ kernel_vfs_socket() ];

			// file located on definied storage
			socket -> storage = kernel -> storage_root;

			// type of operation on file
			socket -> mode = mode;

			// socket opened by process with ID
			socket -> pid = kernel -> task_pid();

			// file identificator
			socket -> id = (uint64_t) vfs;

			// file found
			return socket;
		}

		// follow symbolic links (if possible)
		while( vfs -> type & STD_FILE_TYPE_symbolic_link ) vfs = (struct LIB_VFS_STRUCTURE *) vfs -> offset;

		// if thats not a directory or symbolic link
		if( ! (vfs -> type & STD_FILE_TYPE_directory) ) return EMPTY;	// failed

		// remove parsed directory from path
		path += file_length; length -= file_length;
	}

	// file not found
	return EMPTY;
}

uint64_t kernel_vfs_socket( void ) {
	// block modification of vfs socket list by anyone else
	MACRO_LOCK( kernel -> vfs_semaphore );

	// return nereast socket
	uint64_t i = 0;

	// if entry found, secure it
	for( ; i < KERNEL_VFS_limit; i++ ) if( ! kernel -> vfs_base_address[ i ].mode ) { kernel -> vfs_base_address[ i ].mode = KERNEL_VFS_MODE_reserved; break; }

	// unlock access
	MACRO_UNLOCK( kernel -> vfs_semaphore );

	// return ID of storage
	return i;
}

void kernel_vfs_file_close( struct KERNEL_VFS_STRUCTURE *socket ) {
	// release socket
	socket -> mode = EMPTY;
}

void kernel_vfs_file_read( struct KERNEL_VFS_STRUCTURE *socket, uint8_t *target, uint64_t start_byte, uint64_t length_byte ) {
	// properties of file
	struct LIB_VFS_STRUCTURE *file = (struct LIB_VFS_STRUCTURE *) socket -> id;

	// invalid read request?
	if( start_byte + length_byte > file -> byte ) return;	// yes, ignore

	// copy content of file to destination
	uint8_t *source = (uint8_t *) file -> offset;
	for( uint64_t i = start_byte; i < start_byte + length_byte; i++ ) target[ i ] = source[ i ];
}

// int64_t kernel_vfs_old_write( struct LIB_VFS_STRUCTURE *vfs, uintptr_t source_address, uint64_t length_byte ) {
// 	// current file allocation is enough?
// 	if( MACRO_PAGE_ALIGN_UP( vfs -> size ) >= length_byte ) {
// 		// copy content of file to destination
// 		uint8_t *source = (uint8_t *) source_address;
// 		uint8_t *target = (uint8_t *) vfs -> offset;
// 		for( uint64_t i = 0; i < length_byte; i++ ) target[ i ] = source[ i ];

// 		// preserve new file length in Bytes
// 		vfs -> size = length_byte;

// 		// done
// 		return EMPTY;
// 	}

// 	// assign new file area
// 	uint8_t *target = (uint8_t *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( length_byte ) >> STD_SHIFT_PAGE );

// 	// area assigned?
// 	if( ! target ) return STD_ERROR_memory_low;

// 	// copy content of file to destination
// 	uint8_t *source = (uint8_t *) source_address;
// 	for( uint64_t i = 0; i < length_byte; i++ ) target[ i ] = source[ i ];

// 	// release old file content
// 	kernel -> memory_release( vfs -> offset, MACRO_PAGE_ALIGN_UP( vfs -> size ) >> STD_SHIFT_PAGE );

// 	// preserve new file length and content area
// 	vfs -> offset = (uintptr_t) target;
// 	vfs -> size = length_byte;

// 	// done
// 	return EMPTY;
// }