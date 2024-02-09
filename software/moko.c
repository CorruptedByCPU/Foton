/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

// PROGRAM IN PROGRESS

	#include	"../library/vfs.h"

	struct STD_STREAM_STRUCTURE_META stream_meta;
	struct STD_FILE_STRUCTURE file = { EMPTY };

	uint8_t *document_name = EMPTY;

	uint8_t *document_area = EMPTY;
	uint64_t document_pointer = 0;
	uint64_t document_byte = 0;

	uint64_t document_line_location = 0;
	uint64_t document_line_pointer = 0;
	uint64_t document_line_indicator = 0;
	uint64_t document_line_byte = 0;
	
	uint64_t document_cursor_x = 0;
	uint64_t document_cursor_y = 0;

	uint8_t menu_height_line = 1;
	uint8_t string_menu[] = "\e[48;5;15m\e[38;5;0m^x\e[0m Exit"; // \e[48;5;15m\e[38;5;0m^r\e[0m Read \e[48;5;15m\e[38;5;0m^o\e[0m Save";

	uint8_t key_ctrl_semaphore = FALSE;

// void document_parse( void ) {
// 	// move cursor at beginning of document
// 	print( "\e[0;0H" );

// 	variable_document_count_of_lines = EMPTY;

// 	// until end of document
// 	uint64_t i = 0;
// 	uint64_t j = stream_meta.height - menu_height_line;	// or end of document area
// 	while( document_area[ i ] && j-- ) {
// 		// calculate line length
// 		uint64_t length = lib_string_length_line( (uint8_t *) &document_area[ i ] );

// 		// show line (limited length)
// 		if( length > stream_meta.width ) length = stream_meta.width;
// 		printf( "%.*s\n", length, (uint8_t *) &document_area[ i ] );

// 		// next line
// 		i += lib_string_length_line( (uint8_t *) &document_area[ i ] ) + 1;

// 		variable_document_count_of_lines++;
// 	}

// 	variable_document_address_start = document_area;
// 	variable_cursor_indicator = document_area;
// 	variable_document_address_end = document_area;
// 	variable_cursor_position_on_line = document_area;
// 	variable_line_count_of_chars = lib_string_length_line( document_area );
// 	variable_line_print_start = EMPTY;
// }

void line_fill( void ) {
	// preserve cursor position
	print( "\e[s");

	// update line content on terminal
	uint64_t length = (document_line_location + document_line_byte) - document_pointer;
	if( length >= stream_meta.width - document_cursor_x ) length = stream_meta.width - document_cursor_x;
	for( uint64_t i = 0; i < length; i++ )
		printf( "%c", document_area[ document_pointer + i ] );
	
	// restore cursor position
	print( "\e[u" );
}

void line_update( void ) {
	// move cursor at beginning of line
	print( "\e[G" );

	// update whole line content
	for( uint64_t i = 0; i < stream_meta.width; i++ ) 
		printf( "%c", document_area[ document_line_location + document_line_indicator + i ] );
}

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// retrieve stream meta data
	std_stream_get( (uint8_t *) &stream_meta, STD_STREAM_OUT );

	// prepare area for document name
	document_name = malloc( LIB_VFS_name_limit + 1 );

	// // file selected?
	// if( argc > 1 )	{
	// 	// search for file name
	// 	for( uint64_t i = 1; i < argc; i++ ) {
	// 		// ignore any passed options, no support yet
	// 		if( argv[ i ][ 0 ] == '-' ) continue;
	// 		else {
	// 			// set file properties
	// 			file.length = EMPTY;
	// 			for( uint8_t j = 0; j < lib_string_length( argv[ i ] ); j++ ) file.name[ file.length++ ] = argv[ i ][ j ];

	// 			// ignore other file names, no support yet
	// 			break;
	// 		}
	// 	}
	// }

	// // if selected, retrieve properties of file
	// if( file.length ) std_file( (struct STD_FILE_STRUCTURE *) &file );

	// // if file exist
	// if( file.id ) {
	// 	// set document name
	// 	sprintf( "%s", (uint8_t *) document_name, file.name );

	// 	// alloc area for file content
	// 	document_area = malloc( file.length_byte );
	// 	variable_document_count_of_chars = file.length_byte;

	// 	// load file content into document area
	// 	std_file_read( (struct STD_FILE_STRUCTURE *) &file, (uintptr_t) document_area );
	// } else {
		// prepare new document area
		document_area = malloc( STD_PAGE_byte );

		// set default document name
		uint8_t name[] = "New document";
		sprintf( "%s", (uint8_t *) document_name, name );
	// }

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

	// parse document and show it content
	// document_parse();

	// move cursor at beginning of document
	print( "\e[0;0H" );

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

		// Arrow LEFT?
		if( key == STD_KEY_ARROW_LEFT ) {
			// we are at beginning of docuemnt?
			if( ! document_pointer ) continue;	// yes

			// we are at beginning of line?
			if( ! document_line_pointer ) continue;	// not now

			// set mark on previous character
			document_pointer--;

			// cursor index inside line
			document_line_pointer--;

			// cursor in middle of line?
			if( document_cursor_x ) {
				// update properties of cursor
				document_cursor_x--;

				// move cursor one step backward
				print( "\e[D" );

				// done
				continue;
			}

			// at beginning of line?
			if( document_line_indicator ) {	// no
				// show line from previous character
				document_line_indicator--;

				// update line content after cursor
				line_fill();	// if exist
			}
		}

		// Arrow RIGHT?
		if( key == STD_KEY_ARROW_RIGHT ) {
			// we are at end of docuemnt?
			if( document_pointer == document_byte ) continue;	// yes

			// we are at end of line?
			if( document_line_pointer == document_line_byte ) continue;	// not now

			// set mark on next character
			document_pointer++;

			// cursor index inside line
			document_line_pointer++;

			// cursor in middle of line?
			if( document_cursor_x < stream_meta.width ) {
				// update properties of cursor
				document_cursor_x++;

				// move cursor one step backward
				print( "\e[C" );

				// done
				continue;
			}

			// show line from next character
			document_line_indicator++;

			// update line content
			line_update();
		}

		// HOME?
		if( key == STD_KEY_HOME ) {
			// we are at beginning of docuemnt?
			if( ! document_pointer ) continue;	// yes

			// we are at beginning of line?
			if( ! document_line_pointer ) continue;	// not now

			// set mark on beginning of line
			document_pointer -= document_line_pointer;

			// cursor index inside line
			document_line_pointer = 0;

			// show line from beginning
			document_line_indicator = 0;

			// move cursor at first column
			print( "\e[G" );

			// update cursor properties
			document_cursor_x = 0;

			// update line content after cursor
			line_fill();	// if exist

			// done
			continue;
		}

		// END?
		if( key == STD_KEY_END ) {
			// we are at end of docuemnt?
			if( document_pointer == document_byte ) continue;	// yes

			// we are at end of line?
			if( document_line_pointer == document_line_byte ) continue;	// not now

			// set mark on beginning of line
			document_pointer += document_line_byte - document_line_pointer;

			// cursor index inside line
			document_line_pointer = document_line_byte;

			// update cursor properties
			document_cursor_x = document_line_pointer;
			if( document_cursor_x > stream_meta.width ) document_cursor_x = stream_meta.width;

			// show last characters of line
			if( document_cursor_x == stream_meta.width ) document_line_indicator = document_line_byte - document_cursor_x; else document_line_indicator = 0;

			// update line content
			line_update();

			// done
			continue;
		}

		// check if key is printable
		if( key < STD_ASCII_SPACE || key > STD_ASCII_TILDE ) continue;

		// pointer in middle of document?
		if( document_pointer != document_byte )
			// move all characters one position further
			for( uint64_t i = document_byte; i > document_pointer; i-- )
				document_area[ i ] = document_area[ i - 1 ];
		
		// insert character at end of document
		document_area[ document_pointer++ ] = key;

		// cursor index inside line
		document_line_pointer++;

		// line size
		document_line_byte++;

		// document size
		document_byte++;

		// cursor position at end of row?
		if( document_cursor_x == stream_meta.width ) {
			// show line from next character
			document_line_indicator++;
			
			// update line content
			line_update();
		} else {
			// update properties of cursor
			document_cursor_x++;

			// show character on terminal
			printf( "%c", key );

			// update line content after cursor
			line_fill();	// if exist
		}

		// log( "dp: %u, db: %u, dli: %u, dlp: %u, dlb: %u, dcx: %u\n", document_pointer, document_byte, document_line_indicator, document_line_pointer, document_line_byte, document_cursor_x );
	}

	// process ended properly
	return EMPTY;
}