/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void moko_event( void ) {
	// old stream meta
	struct STD_STREAM_STRUCTURE_META meta = stream_meta;

	// retrieve new stream meta data
	std_stream_get( (uint8_t *) &stream_meta, STD_STREAM_OUT );

	// nothing changed?
	if( meta.width == stream_meta.width && meta.height == stream_meta.height ) return;	// ok

	// update cursor movement sequence for interaction and menu
	sprintf( "\e[%u;%uH", (uint8_t *) &string_cursor_at_interaction, 0, stream_meta.height - 2 );
	sprintf( "\e[%u;%uH", (uint8_t *) &string_cursor_at_menu, 0, stream_meta.height - 1 );

	// TODO, try to keep properties up to date :)

	// cannot fit cursor inside line?
	if( stream_meta.width < document_cursor_x ) {
		// reset cursor position on line
		document_line_pointer = 0;
		document_line_pointer_saved = 0;
		document_line_indicator = 0;
		document_line_indicator_saved = 0;
		document_cursor_x = 0;
	}

	// cannot fit cursor inside page?
	if( stream_meta.height < document_cursor_y ) {
		// reset cursor position on document
		document_line_location = 0;
		document_line_number = 0;
		document_cursor_y = 0;
		document_line_size = lib_string_length_line( (uint8_t *) &document_area[ document_line_location ] );
	}

	// set cursor at current line of page
	printf( "\e[%u;%uH", document_cursor_x, document_cursor_y );

	// reload document view
	moko_document_refresh();
}