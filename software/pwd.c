/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/vfs.h"
	//----------------------------------------------------------------------
	// variables, routines, procedures
	//----------------------------------------------------------------------
	struct STD_FILE_STRUCTURE file = { EMPTY };

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// retrieve current directory name
	uint8_t path[] = ".";
	for( uint8_t i = 0; i < sizeof( path ) - 1; i++ ) file.name[ file.length++ ] = path[ i ];
	std_file( (struct STD_FILE_STRUCTURE *) &file );
	printf( "%s", file.name );

	// exit
	return 0;
}
