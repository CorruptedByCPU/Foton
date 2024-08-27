/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t moko_key_ctrl( uint16_t key ) {
	// CTRL push?
	if( key == STD_KEY_CTRL_LEFT || key == STD_KEY_CTRL_RIGHT ) key_ctrl_semaphore = TRUE;

	// CTRL release?
	if( key == (STD_KEY_CTRL_LEFT | STD_KEY_RELEASE) || key == (STD_KEY_CTRL_RIGHT | STD_KEY_RELEASE) ) key_ctrl_semaphore = FALSE;

	// Exit?
	if( key == STD_KEY_ESC ) {
		// set cursor position below document area
		printf( "%s\n", string_cursor_at_menu );

		// done
		exit();
	}

	// selected menu option?
	if( key_ctrl_semaphore ) {
		switch( key ) {
			// SAVE
			case 'o': {
				// by default file wasn't saved, yet
				uint8_t saved = FALSE;

				// length of save_as path
				uint64_t save_as_length = 0;

				// retrieve file path/name
				for( uint64_t i = 0; i < lib_string_length( file_path ); i++ ) save_as[ save_as_length++ ] = file_path[ i ]; save_as[ save_as_length ] = STD_ASCII_TERMINATOR;
		
				// file saved?
				while( ! saved ) {
					// ask about file name
					printf( "\e[s\e[%u;%uH\e[48;5;15m\e[38;5;0m\e[2KSave as: %s", 0, stream_meta.height - 2, save_as );

					// select current or new file name form user
					save_as_length = lib_input( save_as, stream_meta.width - 9, save_as_length, (uint8_t *) &key_ctrl_semaphore );

					// properties of file if exist
					FILE *file_save_as;

					// if file name provided, retrieve it
					if( save_as_length ) file_save_as = fopen( save_as );
					else {
						// restore cursor position
						print( "\e[u" );

						// file name is required
						continue;
					}

					// file exist?
					if( file_save_as ) {
						// opened for write the same file?
						if( file_save_as -> socket == file -> socket ) {
							// close file
							fclose( file_save_as );

							// allow file save as current file name
							saved = TRUE;
						} else {
							// new file?
							if( ! file_save_as -> byte ) saved = TRUE;	// yes
							else {
								// ask, can we overwrite it
								print( "\e[G\e[2KOverwrite? (y/N)" );
								while( TRUE ) {
									// recieve key
									uint16_t key = getkey();
									if( ! key || key & 0x80 ) continue;

									// yes?
									if( key == 'y' || key == 'Y' ) {
										// set new name of document
										sprintf( "%s", (uint8_t *) document_name, file_save_as -> name );

										// close current connection to file
										fclose( file );

										// set new connection
										file = file_save_as;

										// allow file save as different name
										saved = TRUE;
									}

									// done
									break;
								}
							}
						}
					} else {
						// TODO, do something if directory structure of selected path doesn't exist...

						// try to create empty file
						if( (file_save_as = touch( save_as, STD_FILE_TOUCH_file )) ) {
							// set new name of document
							sprintf( "%s", (uint8_t *) document_name, file_save_as -> name );
							
							// close current connection to file if exist
							if( file ) fclose( file );
							
							// set new connection
							file = file_save_as;

							// allow file save as new
							saved = TRUE;
						}
					}

					// write document content to file
					if( saved ) {
						// overwrite current content
						file -> seek = EMPTY;

						// write content of document into prepared file
						fwrite( file, document_area, document_size );
					}

					// restore cursor properties
					print( "\e[0m\e[2K\e[u" );
				}

				// release key state
				key_ctrl_semaphore = FALSE;

				// document saved
				document_modified_semaphore = FALSE;

				// update file path
				for( uint64_t i = 0; i < save_as_length; i++ ) file_path[ i ] = save_as[ i ]; file_path[ save_as_length ] = STD_ASCII_TERMINATOR;

				// set document name
				printf( "\eX%s\e\\", file -> name );

				// update menu state
				moko_interface();

				// done
				break;
			}
		}

		// update menu state
		moko_interface();

		// done
		return TRUE;
	}

	// nope
	return FALSE;
}

uint8_t moko_key( uint16_t key ) {
	// Arrow LEFT?
	if( key == STD_KEY_ARROW_LEFT ) {
		// we are at beginning of docuemnt?
		if( ! document_line_location && ! document_line_pointer ) return TRUE;	// yes

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
				moko_document_line_refresh( FALSE );

				// new properties of cursor
				document_cursor_y--;

				// move cursor to previous line
				print( "\e[A" );
			} else {	// yes
				// view document from previous line
				document_line_number--;

				// refresh document view
				moko_document_refresh();
			}

			// search for previous line beginning
			while( document_line_location && document_area[ --document_line_location - 1 ] != STD_ASCII_NEW_LINE );

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
		moko_document_line_refresh( TRUE );

		// done
		return TRUE;
	}

	// Arrow RIGHT?
	if( key == STD_KEY_ARROW_RIGHT ) {
		// we are at end of docuemnt?
		if( document_line_location + document_line_pointer == document_size ) return TRUE;	// yes

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
			moko_document_line_refresh( FALSE );

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
				moko_document_refresh();
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
		moko_document_line_refresh( TRUE );

		// done
		return TRUE;
	}

	// Arrow DOWN?
	if( key == STD_KEY_ARROW_DOWN ) {
		// it's a last line of document?
		if( document_line_location + document_line_size == document_size ) return TRUE;	// yes

		// we are at end of document view?
		if( document_cursor_y == (stream_meta.height - menu_height_line) - 1 ) {
			// view document from next line
			document_line_number++;

			// refresh document view
			moko_document_refresh();
		} else {
			// reset properties of current line
			document_line_indicator = 0;

			// show new state of line on screen
			moko_document_line_refresh( FALSE );

			// new properties of cursor
			document_cursor_y++;

			// move cursor to next line
			print( "\e[B" );
		}

		// check next line size
		document_line_location += document_line_size + 1;
		document_line_size = lib_string_length_line( (uint8_t *) &document_area[ document_line_location ] );

		// try saved line properties
		moko_document_line_restore();

		// done
		return TRUE;
	}

	// Arrow UP?
	if( key == STD_KEY_ARROW_UP ) {
		// it's a first line of document?
		if( ! document_line_location ) return TRUE;	// yes

		// we are at beginning of document view?
		if( ! document_cursor_y ) {
			// view document from next line
			document_line_number--;

			// refresh document view
			moko_document_refresh();
		} else {
			// reset properties of current line
			document_line_indicator = 0;

			// show new state of line on screen
			moko_document_line_refresh( FALSE );

			// new properties of cursor
			document_cursor_y--;

			// move cursor to previous line
			print( "\e[A" );
		}

		// search for previous line beginning
		while( document_line_location && document_area[ --document_line_location - 1 ] != STD_ASCII_NEW_LINE );

		// check previous line size
		document_line_size = lib_string_length_line( (uint8_t *) &document_area[ document_line_location ] );

		// try saved line properties
		moko_document_line_restore();

		// done
		return TRUE;
	}

	// HOME?
	if( key == STD_KEY_HOME ) {
		// we are at beginning of line?
		if( ! document_line_pointer ) return TRUE;	// not now

		// new properties of line
		document_line_pointer = 0;
		document_line_indicator = 0;

		// update cursor position
		document_cursor_x = 0;

		// remember current pointer and indicator position for cursor at X axis
		document_line_pointer_saved = document_line_pointer;
		document_line_indicator_saved = document_line_indicator;

		// show new state of line on screen
		moko_document_line_refresh( TRUE );

		// done
		return TRUE;
	}

	// END?
	if( key == STD_KEY_END ) {
		// we are at end of line?
		if( document_line_pointer == document_line_size ) return TRUE;	// not now

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
		moko_document_line_refresh( TRUE );

		// done
		return TRUE;
	}

	// BACKSPACE?
	if( key == STD_KEY_BACKSPACE ) {
		// we are at beginning of docuemnt?
		if( ! document_line_location && ! document_line_pointer ) return TRUE;	// yes

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
			if( ! document_line_location ) return TRUE;	// nothing to do

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
			while( document_line_location && document_area[ --document_line_location - 1 ] != STD_ASCII_NEW_LINE );

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
			moko_document_refresh();
		}

		// remember current pointer and indicator position for cursor at X axis
		document_line_pointer_saved = document_line_pointer;
		document_line_indicator_saved = document_line_indicator;

		// show new state of line on screen
		moko_document_line_refresh( TRUE );

		// update menu state
		moko_interface();

		// done
		return TRUE;
	}

	// DELETE?
	if( key == STD_KEY_DELETE ) {
		// we are at end of docuemnt?
		if( document_line_location + document_line_pointer == document_size ) return TRUE;	// yes

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
			moko_document_refresh();
		}

		// show new state of line on screen
		moko_document_line_refresh( TRUE );

		// update menu state
		moko_interface();

		// done
		return TRUE;
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
		moko_document_refresh();

		// try saved line properties
		moko_document_line_restore();

		// done
		return TRUE;
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
		moko_document_refresh();

		// try saved line properties
		moko_document_line_restore();

		// done
		return TRUE;
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
		document_area[ document_line_location + document_line_pointer ] = STD_ASCII_NEW_LINE;

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
		moko_document_refresh();

		// refresh current line staste
		moko_document_line_refresh( TRUE );

		// remember current pointer and indicator position for cursor at X axis
		document_line_pointer_saved = document_line_pointer;
		document_line_indicator_saved = document_line_indicator;

		// done
		return TRUE;
	}

	// nothing available
	return FALSE;
}