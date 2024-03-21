/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/path.h"

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// show current path of process
	printf( "%s", lib_path() );

	// exit
	return 0;
}
