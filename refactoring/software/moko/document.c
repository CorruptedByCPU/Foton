/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void moko_document_refresh( void ) {
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
	moko_interface();
}

void moko_document_line_refresh( uint8_t current ) {
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

void moko_document_line_restore( void ) {
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
	moko_document_line_refresh( TRUE );
}

void moko_document_parse( void ) {
	// show document content
	moko_document_refresh();

	// default variables of loaded document
	document_line_location = 0;
	document_line_indicator = 0;
	document_line_pointer = 0;
	document_line_size = lib_string_length_line( document_area );
	document_cursor_x = 0;

	// count lines of loaded document
	document_line_count = -1;	// we are counting from ZERO
	uint64_t i = 0; do {
		// by default acquired first line
		document_line_count++;

		// next line of document
		i += lib_string_length_line( (uint8_t *) &document_area[ i ] );
	} while( document_area[ i++ ] );
}

void moko_document_insert( uint16_t key ) {
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
	moko_document_line_refresh( TRUE );

	// document size
	document_size++;

	// remember current pointer and indicator position for cursor at X axis
	document_line_pointer_saved = document_line_pointer;
	document_line_indicator_saved = document_line_indicator;

	// update interface status
	moko_interface();
}