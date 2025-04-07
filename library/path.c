/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/integer.h"

uint8_t *lib_path( void ) {
	// prepare area for path resolution
	uint64_t current_length = EMPTY;
	uint8_t *current_path = calloc( lib_integer_limit_unsigned( MACRO_SIZEOF( struct STD_STRUCTURE_FILE, name_length ) ) + 1 );

	// array of directories names
	uint64_t i = 0;
	FILE **dir = malloc( TRUE );

	// check previous directory names
	do {
		// prepare space for dir properties
		dir = (FILE **) realloc( dir, sizeof( FILE ) * (i + 1) );

		// get directory properties
		dir[ i ] = fopen( (uint8_t *) ".", EMPTY );

		// change directory to previous one
		std_cd( (uint8_t *) "..", 2 );


	} while( *dir[ i++ ] -> name != STD_ASCII_SLASH );

	// show current directory path, and close parent directories
	if( --i ) while( i ) {
		// add directory name to path
		sprintf( "/%s", (uint8_t *) &current_path[ current_length ], (uint8_t *) &dir[ --i ] -> name );

		// generated path length
		current_length += dir[ i ] -> name_length + 1;

		// close directory
		fclose( dir[ i ] );
	}
	else
		// we are at root directory
		sprintf( "%s", (uint8_t *) &current_path[ current_length++ ], "/" );

	// close current directory
	fclose( dir[ 0 ] );

	// terminate path
	current_path[ current_length ] = STD_ASCII_TERMINATOR;

	// exit
	return current_path;
}
