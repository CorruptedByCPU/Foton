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
	struct STD_FILE_STRUCTURE file = { EMPTY };

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// user selected file?
	if( argc == 1 ) return 0;	// no

	// load content of selected file/directory
	for( uint8_t i = 0; i < lib_string_length( argv[ 1 ] ); i++ ) file.name[ file.length++ ] = argv[ 1 ][ i ];

	//----------------------------------------------------------------------

	// retrieve properties of selected file
	if( ! std_file( (struct STD_FILE_STRUCTURE *) &file ) ) return -1;	// file not found

	// assign area for file content
	uint8_t *content = (uint8_t *) malloc( file.length_byte );

	// area acquired?
	if( ! content ) return -1;	// no

	// load directory content
	std_file_read( (struct STD_FILE_STRUCTURE *) &file, (uintptr_t) content );

	//----------------------------------------------------------------------

	// amount of files to show
	uint64_t file_limit = EMPTY;
	for( uint64_t i = 0; i < file.length_byte; i++ )
		putc( content[ i ] );

	// exit
	return 0;
}
