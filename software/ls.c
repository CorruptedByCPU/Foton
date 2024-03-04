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
	struct STD_FILE_STRUCTURE file = { EMPTY };

	#define	LS_MARGIN	2

	// action change
	uint8_t show_hidden	= FALSE;
	uint8_t	show_properties	= FALSE;

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
	// by default load current directory content
	uint8_t path[] = ".";
	for( uint8_t i = 0; i < sizeof( path ) - 1; i++ ) file.name[ file.length++ ] = path[ i ];

	// some arguments provided?
	if( argc > 1 ) {	// yes
		for( uint64_t i = 1; i < argc; i++ ) {	// change behavior
			// option?
			if( argv[ i ][ 0 ] == '-' ) {
				// options
				uint8_t o = EMPTY;
				while( argv[ i ][ ++o ] ) {
					// show hidden?
					if( argv[ i ][ o ] == 'a' ) show_hidden = TRUE;	// yes

					// properties of every file?
					if( argv[ i ][ o ] == 'l' ) show_properties = TRUE;	// yes
				}
			// then should be path to file/directory
			} else {
				// load content of last selected file/directory
				file.length = EMPTY;
				for( uint8_t j = 0; j < lib_string_length( argv[ i ] ); j++ ) file.name[ file.length++ ] = argv[ i ][ j ];
			}
		}
	}

	//----------------------------------------------------------------------

	// retrieve stream meta data
	std_stream_get( (uint8_t *) &stream_meta, STD_STREAM_OUT );

	//----------------------------------------------------------------------

	// retrieve properties of current directory content
	if( ! std_file( (struct STD_FILE_STRUCTURE *) &file ) ) return -1;	// file not found

	// assign area for directory content
	struct LIB_VFS_STRUCTURE *vfs = (struct LIB_VFS_STRUCTURE *) malloc( file.length_byte );

	// area acquired?
	if( ! vfs ) return -1;	// no

	// load directory content
	std_file_read( (struct STD_FILE_STRUCTURE *) &file, (uintptr_t) vfs );

	//----------------------------------------------------------------------

	uint16_t column = 0;
	uint16_t column_width = LS_MARGIN;

	// amount of files to show
	uint64_t file_limit = EMPTY;
	while( vfs[ file_limit ].length ) {
		// set longest file name as column width
		if( column_width <= vfs[ file_limit ].length ) column_width = vfs[ file_limit ].length + LS_MARGIN;

		// next file
		file_limit++;
	}

	// prepare column movement sequence
	uint8_t column_string[ 8 + 1 ] = { EMPTY };
	sprintf( "\e[%uC", (uint8_t *) &column_string, column_width );

	// parse each file
	for( uint64_t i = 0; i < file_limit; i++ ) {
		// show hidden?
		if( vfs[ i ].name[ 0 ] == '.' && ! show_hidden ) continue;	// no

		// properties mode?
		if( show_properties )
			// size of file
			ls_format( vfs[ i ].size );

		// cannot fit name in this column?
		if( column + vfs[ i ].length >= stream_meta.width ) {
			// start from new line
			print( "\n" );

			// first column
			column = 0;
		} else
			// move cursor to next column
			if( column ) printf( "%s", column_string );

		// change color by type
		switch( vfs[ i ].type ) {
			case STD_FILE_TYPE_regular_file: {
				// file is executable?
				if( vfs[ i ].mode & STD_FILE_MODE_user_exec ) print( "\e[38;5;47m" );	// yes
				else print( "\e[38;5;253m" );	// no
				
				// done
				break;
			}
			case STD_FILE_TYPE_directory: { print( "\e[38;5;27m" ); break; }
			case STD_FILE_TYPE_symbolic_link: { print( "\e[38;5;45m" ); break; }
		}

		// properties mode?
		if( show_properties ) {
			// name of file
			printf( "%s\n", (uint8_t *) &vfs[ i ].name );

			// next file
			continue;
		}

		// show file name
		printf( "\e[s%s\e[u\e[0m", (uint8_t *) &vfs[ i ].name );

		// next column position
		column += column_width;
	}

	// exit
	return 0;
}
