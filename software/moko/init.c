/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void moko_init( uint64_t argc, uint8_t *argv[] ) {
	// initialize input library
	// struct LIB_INPUT_STRUCTURE *document_input = lib_input_init();

	// retrieve stream meta data
	std_stream_get( (uint8_t *) &stream_meta, STD_STREAM_OUT );

	// prepare cursor movement sequence for interaction and menu
	sprintf( "\e[%u;%uH", (uint8_t *) &string_cursor_at_interaction, 0, stream_meta.height - 2 );
	sprintf( "\e[%u;%uH", (uint8_t *) &string_cursor_at_menu, 0, stream_meta.height - 1 );

	// prepare area for current file_path and save_as too
	file_path = calloc( lib_integer_limit_unsigned( MACRO_SIZEOF( struct STD_STRUCTURE_FILE, name_length ) ) );
	save_as = calloc( lib_integer_limit_unsigned( MACRO_SIZEOF( struct STD_STRUCTURE_FILE, name_length ) ) );

	// prepare area for document name
	document_name = malloc( STD_FILE_NAME_limit );

	// file selected?
	if( argc > 1 )	{
		// search for file name
		for( uint64_t i = 1; i < argc; i++ ) {
			// ignore any passed options, no support yet
			if( argv[ i ][ 0 ] == '-' ) continue;
			else {
				// set document name
				sprintf( "%s", file_path, argv[ i ] );

				// open provided file
				file = fopen( file_path );

				// ignore other file names, no support yet
				break;
			}
		}
	}

	// if file exist
	if( file ) {
		// alloc area for file content
		document_area = malloc( file -> byte );
		document_size = file -> byte;

		// load file content into document area
		fread( file, document_area, file -> byte );

		// set document name
		sprintf( "%s", (uint8_t *) document_name, file -> name );
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

	// set console window header
	printf( "\eX%s\e\\", document_name );

	// show interface
	moko_interface();

	// =====================================================================

	// parse document and show it content
	moko_document_parse();

	// move cursor at beginning of document
	print( "\e[0;0H" );

	// show new state of line on screen
	moko_document_line_refresh( TRUE );
}