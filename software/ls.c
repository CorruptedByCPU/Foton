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
	struct STD_STREAM_STRUCTURE_META stream_meta;
	struct STD_FILE_STRUCTURE file = { EMPTY };

	#define	LS_MARGIN	2

	// action change
	uint8_t show_hidden	= FALSE;
	uint8_t	show_properties	= FALSE;

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	//----------------------------------------------------------------------

	// retrieve stream meta data
	std_stream_get( (uint8_t *) &stream_meta, STD_STREAM_OUT );

	//----------------------------------------------------------------------

	// load content of current directory
	uint8_t path[] = "/system/lib";
	for( uint8_t i = 0; i < sizeof( path ) - 1; i++ ) file.name[ file.length++ ] = path[ i ];

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
	uint16_t column_width = 0;

	// amount of files to show
	uint64_t file_limit = EMPTY;
	while( vfs[ file_limit ].length ) {
		// set longest file name as column width
		if( column_width < vfs[ file_limit ].length ) column_width = vfs[ file_limit ].length;

		// next file
		file_limit++;
	}

	// prepare column movement sequence
	uint8_t column_string[ ] = { EMPTY };
	sprintf( "\e[%uC", (uint8_t *) &column_string, column_width + LS_MARGIN );

	// parse each file
	for( uint64_t i = 0; i < file_limit; i++ ) {
		// show hidden?
		if( *vfs[ i ].name == '.' && ! show_hidden ) continue;	// no

		// cannot fit name in this column?
		if( column + vfs[ i ].length + LS_MARGIN > stream_meta.width ) {
			// start from new line
			print( "\n" );

			// first column
			column = 0;
		}

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

		// show file name
		printf( "\e[s%s\e[u\e[0m", (uint8_t *) &vfs[ i ].name );

		// move cursor to next column
		printf( "%s", column_string );

		// next column position
		column += column_width;
	}

	// exit
	return 0;
}
