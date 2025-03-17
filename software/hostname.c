/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/string.h"

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// open file for read
	FILE *file = fopen( (uint8_t *) "/etc/hostname.txt", EMPTY );
	if( ! file ) return STD_ERROR_file_not_found;

	// some arguments provided?
	if( argc > 1 ) {	// yes
		for( uint64_t j = 1; j < argc; j++ ) {	// change behavior
			// option?
			if( argv[ j ][ 0 ] == '-' ) continue;	// ignore

			// name overflow?
			if( lib_string_length( argv[ j ] ) > 63 ) {
				// show propert error
				print( "Hostname length limit: 63 characters." );

				// error
				return STD_ERROR_overflow;
			}

			// overwrite current file content
			file -> seek = EMPTY;

			// write new hostname to file
			fwrite( file, argv[ j ], lib_string_length( argv[ j ] ) + 1 );
		}
	}

	// assign area for file content
	uint8_t *content;
	if( ! (content = (uint8_t *) malloc( file -> byte )) ) return STD_ERROR_memory_low;

	// load file content
	fread( file, content, file -> byte );

	// close file
	fclose( file );

	// remove "white" characters from first line
	lib_string_trim( content, lib_string_length( content ) );

	// limit to first word
	content[ lib_string_word_of_letters_and_digits( content, lib_string_length( content ) ) ] = STD_ASCII_TERMINATOR;

	// show content
	printf( "%s", content );

	// exit
	return 0;
}
