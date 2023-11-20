/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#ifndef	LIB_VFS
		#include	"./vfs.h"
	#endif

uint8_t lib_vfs_check( uintptr_t address, uint64_t size_byte ) {
	// properties of file
	uint32_t *vfs = (uint32_t *) address;

	// magic value?
	if( vfs[ (size_byte >> STD_SHIFT_4) - 1 ] == LIB_VFS_magic ) return TRUE;	// yes

	// no
	return FALSE;
}

void lib_vfs_file( struct LIB_VFS_STRUCTURE *vfs, struct STD_FILE_STRUCTURE *file ) {
	// path name index
	uint64_t i = 0;

	// path name length
	uint64_t length = file -> length;

	// start from root directory?
	if( file -> name[ i ] == '/' )
		// find root directory structure
		while( vfs[ 0 ].offset != vfs[ 1 ].offset ) vfs = (struct LIB_VFS_STRUCTURE *) vfs[ 1 ].offset;

	// remove all '/' from end of path
	while( file -> name[ length - 1 ] == '/' ) length--;

	// parse path
	while( TRUE ) {
		// remove leading '/'
		while( file -> name[ i ] == '/' ) { i++; length--; };

		// first file name
		uint64_t filename_length = lib_string_word_end( (uint8_t *) &file -> name[ i ], length, '/' );

		// select file from current directory structure
		do { if( vfs -> length == filename_length && lib_string_compare( (uint8_t *) &file -> name[ i ], (uint8_t *) vfs -> name, filename_length ) ) break;
		} while( (++vfs) -> length );

		// file found?
		if( ! vfs -> length ) return;	// no

		// last file from path is requested one?
		if( length == filename_length && lib_string_compare( (uint8_t *) &file -> name[ i ], (uint8_t *) vfs -> name, filename_length ) ) {
			// set file properties
			file -> id = (uint64_t) vfs;		// file identificator / pointer to content
			file -> length_byte = vfs -> size;	// file size in Bytes
			file -> type = vfs -> type;		// file type

			// file found
			return;
		}

		// change directory
		vfs = (struct LIB_VFS_STRUCTURE *) vfs -> offset;

		// remove parsed directory from path
		i += filename_length;
		length -= filename_length;
	}
}

void lib_vfs_read( struct LIB_VFS_STRUCTURE *vfs, uintptr_t target_address ) {
	// copy content of file to destination
	uint8_t *source = (uint8_t *) vfs -> offset;
	uint8_t *target = (uint8_t *) target_address;
	for( uint64_t i = 0; i < vfs -> size; i++ ) target[ i ] = source[ i ];
}