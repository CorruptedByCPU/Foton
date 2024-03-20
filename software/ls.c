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
	struct STD_STREAM_STRUCTURE_META stream_meta;

	#define	LS_MARGIN	2

	// action change
	uint8_t show_hidden	= FALSE;
	uint8_t	show_properties	= FALSE;

	struct LS_STRUCTURE {
		FILE		*file;
		uint64_t	argv;
	};

uint8_t ls_units[] = { ' ', 'K', 'M', 'G', 'T' };

void ls_format( uint64_t bytes ) {
	// unity type
	uint8_t unit = 0;	// bytes by default
	while( pow( 1024, unit ) < bytes ) unit++;

	// show higher units?
	if( unit ) printf( "\e[0m %4.1f\e[38;5;241m%c ", (float) bytes / (float) pow( 1024, unit - 1 ), ls_units[ unit - 1] );
	else printf( "%9u ", bytes );	// only Bytes
}

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// array of directories
	uint64_t i = 0;
	struct LS_STRUCTURE *dir = malloc( TRUE );

	// some arguments provided?
	if( argc > 1 ) {	// yes
		for( uint64_t j = 1; j < argc; j++ ) {	// change behavior
			// option?
			if( argv[ j ][ 0 ] == '-' ) {
				// options
				uint8_t o = EMPTY;
				while( argv[ j ][ ++o ] ) {
					// show hidden?
					if( argv[ j ][ o ] == 'a' ) show_hidden = TRUE;	// yes

					// properties of every file?
					if( argv[ j ][ o ] == 'l' ) show_properties = TRUE;	// yes
				}
			// then it should be directory/path
			} else {
				// prepare space for directory properties
				dir = (struct LS_STRUCTURE *) realloc( dir, sizeof( struct LS_STRUCTURE ) * (i + 1) );

				// get directory properties
				if( (dir[ i ].file = fopen( argv[ j ], STD_FILE_MODE_read )) ) {
					// assign argv entry to directory entry
					dir[ i ].argv = j;

					// directory opened
					i++;
				}
				else {
					// directory not found
					printf( "Directory '%s' not found.\n", argv[ j ] );

					// end of program
					return STD_ERROR_file_not_found;
				}
			}
		}
	}

	// if no directoris selected inside arguments, open current directory
	if( ! i ) dir[ i++ ].file = fopen( (uint8_t *) ".", STD_FILE_MODE_read );

	//----------------------------------------------------------------------

	// retrieve stream meta data
	std_stream_get( (uint8_t *) &stream_meta, STD_STREAM_OUT );

	//----------------------------------------------------------------------

	// show content of each directory
	for( uint64_t j = 0; j < i; j++ ) {
		// show few directories?
		if( i > 1 ) printf( "\e[0m%s:\n", argv[ dir[ j ].argv ] );

		// assign area for directory content
		struct LIB_VFS_STRUCTURE *vfs = (struct LIB_VFS_STRUCTURE *) malloc( dir[ j ].file -> byte );

		// area acquired?
		if( ! vfs ) return STD_ERROR_memory_low;	// no

		// load directory content
		fread( dir[ j ].file, (uint8_t *) vfs, dir[ j ].file -> byte );

		//----------------------------------------------------------------------

		uint16_t column = 0;
		uint16_t column_width = LS_MARGIN;

		// amount of files to show
		uint64_t file_limit = EMPTY;
		while( vfs[ file_limit ].name_length ) {
			// set longest file name as column width
			if( column_width <= vfs[ file_limit ].name_length ) column_width = vfs[ file_limit ].name_length + LS_MARGIN;

			// next file
			file_limit++;
		}

		// prepare column movement sequence
		uint8_t column_string[ 8 + 1 ] = { EMPTY };
		sprintf( "\e[%uC", (uint8_t *) &column_string, column_width );

		// parse each file
		for( uint64_t k = 0; k < file_limit; k++ ) {
			// show hidden?
			if( vfs[ k ].name[ 0 ] == '.' && ! show_hidden ) continue;	// no

			// properties mode?
			if( show_properties )
				// size of file
				ls_format( vfs[ k ].byte );

			// cannot fit name in this column?
			if( column + vfs[ k ].name_length >= stream_meta.width ) {
				// start from new line
				print( "\n" );

				// first column
				column = 0;
			} else
				// move cursor to next column
				if( column ) printf( "%s", column_string );

			// change color by type
			switch( vfs[ k ].type ) {
				case STD_FILE_TYPE_default: { print( "\e[38;5;253m" ); break; }
				case STD_FILE_TYPE_directory: { print( "\e[38;5;27m" ); break; }
				case STD_FILE_TYPE_link: { print( "\e[38;5;45m" ); break; }
			}

			// properties mode?
			if( show_properties ) {
				// name of file
				printf( "%s\n", (uint8_t *) &vfs[ k ].name );

				// next file
				continue;
			}

			// show file name
			printf( "\e[s%s\e[u\e[0m", (uint8_t *) &vfs[ k ].name );

			// next column position
			column += column_width;

			// last file showed somewhere far in column
			if( k + 1 == file_limit && column ) print( "\n" );
		}

		// first directory showed?
		if( i > 1 && j + 1 < i ) print( "\n" );	// yep

		// release directory content
		free( vfs );

		// close directory
		fclose( dir[ j ].file );
	}

	// release directory structure
	free( dir );

	// exit
	return 0;
}
