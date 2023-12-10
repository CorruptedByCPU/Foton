/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/vfs.h"

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// path directories count
	int64_t i = 0;

	// array of files path
	struct STD_FILE_STRUCTURE *dir = malloc( sizeof( struct STD_FILE_STRUCTURE ) * (i + 1) );

	// get properties of current directory
	uint8_t link[ 2 ] = "..";
	for( uint8_t j = 0; j < sizeof( link ) - 1; j++ ) dir[ i ].name[ dir[ i ].length++ ] = link[ j ];
	std_file( (struct STD_FILE_STRUCTURE *) &dir[ i ] );

	// check previous directory names
	while( *dir[ i ].name != STD_ASCII_SLASH ) {
		// prepare space for dir properties
		dir = realloc( dir, sizeof( struct STD_FILE_STRUCTURE ) * ++i );

		// get directory properties
		for( uint8_t j = 0; j < sizeof( link ); j++ ) dir[ i ].name[ dir[ i ].length++ ] = link[ j ];
		std_file( (struct STD_FILE_STRUCTURE *) &dir[ i ] );

		// change directory to previous one
		std_cd( (uint8_t *) &link );
	}

	// show current directory path
	if( i ) while( --i >= 0 ) printf( "/%s", dir[ i ].name );
	else printf( "%s", "/" );

	// exit
	return 0;
}
