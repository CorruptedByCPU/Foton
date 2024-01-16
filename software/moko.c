/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

// PROGRAM IN PROGRESS

	#include	"../library/vfs.h"

	struct STD_STREAM_STRUCTURE_META stream_meta;
	struct STD_FILE_STRUCTURE file = { EMPTY };

	uint8_t *document_name = EMPTY;

	uint8_t *document = EMPTY;
	uint64_t docuemnt_index = 0;
	uint64_t document_length = EMPTY;

	uint64_t document_line = EMPTY;
	uint64_t document_line_index = 0;

	uint64_t document_cursor = EMPTY;

	uint8_t string_menu[] = "\e[48;5;15m\e[38;5;0m^x\e[0m Exit"; // \e[48;5;15m\e[38;5;0m^r\e[0m Read \e[48;5;15m\e[38;5;0m^o\e[0m Save";

	uint8_t key_ctrl_semaphore = FALSE;

void document_parse( void ) {
	// until end of document
	uint64_t i = 0;
	while( document[ i ] ) {
		MACRO_DEBUF();

		// calculate line length
		uint64_t document_line_length = lib_string_length( (uint8_t *) &document[ i ] );

		// show line (limited length)
		if( document_line_length > stream_meta.width ) document_line_length = stream_meta.width;
		printf( "%.*s\n", document_line_length, (uint8_t *) &document[ i ] );

		// next line
		i += document_line_length;
	}
}

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// retrieve stream meta data
	std_stream_get( (uint8_t *) &stream_meta, STD_STREAM_OUT );

	// prepare area for document name
	document_name = malloc( LIB_VFS_name_limit + 1 );

	// file selected?
	if( argc > 1 )	{
		// search for file name
		for( uint64_t i = 1; i < argc; i++ ) {
			// ignore any passed options, no support yet
			if( argv[ i ][ 0 ] == '-' ) continue;
			else {
				// set file properties
				file.length = EMPTY;
				for( uint8_t j = 0; j < lib_string_length( argv[ i ] ); j++ ) file.name[ file.length++ ] = argv[ i ][ j ];

				// ignore other file names, no support yet
				break;
			}
		}
	}

	// if selected, retrieve properties of file
	if( file.length ) std_file( (struct STD_FILE_STRUCTURE *) &file );

	// if file exist
	if( file.id ) {
		// set document name
		sprintf( "%s", (uint8_t *) document_name, file.name );

		// alloc area for file content
		document = malloc( file.length_byte );
		document_length = file.length_byte;

		// load file content into document area
		std_file_read( (struct STD_FILE_STRUCTURE *) &file, (uintptr_t) document );
	} else {
		// prepare new document area
		document = malloc( TRUE );

		// set default document name
		uint8_t name[] = "New document";
		sprintf( "%s", (uint8_t *) document_name, name );
	}

	// INTERFACE -----------------------------------------------------------

	// clear screen
	print( "\e[2J" );

	// set header
	printf( "\eX%s\e\\", document_name );

	// prepare row movement sequence
	uint8_t cursor_string[ 14 + 1 ] = { EMPTY };
	sprintf( "\e[%u;%uH", (uint8_t *) &cursor_string, 0, stream_meta.height );

	// set cursor position at lowest line of terminal
	printf( "%s", cursor_string );

	// show menu
	printf( "%s", string_menu );

	// =====================================================================

	// move cursor at beginning of document
	print( "\e[0;0H" );

	// parse document and show it content
	document_parse();

	// main loop
	while( TRUE ) {
		// recieve key
		uint16_t key = getkey();

		// CTRL push?
		if( key == STD_KEY_CTRL_LEFT || key == STD_KEY_CTRL_RIGHT ) key_ctrl_semaphore = TRUE;

		// CTRL release?
		if( key == (STD_KEY_CTRL_LEFT | STD_KEY_RELEASE) || key == (STD_KEY_CTRL_RIGHT | STD_KEY_RELEASE) ) key_ctrl_semaphore = FALSE;

		// selected menu option? EXIT
		if( key_ctrl_semaphore && key == 'x' ) {
			// set cursor position below document area
			printf( "%s\n", cursor_string );

			// done
			exit();
		}

		// check if key is printable
		if( key < STD_ASCII_SPACE && key > STD_ASCII_TILDE ) continue;	// no, ignore key
	}

	// process ended properly
	return EMPTY;
}
