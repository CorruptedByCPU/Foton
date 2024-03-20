/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

// PROGRAM IN PROGRESS

	#include	"../library/input.h"
	#include	"../library/string.h"
	#include	"../library/vfs.h"

	struct STD_STREAM_STRUCTURE_META stream_meta;
	FILE *file = EMPTY;

	uint8_t *document_name = EMPTY;

	uint8_t *document_area = EMPTY;
	uint64_t document_size = 0;

	uint64_t document_line_location = 0;
	uint64_t document_line_pointer = 0;
	uint64_t document_line_pointer_saved = 0;
	uint64_t document_line_indicator = 0;
	uint64_t document_line_indicator_saved = 0;
	uint64_t document_line_number = 0;
	uint64_t document_line_count = 0;
	uint64_t document_line_size = 0;
	
	uint64_t document_cursor_x = 0;
	uint64_t document_cursor_y = 0;

	uint8_t document_modified_semaphore = FALSE;

	uint8_t menu_height_line = 2;
	uint8_t string_menu[] = "\e[48;5;15m\e[38;5;0m^x\e[0m Exit %s^o\e[0m Save";

	uint8_t string_color_default[] = "\e[0m";
	uint8_t string_color_modified[] = "\e[48;5;9m\e[38;5;15m";
	uint8_t string_color_shortcut[] = "\e[48;5;15m\e[38;5;0m";

	uint8_t key_ctrl_semaphore = FALSE;

	uint8_t string_cursor_at_menu[ 42 + 4 + 1 ] = { EMPTY };
	uint8_t string_cursor_at_interaction[ 42 + 4 + 1 ] = { EMPTY };

void draw_menu( void ) {
	printf( "\e[s%s\e[2K\e[E", string_cursor_at_interaction );
	if( document_modified_semaphore ) printf( "%s^x\e[0m Exit %s^o\e[0m Save", string_color_shortcut, string_color_modified );
	else printf( "%s^x\e[0m Exit %s^o\e[0m Save", string_color_shortcut, string_color_shortcut );
	print( "\e[u" );
}

void document_refresh( void ) {
	// locate line inside document
	uint64_t i = 0;	// default pointer at beginning of document
	for( uint64_t j = 0; j < document_line_number; j++ ) i += lib_string_length_line( (uint8_t *) &document_area[ i ] ) + 1;

	// preserve cursor position
	print( "\e[s" );

	// clean screen and move cursor at beginning of document
	print( "\e[2J\e[0;0H" );

	// show lines of document up to menu area
	uint64_t j = stream_meta.height - menu_height_line;

	// or until end of document itself
	while( document_area[ i ] && j-- ) {
		// calculate line length
		uint64_t length = lib_string_length_line( (uint8_t *) &document_area[ i ] );

		// limit line length to document view width
		if( length > stream_meta.width - 1 ) length = stream_meta.width - 1;

		// show line if available
		if( length ) printf( "%.*s\n", length, (uint8_t *) &document_area[ i ] );
		else
			// or move to next line
			print( "\n" );

		// next line
		i += lib_string_length_line( (uint8_t *) &document_area[ i ] ) + 1;
	}

	// restore cursor position
	print( "\e[u" );

	// show menu
	draw_menu();
}

void line_refresh( uint8_t current ) {
	// highlight current line
	if( current ) print( "\e[48;5;233m" );

	// move cursor at beginning of line and clear current line
	print( "\e[G\e[2K" );

	// update line content before cursor
	for( uint64_t i = 0; i < document_cursor_x; i++ )
		printf( "%c", document_area[ document_line_location + document_line_indicator + i ] );

	// preserve cursor position
	print( "\e[s");

	// update line content after cursor
	uint64_t length = (stream_meta.width - 1) - document_cursor_x;
	if( length > document_line_size - document_line_pointer ) length = document_line_size - document_line_pointer;
	for( uint64_t i = 0; i < length; i++ )
		printf( "%c", document_area[ document_line_location + document_line_indicator + document_cursor_x + i ] );

	// restore cursor position
	print( "\e[0m\e[u" );
}

void line_restore( void ) {
	// current line have different properties than previous?
	if( document_line_size <= document_line_pointer_saved ) {
		// new properties of line
		document_line_pointer = document_line_size;

		// find which part of line to show
		document_line_indicator = 0;
		while( document_line_size - document_line_indicator > stream_meta.width - 1 ) document_line_indicator++;

		// place cursor at end of line
		document_cursor_x = document_line_size - document_line_indicator;
	} else {
		// restore preserved line properties
		document_line_pointer = document_line_pointer_saved;
		document_line_indicator = document_line_indicator_saved;

		// place cursor at end of line
		document_cursor_x = document_line_pointer - document_line_indicator;
	}

	// show new state of line on screen
	line_refresh( TRUE );
}

void document_parse( void ) {
	document_refresh();

	// default variables of loaded document
	document_line_location = 0;
	document_line_indicator = 0;
	document_line_pointer = 0;
	document_line_size = lib_string_length_line( document_area );
	document_line_count = -1;	// we are counting from ZERO
	document_cursor_x = 0;

	// count lines of loaded document
	uint64_t i = 0; do {
		// by default acquired first line
		document_line_count++;

		// next line of document
		i += lib_string_length_line( (uint8_t *) &document_area[ i ] );
	} while( document_area[ i++ ] );
}

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// retrieve stream meta data
	std_stream_get( (uint8_t *) &stream_meta, STD_STREAM_OUT );

	// prepare row movement sequence for interaction and menu
	sprintf( "\e[%u;%uH", (uint8_t *) &string_cursor_at_interaction, 0, stream_meta.height - 2 );
	sprintf( "\e[%u;%uH", (uint8_t *) &string_cursor_at_menu, 0, stream_meta.height - 1 );

	// prepare area for document name
	document_name = malloc( LIB_VFS_NAME_limit + 1 );

	// file selected?
	if( argc > 1 )	{
		// search for file name
		for( uint64_t i = 1; i < argc; i++ ) {
			// ignore any passed options, no support yet
			if( argv[ i ][ 0 ] == '-' ) continue;
			else {
				// open selected file
				file = fopen( argv[ i ], STD_FILE_MODE_read );

				// ignore other file names, no support yet
				break;
			}
		}
	}

	// if file exist
	if( file ) {
		// set document name
		sprintf( "%s", (uint8_t *) document_name, file -> name );

		// alloc area for file content
		document_area = malloc( file -> byte );
		document_size = file -> byte;

		// load file content into document area
		fread( file, document_area, file -> byte );
	} else {
		// prepare new document area
		document_area = malloc( STD_PAGE_byte + 1 );
		*document_area = STD_ASCII_TERMINATOR;

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

	// show new state of line on screen
	line_refresh( TRUE );

	// main loop
	while( TRUE ) {
		// recieve key
		uint16_t key = getkey();
		if( ! key ) continue;

		// CTRL push?
		if( key == STD_KEY_CTRL_LEFT || key == STD_KEY_CTRL_RIGHT ) key_ctrl_semaphore = TRUE;

		// CTRL release?
		if( key == (STD_KEY_CTRL_LEFT | STD_KEY_RELEASE) || key == (STD_KEY_CTRL_RIGHT | STD_KEY_RELEASE) ) key_ctrl_semaphore = FALSE;

		// selected menu option?
		if( key_ctrl_semaphore ) {
			switch( key ) {
				// SAVE
				case 'o': {
					// by default file wasn't saved, yet
					uint8_t saved = FALSE;

					// // retrieve file name
					// struct DEPRECATED_STD_FILE_STRUCTURE save_as = { EMPTY };
					// for( uint64_t i = 0; i < file.length; i++ ) save_as.name[ save_as.length++ ] = file.name[ i ];
			
					// // file saved?
					// while( ! saved ) {
					// 	// ask about file name
					// 	printf( "\e[s\e[%u;%uH\e[48;5;15m\e[38;5;0m\e[2KSave as: %s", 0, stream_meta.height - 2, save_as.name );

					// 	// select current or new file name form user
					// 	save_as.length = lib_input( (uint8_t *) &save_as.name, stream_meta.width - 9, save_as.length, FALSE );

					// 	// if file name provided, retrieve properties of file if exist
					// 	if( save_as.length ) std_file( (struct DEPRECATED_STD_FILE_STRUCTURE *) &save_as );

					// 	// diffrent exist file selected?
					// 	if( save_as.id != file.id && save_as.id ) {
					// 		// ask, can we overwrite it
					// 		print( "\e[G\e[2KOverwrite? (y/N)" );
					// 		while( TRUE ) {
					// 			// recieve key
					// 			uint16_t key = getkey();
					// 			if( ! key || key & 0x80 ) continue;

					// 			// yes?
					// 			if( key == 'y' || key == 'Y' ) saved = TRUE;

					// 			// done
					// 			break;
					// 		}
					// 	} else
					// 		// save
					// 		saved = TRUE;

						// write document content to file
						// if( saved ) std_file_write( (struct DEPRECATED_STD_FILE_STRUCTURE *) &save_as, (uintptr_t) document_area, document_size );

						// restore cursor properties
						// print( "\e[0m\e[2K\e[u" );
					// }

					// release key state
					key_ctrl_semaphore = FALSE;

					// document saved
					document_modified_semaphore = FALSE;

					// // update file name
					// file -> name_length = 0;
					// for( uint64_t i = 0; i < save_as.length; i++ ) file.name[ file.length++ ] = save_as.name[ i ]; file.name[ file.length ] = STD_ASCII_TERMINATOR;

					// // set document name
					// printf( "\eX%s\e\\", file.name );

					// // update menu state
					// draw_menu();

					// done
					break;
				}

				// EXIT
				case 'x': {
					// set cursor position below document area
					printf( "%s\n", string_cursor_at_menu );

					// done
					exit();
				}
			}

			// done
			continue;
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
					// show new state of line on screen (only remove highlight)
					line_refresh( FALSE );

					// new properties of cursor
					document_cursor_y--;

					// move cursor to previous line
					print( "\e[A" );
				} else {	// yes
					// view document from previous line
					document_line_number--;

					// refresh document view
					document_refresh();
				}

				// search for previous line beginning
				while( document_line_location && document_area[ --document_line_location - 1 ] != STD_ASCII_LINE );

				// check previous line size
				document_line_size = lib_string_length_line( (uint8_t *) &document_area[ document_line_location ] );
				
				// new properties of line
				document_line_pointer = document_line_size;

				// find which part of line to show
				document_line_indicator = 0;
				while( document_line_size - document_line_indicator > stream_meta.width - 1 ) document_line_indicator++;

				// place cursor at end of line
				document_cursor_x = document_line_size - document_line_indicator;
			}

			// remember current pointer and indicator position for cursor at X axis
			document_line_pointer_saved = document_line_pointer;
			document_line_indicator_saved = document_line_indicator;

			// show new state of line on screen
			line_refresh( TRUE );

			// done
			continue;
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
				if( document_cursor_x == stream_meta.width - 1 ) document_line_indicator++;
				else document_cursor_x++;	// no
			} else {
				// show new state of line on screen
				document_line_indicator = 0;	// from beginning
				line_refresh( FALSE );

				// we are in middle of document view?
				if( document_cursor_y < (stream_meta.height - menu_height_line) - 1 ) {
					// new properties of cursor
					document_cursor_y++;

					// move cursor to next line
					print( "\e[B" );
				} else {
					// view document from next line
					document_line_number++;

					// refresh document view
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

			// remember current pointer and indicator position for cursor at X axis
			document_line_pointer_saved = document_line_pointer;
			document_line_indicator_saved = document_line_indicator;

			// show new state of line on screen
			line_refresh( TRUE );

			// done
			continue;
		}
	
		// Arrow DOWN?
		if( key == STD_KEY_ARROW_DOWN ) {
			// it's a last line of document?
			if( document_line_location + document_line_size == document_size ) continue;	// yes

			// we are at end of document view?
			if( document_cursor_y == (stream_meta.height - menu_height_line) - 1 ) {
				// view document from next line
				document_line_number++;

				// refresh document view
				document_refresh();
			} else {
				// reset properties of current line
				document_line_indicator = 0;

				// show new state of line on screen
				line_refresh( FALSE );

				// new properties of cursor
				document_cursor_y++;

				// move cursor to next line
				print( "\e[B" );
			}

			// check next line size
			document_line_location += document_line_size + 1;
			document_line_size = lib_string_length_line( (uint8_t *) &document_area[ document_line_location ] );

			// try saved line properties
			line_restore();

			// done
			continue;
		}

		// Arrow UP?
		if( key == STD_KEY_ARROW_UP ) {
			// it's a first line of document?
			if( ! document_line_location ) continue;	// yes

			// we are at beginning of document view?
			if( ! document_cursor_y ) {
				// view document from next line
				document_line_number--;

				// refresh document view
				document_refresh();
			} else {
				// reset properties of current line
				document_line_indicator = 0;

				// show new state of line on screen
				line_refresh( FALSE );

				// new properties of cursor
				document_cursor_y--;

				// move cursor to previous line
				print( "\e[A" );
			}

			// search for previous line beginning
			while( document_line_location && document_area[ --document_line_location - 1 ] != STD_ASCII_LINE );

			// check previous line size
			document_line_size = lib_string_length_line( (uint8_t *) &document_area[ document_line_location ] );

			// try saved line properties
			line_restore();

			// done
			continue;
		}

		// HOME?
		if( key == STD_KEY_HOME ) {
			// we are at beginning of line?
			if( ! document_line_pointer ) continue;	// not now

			// new properties of line
			document_line_pointer = 0;
			document_line_indicator = 0;

			// update cursor position
			document_cursor_x = 0;

			// remember current pointer and indicator position for cursor at X axis
			document_line_pointer_saved = document_line_pointer;
			document_line_indicator_saved = document_line_indicator;

			// show new state of line on screen
			line_refresh( TRUE );

			// done
			continue;
		}

		// END?
		if( key == STD_KEY_END ) {
			// we are at end of line?
			if( document_line_pointer == document_line_size ) continue;	// not now

			// cursor index inside line
			document_line_pointer = document_line_size;

			// find which part of line to show
			document_line_indicator = 0;
			while( document_line_pointer - document_line_indicator > stream_meta.width - 1 ) document_line_indicator++;

			// place cursor at end of line
			document_cursor_x = document_line_pointer - document_line_indicator;

			// remember current pointer and indicator position for cursor at X axis
			document_line_pointer_saved = document_line_pointer;
			document_line_indicator_saved = document_line_indicator;

			// show new state of line on screen
			line_refresh( TRUE );

			// done
			continue;
		}

		// BACKSPACE?
		if( key == STD_KEY_BACKSPACE ) {
			// we are at beginning of docuemnt?
			if( ! document_line_location && ! document_line_pointer ) continue;	// yes

			// document modified
			document_modified_semaphore = TRUE;

			// we are inside of line?
			if( document_line_pointer ) {
				// move line pointer one character back
				document_line_pointer--;

				// move all characters one position back
				for( uint64_t i = document_line_location + document_line_pointer; i < document_size; i++ )
					document_area[ i ] = document_area[ i + 1 ];

				// character removed from document and line
				document_size--;
				document_line_size--;

				// we changed line visibility?
				if( document_line_indicator > document_line_pointer ) document_line_indicator--;	// fix it
				else document_cursor_x--;
			} else {
				// we are on first line of document area?
				if( ! document_line_location ) continue;	// nothing to do

				// we are on first line of document view?
				if( document_cursor_y ) {
					// new properties of cursor
					document_cursor_y--;

					// move cursor to previous line
					print( "\e[A" );
				} else
					// show document from previous line
					document_line_number--;

				// search for previous line beginning
				while( document_line_location && document_area[ --document_line_location - 1 ] != STD_ASCII_LINE );

				// move line pointer one character back
				document_line_pointer = lib_string_length_line( (uint8_t *) &document_area[ document_line_location ] );

				// check future previous line size
				document_line_size += document_line_pointer;

				// move all characters one position back
				for( uint64_t i = document_line_location + document_line_pointer; i < document_size; i++ )
					document_area[ i ] = document_area[ i + 1 ];

				// character removed from document
				document_size--;

				// merged 2 lines
				document_line_count--;

				// find which part of line to show
				document_line_indicator = 0;
				while( document_line_pointer - document_line_indicator > stream_meta.width - 1 ) document_line_indicator++;

				// place cursor at end of line
				document_cursor_x = document_line_pointer - document_line_indicator;

				// refresh document view
				document_refresh();
			}

			// remember current pointer and indicator position for cursor at X axis
			document_line_pointer_saved = document_line_pointer;
			document_line_indicator_saved = document_line_indicator;

			// show new state of line on screen
			line_refresh( TRUE );

			// update menu state
			draw_menu();

			// done
			continue;
		}

		// DELETE?
		if( key == STD_KEY_DELETE ) {
			// we are at end of docuemnt?
			if( document_line_location + document_line_pointer == document_size ) continue;	// yes

			// document modified
			document_modified_semaphore = TRUE;

			// move all characters one position back
			for( uint64_t i = document_line_location + document_line_pointer; i < document_size; i++ )
				document_area[ i ] = document_area[ i + 1 ];

			// character removed from document and line
			document_size--;

			// removed line endpoint?
			if( ! document_line_size || document_line_pointer > --document_line_size ) {
				// get new line size
				document_line_size = lib_string_length_line( (uint8_t *) &document_area[ document_line_location ] );

				// merged 2 lines
				document_line_count--;

				// refresh document view
				document_refresh();
			}

			// show new state of line on screen
			line_refresh( TRUE );

			// update menu state
			draw_menu();

			// done
			continue;
		}

		// PAGE Down?
		if( key == STD_KEY_PAGE_DOWN ) {
			// by default select last line of document
			uint64_t line = document_line_count;

			// select page to show up
			if( document_line_number + ((stream_meta.height - menu_height_line) - 1) < document_line_count ) {
				// change page
				document_line_number += stream_meta.height - menu_height_line;

				// can we change line as cursor is placed?
				if( document_line_number + document_cursor_y < document_line_count ) line = document_line_number + document_cursor_y;
				else
					// place cursor at last line of document page
					document_cursor_y = document_line_count - document_line_number;
			} else
				// place cursor at last line of document page
				document_cursor_y = document_line_count - document_line_number;

			// set cursor at current line of page
			printf( "\e[%u;%uH", 0, document_cursor_y );

			// locate line inside document
			document_line_location = 0;	// default pointer at beginning of document
			while( line-- ) document_line_location += lib_string_length_line( (uint8_t *) &document_area[ document_line_location ] ) + 1;

			// get new line size
			document_line_size = lib_string_length_line( (uint8_t *) &document_area[ document_line_location ] );

			// refresh document view
			document_refresh();

			// try saved line properties
			line_restore();

			// done
			continue;
		}

		// PAGE Up?
		if( key == STD_KEY_PAGE_UP ) {
			// selected line
			uint64_t line;

			// select page to show up
			if( document_line_number - ((stream_meta.height - menu_height_line) - 1) < document_line_count ) {
				// change page
				document_line_number -= stream_meta.height - menu_height_line;

				// by default first line of page
				line = document_line_number;

				// can we change line to cursor placed?
				if( line + document_cursor_y < document_line_count ) line += document_cursor_y;

			} else {
				// select first page of document
				document_line_number = 0;

				// select first line of page
				line = 0;

				// place cursor at first line of document page
				document_cursor_y = 0;
			}

			// set cursor at current line of page
			printf( "\e[%u;%uH", document_cursor_x, document_cursor_y );

			// locate line inside document
			document_line_location = 0;	// default pointer at beginning of document
			while( line-- ) document_line_location += lib_string_length_line( (uint8_t *) &document_area[ document_line_location ] ) + 1;

			// get new line size
			document_line_size = lib_string_length_line( (uint8_t *) &document_area[ document_line_location ] );

			// refresh document view
			document_refresh();

			// try saved line properties
			line_restore();

			// done
			continue;
		}

		// ENTER?
		if( key == STD_KEY_ENTER ) {
			// document modified
			document_modified_semaphore = TRUE;

			// pointer in middle of document?
			if( document_line_location + document_line_pointer != document_size )
				// move all characters one position further
				for( uint64_t i = document_size; i > document_line_location + document_line_pointer; i-- )
					document_area[ i ] = document_area[ i - 1 ];

			// insert character at end of document
			document_area[ document_line_location + document_line_pointer ] = STD_ASCII_LINE;

			// another line in document
			document_line_count++;

			// document size
			document_size++;

			// update cursor position
			document_cursor_x = 0;

			// we are at end of document view?
			if( document_cursor_y == (stream_meta.height - menu_height_line) - 1 )
				// view document from next line
				document_line_number++;
			else
				// new properties of cursor
				document_cursor_y++;

			// set cursor at current line of page
			printf( "\e[%u;%uH", document_cursor_x, document_cursor_y );

			// move to start of new line
			document_line_location += document_line_pointer + 1;

			// get new line size
			document_line_size = lib_string_length_line( (uint8_t *) &document_area[ document_line_location ] );

			// show current and new line from beginning
			document_line_indicator = 0;
			document_line_pointer = 0;

			// refresh document view
			document_refresh();

			// refresh current line staste
			line_refresh( TRUE );

			// remember current pointer and indicator position for cursor at X axis
			document_line_pointer_saved = document_line_pointer;
			document_line_indicator_saved = document_line_indicator;

			// done
			continue;
		}

		// check if key is printable
		if( key < STD_ASCII_SPACE || key > STD_ASCII_TILDE) continue;	// no, done

		// document modified
		document_modified_semaphore = TRUE;

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

		// cursor position at end of row?
		if( document_cursor_x == stream_meta.width - 1 )
			// show line from next character
			document_line_indicator++;
		else
			// update properties of cursor
			document_cursor_x++;

		// show new state of line on screen
		line_refresh( TRUE );

		// update menu state
		draw_menu();

		// document size
		document_size++;

		// remember current pointer and indicator position for cursor at X axis
		document_line_pointer_saved = document_line_pointer;
		document_line_indicator_saved = document_line_indicator;
	}

	// process ended properly
	return EMPTY;
}

// log( "ds: %u, dln/c: %u/%u, dll: %u, dli[s]: %u/%u, dlp[s]: %u/%u, dls: %u, dcx: %u, dcy: %u\n", document_size, document_line_number, document_line_count, document_line_location, document_line_indicator, document_line_indicator_saved, document_line_pointer, document_line_pointer_saved, document_line_size, document_cursor_x, document_cursor_y );