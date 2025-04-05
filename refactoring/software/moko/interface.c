/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void moko_interface( void ) {
	// save current cursor position and move at beginning of menu
	printf( "\e[s%s\e[2K\e[E", string_cursor_at_interaction );

	// change color of "save" option if document was malformed
	if( document_modified_semaphore ) printf( "%s", string_color_modified ); else printf( "%s", string_color_shortcut );

	// SAVE option
	print( "^o\e[0m Save " );
	
	// restore original cursor position
	print( "\e[u" );
}