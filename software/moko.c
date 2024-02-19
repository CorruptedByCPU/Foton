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
	uint64_t document_size = 0;

	uint64_t document_line_location = 0;
	uint64_t document_line_pointer = 0;
	uint64_t document_line_indicator = 0;
	uint64_t document_line_number = 0;
	uint64_t document_line_count = 0;
	uint64_t document_line_size = 0;
	
	uint64_t document_cursor_x = 0;
	uint64_t document_cursor_y = 0;
	uint64_t document_cursor_x_last = 0;

	uint8_t menu_height_line = 1;
	uint8_t string_menu[] = "\e[48;5;15m\e[38;5;0m^x\e[0m Exit"; // \e[48;5;15m\e[38;5;0m^r\e[0m Read \e[48;5;15m\e[38;5;0m^o\e[0m Save";

	uint8_t key_ctrl_semaphore = FALSE;

	uint8_t string_cursor_at_menu[ 42 + 4 + 1 ] = { EMPTY };
	uint8_t string_cursor_at_interaction[ 42 + 4 + 1 ] = { EMPTY };

void document_refresh( void ) {
	// locate line inside document
	uint64_t i = 0;	// default pointer at beginning of document
	uint64_t lines = document_line_number;	// first line number to display
	while( lines-- ) i += lib_string_length_line( (uint8_t *) &document_area[ i ] ) + 1;

	// preserve cursor position and move it at beginning of document
	print( "\e[s\e[0;0H" );

	// show lines of document up to menu area
	uint64_t j = stream_meta.height - menu_height_line;

	// or until end of document itself
	while( document_area[ i ] && j-- ) {
		// calculate line length
		uint64_t length = lib_string_length_line( (uint8_t *) &document_area[ i ] );

		// show line (with limited length)
		if( length > stream_meta.width ) length = stream_meta.width;
		printf( "\e[2K%.*s\n", length, (uint8_t *) &document_area[ i ] );

		// next line
		i += lib_string_length_line( (uint8_t *) &document_area[ i ] ) + 1;
	}

	// restore cursor position
	print( "\e[u" );
}

void line_refresh( void ) {
	// move cursor at beginning of line and clean it
	print( "\e[G\e[2K" );

	// update line content before cursor
	for( uint64_t i = 0; i < document_cursor_x; i++ )
		printf( "%c", document_area[ document_line_location + document_line_indicator + i ] );

	// preserve cursor position
	print( "\e[s");

	// update line content after cursor
	uint64_t length = stream_meta.width - document_cursor_x;
	if( length > document_line_size - document_line_pointer ) length = document_line_size - document_line_pointer;
	for( uint64_t i = 0; i < length; i++ )
		printf( "%c", document_area[ document_line_location + document_line_indicator + document_cursor_x + i ] );

	// restore cursor position
	print( "\e[u" );
}

void document_parse( void ) {
	document_refresh();

	// default variables of loaded document
	document_pointer = 0;
	document_line_location = 0;
	document_line_indicator = 0;
	document_line_pointer = 0;
	document_line_size = lib_string_length_line( document_area );
	document_cursor_x = 0;
}

void line_start( void ) {
	// preserve cursor position
	print( "\e[s");

	// move cursor at beginning of line
	print( "\e[G" );

	// update line content on terminal
	uint64_t length = document_line_size;
	if( length > stream_meta.width ) length = stream_meta.width;
	for( uint64_t i = 0; i < length; i++ )
		printf( "%c", document_area[ document_line_location + i ] );

	for( uint64_t i = 0; i < stream_meta.width - length; i++ )
		// clean up other character locations on line
		print( " " );

	// restore cursor position
	print( "\e[u" );
}

void line_fill( void ) {
	// preserve cursor position
	print( "\e[s");

	// update line content on terminal
	uint64_t length = document_line_size - document_line_pointer;
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
	if( length > document_line_size ) length = document_line_size;
	for( uint64_t i = 0; i < length; i++ ) 
		printf( "%c", document_area[ document_line_location + document_line_indicator + i ] );
}

void draw_menu( void ) { printf( "\e[s%s\e[2K\e[E%s\e[u", string_cursor_at_interaction, string_menu ); }

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// retrieve stream meta data
	std_stream_get( (uint8_t *) &stream_meta, STD_STREAM_OUT );

	// prepare row movement sequence for interaction and menu
	sprintf( "\e[%u;%uH", (uint8_t *) &string_cursor_at_interaction, 0, stream_meta.height - 1 );
	sprintf( "\e[%u;%uH", (uint8_t *) &string_cursor_at_menu, 0, stream_meta.height );

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
		document_size = file.length_byte;

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

	draw_menu();

	// =====================================================================

	// parse document and show it content
	document_parse();

	// move cursor at beginning of document
	print( "\e[0;0H" );

	// main loop
	while( TRUE ) {
		// recieve key
		uint16_t key = getkey();

		// debug
		// if( key & 0x8000 ) log( "ds: %u, dln: %u, dll: %u, dli: %u, dlp: %u, dls: %u, dcx: %u, dcy: %u\n", document_size, document_line_number, document_line_location, document_line_indicator, document_line_pointer, document_line_size, document_cursor_x, document_cursor_y );

		// CTRL push?
		if( key == STD_KEY_CTRL_LEFT || key == STD_KEY_CTRL_RIGHT ) key_ctrl_semaphore = TRUE;

		// CTRL release?
		if( key == (STD_KEY_CTRL_LEFT | STD_KEY_RELEASE) || key == (STD_KEY_CTRL_RIGHT | STD_KEY_RELEASE) ) key_ctrl_semaphore = FALSE;

		// selected menu option? EXIT
		if( key_ctrl_semaphore && key == 'x' ) {
			// set cursor position below document area
			printf( "%s\n", string_cursor_at_menu );

			// done
			exit();
		}

		// Arrow LEFT?
		if( key == STD_KEY_ARROW_LEFT ) {
			// we are at beginning of docuemnt?
			if( ! document_line_location && ! document_line_pointer ) continue;	// yes

			// we are inside of line?
			if( document_line_pointer ) {
				// somewhere far in line?
				if( document_line_indicator == document_line_pointer )
					// show line from previous character
					document_line_indicator--;
				else document_cursor_x--;	// new properties of cursor

				// line index
				document_line_pointer--;
			} else {
				// we are at first line of document view?
				if( document_cursor_y ) {	// no
					// new properties of cursor
					document_cursor_y--;

					// move cursor to previous line
					print( "\e[A" );
				} else {	// yes
					// view document from previous line
					document_line_number--;

					// refresh menu
					document_refresh();
				}

				// search for previous line beginning
				while( document_line_location && document_area[ --document_line_location - 1 ] != STD_ASCII_NEW_LINE );

				// check previous line size
				document_line_size = lib_string_length_line( (uint8_t *) &document_area[ document_line_location ] );
				
				// new properties of line
				document_line_pointer = document_line_size;

				// find which part of line to show
				document_line_indicator = 0;
				while( document_line_size - document_line_indicator > stream_meta.width ) document_line_indicator++;

				// place cursor at end of line
				document_cursor_x = document_line_size - document_line_indicator;
			}

			// show new state of line on screen
			line_refresh();
		}

		// Arrow RIGHT?
		if( key == STD_KEY_ARROW_RIGHT ) {
			// we are at end of docuemnt?
			if( document_line_location + document_line_pointer == document_size ) continue;	// yes

			// we are inside of line?
			if( document_line_pointer < document_line_size ) {
				// line index
				document_line_pointer++;

				// cursor already at end of document view?
				if( document_cursor_x == stream_meta.width ) document_line_indicator++;
				else document_cursor_x++;	// no
			} else {
				// show new state of line on screen
				document_line_indicator = 0;
				line_refresh();

				// we are in middle of document view?
				if( document_cursor_y < stream_meta.height - 2 ) {
					// new properties of cursor
					document_cursor_y++;

					// move cursor to next line
					print( "\e[B" );
				} else {
					// view document from next line
					document_line_number++;

					// refresh menu
					document_refresh();
				}

				// check next line size
				document_line_location += document_line_size + 1;
				document_line_size = lib_string_length_line( (uint8_t *) &document_area[ document_line_location ] );

				// new properties of line
				document_line_pointer = 0;

				// place cursor at beginning of line
				document_cursor_x = 0;
			}

			// show new state of line on screen
			line_refresh();
		}
	
		// Arrow DOWN?
		if( key == STD_KEY_ARROW_DOWN ) {
			// it's a last line of document?
			if( document_line_location + document_line_size == document_size ) continue;	// yes

			// check next line size
			uint64_t new_length = lib_string_length_line( (uint8_t *) &document_area[ document_line_location + document_line_size + 1 ] );

			// move document pointer at beginning of current line
			document_pointer -= document_line_pointer;

			// preserve important line and cursor properties
			uint64_t old_indicator = document_line_indicator;
			uint64_t old_pointer = document_line_pointer;
			uint64_t old_length = document_line_size;
			uint64_t old_x = document_cursor_x;

			// current line visible from beginning?
			document_line_indicator = 0;
			if( old_indicator ) line_refresh(); // no

			// we are at last line of document view?
			if( document_cursor_y == stream_meta.height - 2 ) {
				// scroll up
				print( "\e[S" );

				// view document from next line
				document_line_number++;

				// refresh menu
				draw_menu();
			} else {
				// new properties of cursor
				document_cursor_y++;

				// move cursor to next line
				print( "\e[B" );
			}

			// new properties of document
			document_pointer = document_line_location + document_line_size + 1;

			// line
			document_line_location = document_pointer;
			document_line_size = new_length;

			// current line have similar properties as previous?
			if( new_length >= document_line_pointer ) {
				// nothing changed
				document_pointer += old_pointer;
				document_line_pointer = old_pointer;
				document_line_indicator = old_indicator;
			} else {
				// find which part of line to show
				document_line_indicator = 0;
				while( new_length > stream_meta.width ) { new_length -= stream_meta.width; document_line_indicator += stream_meta.width; }

				// place cursor at end of line
				document_cursor_x = new_length;

				// new propertied of line and document
				document_line_pointer = document_line_indicator + new_length;
				document_pointer += document_line_pointer;
			}

			// show new state of line on screen
			line_refresh();

			// done
			continue;
		}

		// Arrow UP?
		if( key == STD_KEY_ARROW_UP ) {
			// it's a first line of document?
			if( ! document_line_location ) continue;	// yes

			// move document pointer at beginning of current line
			document_pointer -= document_line_pointer;

			// preserve important line and cursor properties
			uint64_t old_indicator = document_line_indicator;
			uint64_t old_pointer = document_line_pointer;
			uint64_t old_length = document_line_size;
			uint64_t old_x = document_cursor_x;

			// current line visible from beginning?
			document_line_indicator = 0;
			if( old_indicator ) line_refresh(); // no

			// we are at first line of document view?
			if( ! document_cursor_y ) {
				// scroll down
				print( "\e[T" );

				// view document from previous line
				document_line_number--;

				// refresh menu
				draw_menu();
			} else {
				// new properties of cursor
				document_cursor_y--;

				// move cursor to next line
				print( "\e[A" );
			}

			// search for previous line beginning
			while( document_pointer && document_area[ --document_pointer - 1 ] != '\n' );

			// check next line size
			uint64_t new_length = lib_string_length_line( (uint8_t *) &document_area[ document_pointer ] );

			// new properties of line
			document_line_location = document_pointer;
			document_line_size = new_length;

			// previous line have similar properties as previous?
			if( new_length >= old_pointer ) {
				// nothing changed
				document_pointer += old_pointer;
				document_line_pointer = old_pointer;
				document_line_indicator = old_indicator;
			} else {
				// find which part of line to show
				document_line_indicator = 0;
				while( new_length > stream_meta.width ) { new_length -= stream_meta.width; document_line_indicator += stream_meta.width; }

				// place cursor at end of line
				document_cursor_x = new_length;

				// new propertied of line and document
				document_line_pointer = document_line_indicator + new_length;
				document_pointer += document_line_pointer;
			}

			// show new state of line on screen
			line_refresh();

			// done
			continue;
		}

		// HOME?
		if( key == STD_KEY_HOME ) {
			// we are at beginning of docuemnt?
			if( ! document_pointer ) continue;	// yes

			// we are at beginning of line?
			if( ! document_line_pointer ) continue;	// not now

			// set mark on beginning of line
			document_pointer = document_line_location;

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
			if( document_pointer == document_size ) continue;	// yes

			// we are at end of line?
			if( document_line_pointer == document_line_size ) continue;	// not now

			// set mark on beginning of line
			document_pointer += document_line_size - document_line_pointer;

			// cursor index inside line
			document_line_pointer = document_line_size;

			// update cursor properties
			document_cursor_x = document_line_pointer;
			if( document_cursor_x > stream_meta.width ) document_cursor_x = stream_meta.width;

			// show last characters of line
			if( document_cursor_x == stream_meta.width ) document_line_indicator = document_line_size - document_cursor_x; else document_line_indicator = 0;

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
			for( uint64_t i = document_pointer; i < document_size; i++ )
				document_area[ i ] = document_area[ i + 1 ];

			// character removed from document and line
			document_size--;
			document_line_size--;

			// remove previous character from terminal
			print( "\b" );

			// update line content after cursor
			line_fill();	// if exist

			// done
			continue;
		}

		// DELETE?
		if( key == STD_KEY_DELETE ) {
			// we are at end of docuemnt?
			if( document_pointer == document_size ) continue;	// yes

			// move all characters one position back
			for( uint64_t i = document_pointer; i < document_size; i++ )
				document_area[ i ] = document_area[ i + 1 ];

			// character removed from document
			document_size--;

			// we are at end of line?
			if( document_line_pointer == document_line_size ) {
				// calculate new current line length
				document_line_size = lib_string_length_line( (uint8_t *) &document_area[ document_line_location ] );

				// refresh lines below current one

				// preserve cursor position
				print( "\e[s");

				// until end of document
				uint64_t i = document_line_location;
				uint64_t j = stream_meta.height - document_cursor_y;	// or end of document area
				log( "%u\n", j );
				while( --j ) {
					// calculate line length
					uint64_t length = lib_string_length_line( (uint8_t *) &document_area[ i ] );

					// show line (with limited length)
					if( length > stream_meta.width ) length = stream_meta.width;
					printf( "%.*s\n", length, (uint8_t *) &document_area[ i ] );

					// next line
					uint64_t a = lib_string_length_line( (uint8_t *) &document_area[ i ] ) + 1;
					i += a;
					log( "dls: %u\n", a );

					// move cursor at beginning of next line and clean it
					print( "\e[E\e[2K" );
				}

				// restore cursor position
				print( "\e[u" );
			} else document_line_size--;	// character removed from line

			// show new state of line on screen
			line_refresh();

			// done
			continue;
		}

		// check if key is printable
		if( key < STD_ASCII_SPACE || key > STD_ASCII_TILDE ) continue;

		// pointer in middle of document?
		if( document_line_location + document_line_pointer != document_size )
			// move all characters one position further
			for( uint64_t i = document_size; i > document_line_location + document_line_pointer; i-- )
				document_area[ i ] = document_area[ i - 1 ];
		
		// insert character at end of document
		document_area[ document_line_location + document_line_pointer ] = key;

		// cursor index inside line
		document_line_pointer++;

		// line size
		document_line_size++;

		// document size
		document_size++;

		// cursor position at end of row?
		if( document_cursor_x == stream_meta.width )
			// show line from next character
			document_line_indicator++;
		else
			// update properties of cursor
			document_cursor_x++;

		// show new state of line on screen
		line_refresh();
	}

	// process ended properly
	return EMPTY;
}

// log( "dp: %u, ds: %u, dli: %u, dlp: %u, dls: %u, dcx: %u, dcy: %u\n", document_pointer, document_size, document_line_indicator, document_line_pointer, document_line_size, document_cursor_x, document_cursor_y );