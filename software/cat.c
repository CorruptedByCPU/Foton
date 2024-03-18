/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/string.h"
	#include	"../library/vfs.h"
	//----------------------------------------------------------------------
	// variables, routines, procedures
	//----------------------------------------------------------------------
	struct DEPRECATED_STD_FILE_STRUCTURE file = { EMPTY };

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// user selected file?
	if( argc == 1 ) return 0;	// no

	// open file for read
	struct NEW_STD_FILE_STRUCTURE *file;
	if( ! (file = fopen( (const char *) argv[ 1 ], NEW_STD_FILE_MODE_read )) ) return STD_ERROR_file_not_found;

	MACRO_DEBUF();

	// assign area for file content
	uint8_t *content = (uint8_t *) malloc( file -> byte );

	// area acquired?
	if( ! content ) return -1;	// no

	// load directory content
	// std_file_read( (struct DEPRECATED_STD_FILE_STRUCTURE *) &file, (uintptr_t) content );

	//----------------------------------------------------------------------

	// amount of files to show
	// uint64_t file_limit = EMPTY;
	// for( uint64_t i = 0; i < file.length_byte; i++ )
		// putc( content[ i ] );

	// exit
	return 0;
}
