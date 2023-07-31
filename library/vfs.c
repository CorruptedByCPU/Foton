/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#ifndef	LIB_VFS
		#include	"vfs.h"
	#endif
	#ifndef	LIB_STRING
		#include	"string.h"
	#endif
	#ifndef	KERNEL_STORAGE
		#include	"../kernel/storage.h"
	#endif

uint8_t lib_vfs_check( uintptr_t address, uint64_t size_byte ) {
	// properties of file
	uint32_t *vfs = (uint32_t *) address;

	// magic value?
	if( vfs[ (size_byte >> STD_SHIFT_4) - 1 ] == LIB_VFS_magic ) return TRUE;	// yes

	// no
	return FALSE;
}

struct KERNEL_STORAGE_STRUCTURE_FILE lib_vfs_file( struct LIB_VFS_STRUCTURE *vfs, uint8_t *path, uint64_t length ) {
	MACRO_DEBUF();

	// start from root directory?
	if( *path == '/' ) {
		// find root directory structure
		while( vfs[ 0 ].offset != vfs[ 1 ].offset ) vfs = (struct LIB_VFS_STRUCTURE *) vfs[ 1 ].offset;

		// remove leading '/' and ending slash from path
		while( *path == '/' ) { path++; length--; }
		while( path[ length - 1 ] == '/' ) length--;
	}

	// parse path
	while( TRUE ) {
		// first name in path
		uint64_t filename_length = EMPTY;

		// increment until slash
		while( filename_length < length && path[ filename_length ] != '/' ) filename_length++;

		// select file from current directory structure
		do { if( vfs -> length == filename_length && lib_string_compare( path, (uint8_t *) vfs -> name, filename_length ) ) break;
		} while( (++vfs) -> length );

		// file found?
		if( ! vfs -> length ) break;	// no

		// this is requested file?
		if( length > filename_length ) {
			// select next file from path
			path += filename_length;

			// remove leading '/'
			while( *path == '/' ) { path++; length--; };

			// continue
			continue;
		}

		// last file from path is requested one?
		if( length == filename_length && lib_string_compare( path, (uint8_t *) vfs -> name, filename_length ) ) return (struct KERNEL_STORAGE_STRUCTURE_FILE) { (uint64_t) vfs, vfs -> size };
	}

	// file not found
	return (struct KERNEL_STORAGE_STRUCTURE_FILE) { EMPTY };
}