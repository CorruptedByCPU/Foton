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

struct KERNEL_VFS_STRUCTURE *kernel_vfs_file_open( uint8_t *path, uint64_t length ) {
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
// 		// remove leading '/'
// 		while( file -> name[ i ] == '/' ) { i++; length--; };

// 		// first file name
// 		uint64_t filename_length = lib_string_word_end( (uint8_t *) &file -> name[ i ], length, '/' );

// 		// select file from current directory structure
// 		do { if( vfs -> length == filename_length && lib_string_compare( (uint8_t *) &file -> name[ i ], (uint8_t *) vfs -> name, filename_length ) ) break;
// 		} while( (++vfs) -> length );

// 		// file found?
// 		if( ! vfs -> length ) return;	// no

// 		// last file from path is requested one?
// 		if( length == filename_length && lib_string_compare( (uint8_t *) &file -> name[ i ], (uint8_t *) vfs -> name, filename_length ) ) {
// 			// symbolic link selected?
// 			while( vfs -> type & STD_FILE_TYPE_symbolic_link ) vfs = (struct LIB_VFS_STRUCTURE *) vfs -> offset;

// 			// set file properties
// 			file -> id = (uint64_t) vfs;		// file identificator / pointer to content
// 			file -> length_byte = vfs -> size;	// file size in Bytes
// 			file -> type = vfs -> type;		// file type

// 			// update name of selected file
// 			file -> length = vfs -> length;
// 			for( uint64_t j = 0; j < file -> length; j++ ) file -> name[ j ] = vfs -> name[ j ]; file -> name[ file -> length ] = STD_ASCII_TERMINATOR;

// 			// file found
// 			return;
// 		}

// 		// symbolic link selected?
// 		while( vfs -> type & STD_FILE_TYPE_symbolic_link ) vfs = (struct LIB_VFS_STRUCTURE *) vfs -> offset;

// 		// if thats not a directory or symbolic link
// 		if( ! (vfs -> type & STD_FILE_TYPE_directory) ) return;	// failed

// 		// remove parsed directory from path
// 		i += filename_length;
// 		length -= filename_length;
	}


	// file not found
	return EMPTY;
}

uint64_t kernel_vfs_register( void ) {
	// block modification of vfs socket list by anyone else
	MACRO_LOCK( kernel -> vfs_semaphore );

	// return nereast socket
	uint64_t i = 0;

	// if entry found, secure it
	while( i < KERNEL_VFS_limit ) if( ! kernel -> vfs_base_address[ i++ ].flags ) { kernel -> vfs_base_address[ i ].flags = KERNEL_VFS_FLAG_reserved; break; }

	// unlock access
	MACRO_UNLOCK( kernel -> storage_semaphore );

	// return ID of storage
	return i;
}

void kernel_vfs_file_close( struct KERNEL_VFS_STRUCTURE *socket ) {

}

// }

// void kernel_vfs_old_read( struct LIB_VFS_STRUCTURE *vfs, uintptr_t target_address ) {
// 	// copy content of file to destination
// 	uint8_t *source = (uint8_t *) vfs -> offset;
// 	uint8_t *target = (uint8_t *) target_address;
// 	for( uint64_t i = 0; i < vfs -> size; i++ ) target[ i ] = source[ i ];
// }

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