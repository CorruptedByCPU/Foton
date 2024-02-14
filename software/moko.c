/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

// PROGRAM IN PROGRESS

	#include	"../library/string.h"
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
	uint64_t document_cursor_x_last = 0;

	uint8_t menu_height_line = 1;
	uint8_t string_menu[] = "\e[48;5;15m\e[38;5;0m^x\e[0m Exit"; // \e[48;5;15m\e[38;5;0m^r\e[0m Read \e[48;5;15m\e[38;5;0m^o\e[0m Save";

	uint8_t key_ctrl_semaphore = FALSE;

void document_parse( void ) {
	// move cursor at beginning of document
	print( "\e[0;0H" );

	// until end of document
	uint64_t i = 0;
	uint64_t j = stream_meta.height - menu_height_line;	// or end of document area
	while( document_area[ i ] && j-- ) {
		// calculate line length
		uint64_t length = lib_string_length_line( (uint8_t *) &document_area[ i ] );

		// show line (with limited length)
		if( length > stream_meta.width ) length = stream_meta.width;
		printf( "%.*s\n", length, (uint8_t *) &document_area[ i ] );

		// next line
		i += lib_string_length_line( (uint8_t *) &document_area[ i ] ) + 1;
	}

	// default variables of loaded document
	document_pointer = 0;
	document_line_location = 0;
	document_line_indicator = 0;
	document_line_pointer = 0;
	document_line_byte = lib_string_length_line( document_area );
	document_cursor_x = 0;
}

void line_fill( void ) {
	// preserve cursor position
	print( "\e[s");

	// update line content on terminal
	uint64_t length = document_line_byte - document_line_pointer;
	if( length > stream_meta.width - document_cursor_x ) length = stream_meta.width - document_cursor_x;
	for( uint64_t i = 0; i < length; i++ )
		printf( "%c", document_area[ document_pointer + i ] );

	// clean up other character locations on line
	print( " " );

	// restore cursor position
	print( "\e[u" );
}

void line_update( void ) {
	// move cursor at beginning of line
	print( "\e[G" );

	// update whole line content
	uint64_t length = stream_meta.width;
	if( length > document_line_byte ) length = document_line_byte;
	for( uint64_t i = 0; i < length; i++ ) 
		printf( "%c", document_area[ document_line_location + document_line_indicator + i ] );
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
		document_area = malloc( file.length_byte );
		document_byte = file.length_byte;

		// load file content into document area
		std_file_read( (struct STD_FILE_STRUCTURE *) &file, (uintptr_t) document_area );
	} else {
		// prepare new document area
		document_area = malloc( STD_PAGE_byte + 1 );
		document_area[ document_pointer ] = STD_ASCII_TERMINATOR;

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

	// parse document and show it content
	document_parse();

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
	
		// // Arrow DOWN?
		// if( key == STD_KEY_ARROW_DOWN ) {
		// 	// it's a last line of document?
		// 	if( document_line_location + document_line_byte == document_byte ) continue;	// yes

		// 	// check next line size
		// 	uint64_t length = lib_string_length_line( (uint8_t *) &document_area[ document_line_location + document_line_byte + 1 ] );

		// 	// move document pointer at beginning of current line
		// 	document_pointer -= document_line_pointer;

		// 	// preserve important line and cursor properties
		// 	uint64_t indicator = document_line_indicator;
		// 	uint64_t pointer = document_line_pointer;
		// 	uint64_t x = document_cursor_x;

		// 	// reset line properties
		// 	document_line_pointer = 0;

		// 	// current line visible from beginning?
		// 	if( document_line_indicator ) {	// no
		// 		// preserve cursor position
		// 		print( "\e[s");

		// 		// show line from beginning
		// 		document_line_indicator = 0;
		// 		line_update();

		// 		// restore cursor position
		// 		print( "\e[u" );
		// 	}

		// 	// move cursor line down
		// 	print( "\e[B" );

		// 	// current line have same properties as previous?
		// 	if( length >= x ) document_cursor_x = x; else document_cursor_x = length;
		// 	while( length > stream_meta.width ) { length -= stream_meta.width; document_line_indicator += stream_meta.width; }
		// }

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

		// BACKSPACE?
		if( key == STD_KEY_BACKSPACE ) {
			// we are at beginning of docuemnt?
			if( ! document_pointer ) continue;	// yes

			// we are at beginning of line?
			if( ! document_line_pointer ) continue;	// not now

			// set mark on previous character
			document_pointer--;

			// cursor index inside line
			document_line_pointer--;

			// cursor at first column and line showed not from first character
			if( ! document_cursor_x && document_line_indicator ) document_line_indicator--;
			else document_cursor_x--;	// no

			// move all characters one position back
			for( uint64_t i = document_pointer; i < document_byte; i++ )
				document_area[ i ] = document_area[ i + 1 ];

			// character removed from document and line
			document_byte--;
			document_line_byte--;

			// remove previous character from terminal
			print( "\b" );

			// update line content after cursor
			line_fill();	// if exist
		}

		// DELETE?
		if( key == STD_KEY_DELETE ) {
			// we are at end of docuemnt?
			if( document_pointer == document_byte ) continue;	// yes

			// we are at end of line?
			if( document_line_pointer == document_line_byte ) continue;	// not now

			// move all characters one position back
			for( uint64_t i = document_pointer; i < document_byte; i++ )
				document_area[ i ] = document_area[ i + 1 ];

			// character removed from document and line
			document_byte--;
			document_line_byte--;

			// update line content after cursor
			line_fill();	// if exist
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