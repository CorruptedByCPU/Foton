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

	// open file for read
	FILE *file;
	if( ! (file = fopen( argv[ 1 ] )) ) return STD_ERROR_file_not_found;

	// assign area for file content
	uint8_t *content;
	if( ! (content = (uint8_t *) malloc( file -> byte )) ) return STD_ERROR_memory_low;

	// load file content
	fread( file, content, file -> byte );

	// close file
	fclose( file );

	// show content
	printf( "%s", content );

	// exit
	return 0;
}
