/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kuro_dir_add( struct STD_STRUCTURE_DIR *dir, struct LIB_UI_STRUCTURE_ELEMENT_TABLE_ROW *row ) {
	// insert new entries
	uint64_t file = FALSE;
	while( dir[ ++file ].type ) {
		// extend table content by row
		row = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE_ROW *) realloc( row, table_row * sizeof( struct LIB_UI_STRUCTURE_ELEMENT_TABLE_ROW ) );

		// default flag state
		row[ table_row ].flag = EMPTY;	// none

		// assign area for row
		row[ table_row ].cell = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE_CELL *) calloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_TABLE_CELL ) << STD_SHIFT_2 );

		//
		row[ table_row ].reserved = kuro_icon_assign( (struct STD_STRUCTURE_DIR *) &dir[ file ] );

		// column 0 --------------------------------------------

		// assign proper icon
		row[ table_row ].cell[ 0 ].icon = kuro_icon[ row[ table_row ].reserved ];

		// set cell: name
		// only if thats not a special link
		// if( file != TRUE ) {
			row[ table_row ].cell[ 0 ].name = (uint8_t *) calloc( dir[ file ].name_limit + TRUE );
			for( uint64_t i = 0; i < dir[ file ].name_limit; i++ ) row[ table_row ].cell[ 0 ].name[ i ] = dir[ file ].name[ i ];
		// }

		// column 1 --------------------------------------------

		// align content to right
		row[ table_row ].cell[ 1 ].flag = LIB_FONT_FLAG_ALIGN_right;

		// set cell: size
		// only if thats not a special link or directory
		if( file != TRUE && dir[ file ].type != STD_FILE_TYPE_directory ) {
			row[ table_row ].cell[ 1 ].name = (uint8_t *) calloc( lib_integer_digit_count( dir[ file ].limit, STD_NUMBER_SYSTEM_decimal ) + TRUE );
			lib_integer_to_string( dir[ file ].limit, STD_NUMBER_SYSTEM_decimal, row[ table_row ].cell[ 1 ].name );
		}

		//------------------------------------------------------

		// row created
		table_row++;
	}
}

void kuro_dir( void ) {
	// retrieve list of files inside current directory
	struct STD_STRUCTURE_DIR *dir = (struct STD_STRUCTURE_DIR *) std_dir( (uint8_t *) ".", TRUE );

	// count amount of entries
	uint64_t file_count = EMPTY;
	while( dir[ ++file_count ].type );

	// arrange files in alphabetical order starting from directories
	kuro_dir_sort( (struct STD_STRUCTURE_DIR *) &dir[ TRUE ], file_count - TRUE );	// ignore first entry, not usable for us

	// there is no header?
	if( ! table_header ) {
		// create one
		table_header = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE_HEADER *) calloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_TABLE_HEADER ) << STD_SHIFT_2 );

		// column 0
		uint8_t column_name[] = "File name:";
		table_header[ FALSE ].cell.name	= (uint8_t *) malloc( sizeof( column_name ) );
		for( uint8_t i = 0; i < sizeof( column_name ); i++ ) table_header[ FALSE ].cell.name[ i ] = column_name[ i ];

		table_header[ FALSE ].cell.flag = LIB_FONT_FLAG_WEIGHT_bold;

		// column 1
		uint8_t column_size[] = "Bytes:";
		table_header[ TRUE ].cell.name	= (uint8_t *) malloc( sizeof( column_size ) );
		for( uint8_t i = 0; i < sizeof( column_size ); i++ ) table_header[ TRUE ].cell.name[ i ] = column_size[ i ];

		table_header[ TRUE ].cell.flag	= LIB_FONT_FLAG_WEIGHT_bold | LIB_FONT_FLAG_ALIGN_right;
	}

	// there is no table content?
	if( ! table_content ) {
		// create one
		table_content = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE_ROW *) malloc( FALSE );

		// insert new entries
		kuro_dir_add( dir, table_content );

		// add table to ui interface
		table_id = lib_ui_add_table( ui, LIB_UI_MARGIN_DEFAULT, LIB_UI_HEADER_HEIGHT, -1, -1, EMPTY, table_header, table_content, 2, table_row );
	} else {
		// release old table content
		for( uint64_t y = 0; y < table_row; y++ ) {
			for( uint64_t x = 0; x < 2; x++ )
				// name
				if( table_content[ y ].cell[ x ].name ) free( table_content[ y ].cell[ x ].name );

			// and cell itself
			free( table_content[ y ].cell );
		}

		// no entries by default
		table_row = EMPTY;

		// truncate table content
		table_content = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE_ROW *) realloc( table_content, FALSE );

		// insert new entries
		kuro_dir_add( dir, table_content );

		// update table properties
		lib_ui_update_table( ui, table_id, table_content, table_row );
	}

	// sync all interface elements with window
	lib_ui_flush( ui );	// even window name

	// window ready for show up
	ui -> window -> flags |= LIB_WINDOW_FLAG_flush;
}

uint8_t kuro_dir_compare( struct STD_STRUCTURE_DIR *first, struct STD_STRUCTURE_DIR *second ) {
	// minimal length
	size_t length = first -> name_limit;
	if( second -> name_limit < length ) length = second -> name_limit;

	// check which name is lower in alphabetical order
	for( size_t i = 0; i < length; i++ ) {
		// retrieve characters
		uint8_t first_char = first -> name[ i ]; if( first_char > '`' && first_char < '{' ) first_char -= STD_ASCII_SPACE;
		uint8_t second_char = second -> name[ i ]; if( second_char > '`' && second_char < '{' ) second_char -= STD_ASCII_SPACE;

		// compare characters, return higher
		if( first_char > second_char ) return TRUE;
		else return FALSE;
	}

	// both are equal
	return FALSE;
}

// bubble sort, thats only for pre-alpha release, so why not?
void kuro_dir_sort( struct STD_STRUCTURE_DIR *entries, uint64_t n ) {
	// prepare area for separated directories and files
	struct STD_STRUCTURE_DIR *directories = (struct STD_STRUCTURE_DIR *) malloc( (n + 1) * sizeof( struct STD_STRUCTURE_DIR ) );
	struct STD_STRUCTURE_DIR *files = (struct STD_STRUCTURE_DIR *) malloc( (n + 1) * sizeof( struct STD_STRUCTURE_DIR ) );

	// separate directories from other files
	uint64_t directory = EMPTY;
	uint64_t file = EMPTY;
	for( uint64_t i = 1; i < n; i++ ) {
		// based on type
		switch( entries[ i ].type ) {
			// for directories
			case STD_FILE_TYPE_directory: { directories[ directory++ ] = entries[ i ]; break; }
			// and other files
			default: files[ file++ ] = entries[ i ];
		}
	}

	// reserve infinity for bubble sort
	uint8_t loop = FALSE;

	// sort directories
	while( directory && ! loop ) {
		// until raedy
		loop = TRUE;

		// compare all entries
		for( uint64_t i = 0; i < directory; i++ ) {
			// if first entry name is higher alphabetically than second
			if( kuro_dir_compare( (struct STD_STRUCTURE_DIR *) &directories[ i ], (struct STD_STRUCTURE_DIR *) &directories[ i + 1 ] ) ) {
				// replace them
				struct STD_STRUCTURE_DIR tmp = directories[ i ]; directories[ i ] = directories[ i + 1 ]; directories[ i + 1 ] = tmp;

				// and check all entries again
				loop = FALSE;
			}
		}
	}

	// sort other files
	loop = FALSE; while( file && ! loop ) {
		// until raedy
		loop = TRUE;

		// compare all entries
		for( uint64_t i = 0; i < file; i++ ) {
			// if first entry name is higher alphabetically than second
			if( kuro_dir_compare( (struct STD_STRUCTURE_DIR *) &files[ i ], (struct STD_STRUCTURE_DIR *) &files[ i + 1 ] ) ) {
				// replace them
				struct STD_STRUCTURE_DIR tmp = files[ i ]; files[ i ] = files[ i + 1 ]; files[ i + 1 ] = tmp;

				// and check all entries again
				loop = FALSE;
			}
		}
	}

	// combine results
	for( uint64_t i = 0; i < directory; i++ ) entries[ i + 1 ] = directories[ i ];
	for( uint64_t i = 0; i < file; i++ ) entries[ i + 1 + directory ] = files[ i ];
}
