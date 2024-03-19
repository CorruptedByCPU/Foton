/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/vfs.h"

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// array of directories names
	uint64_t i = 0;
	FILE **dir = malloc( TRUE );

	// check previous directory names
	do {
		// prepare space for dir properties
		dir = (FILE **) realloc( dir, sizeof( FILE ) * (i + 1) );

		// get directory properties
		dir[ i ] = fopen( (uint8_t *) ".", NEW_STD_FILE_MODE_read );

		// change directory to previous one
		std_cd( (uint8_t *) "..", 2 );


	} while( *dir[ i++ ] -> name != STD_ASCII_SLASH );

	// show current directory path, and close parent directories
	if( --i ) while( i ) { printf( "/%s", dir[ --i ] -> name ); fclose( dir[ i ] ); }
	else print( "/" );

	// close current directory
	fclose( dir[ 0 ] );

	// exit
	return 0;
}
