/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	struct STD_STREAM_STRUCTURE_META stream_meta;
	struct STD_FILE_STRUCTURE file = { EMPTY };

	uint8_t *document = EMPTY;
	uint64_t docuemnt_index = EMPTY;
	uint64_t document_length = EMPTY;

	uint8_t string_menu[] = "^x Exit";

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// retrieve stream meta data
	std_stream_get( (uint8_t *) &stream_meta, STD_STREAM_OUT );

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
		// alloc area for file content
		document = malloc( file.length_byte );

		// load file content into document area
		std_file_read( (struct STD_FILE_STRUCTURE *) &file, (uintptr_t) document );
	} else
		// prepare new document area
		document = malloc( TRUE );

	// clear screen
	print( "\e[2J" );

	

	// process ended properly
	return EMPTY;
}
