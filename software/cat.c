/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/string.h"
	#include	"../library/vfs.h"

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// user selected file?
	if( argc == 1 ) return 0;	// no

	// open file for readd
	struct STD_STRUCTURE_FILE file = { EMPTY };
	file.socket = std_file_open( argv[ 1 ], lib_string_length( argv[ 1 ] ) );
	
	// default error if path not found
	if( file.socket < EMPTY ) return STD_ERROR_file_not_found;

	// retrieve file properties
	std_file( (struct STD_STRUCTURE_FILE *) &file );

	// assign area for file content
	uint8_t *content;
	if( ! (content = (uint8_t *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( file.byte ) >> STD_SHIFT_PAGE )) ) return STD_ERROR_memory_low;

	// load file content
	std_file_read( (struct STD_STRUCTURE_FILE *) &file, content, file.byte );

	// show content
	for( uint64_t i = 0; i < file.byte; i++ ) printf( "%c", content[ i ] );

	// close file
	std_file_close( file.socket );

	// exit
	return 0;
}