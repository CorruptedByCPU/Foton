/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/image.h"
	#include	"../library/interface.h"

MACRO_IMPORT_FILE_AS_ARRAY( interface, "./software/fm/interface.json" );

struct LIB_INTERFACE_STRUCTURE	*fm_interface;

void close( void ) {
	// end of program
	exit();
}

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// alloc area for interface properties
	fm_interface = (struct LIB_INTERFACE_STRUCTURE *) malloc( sizeof( struct LIB_INTERFACE_STRUCTURE ) );

	// initialize interface library
	fm_interface -> properties = (uint8_t *) &file_interface_start;
	if( ! lib_interface( fm_interface ) ) { log( "Cannot create window.\n" ); exit(); }

	// set minimal window size as current
	fm_interface -> min_width = fm_interface -> width;
	fm_interface -> min_height = fm_interface -> height;

	// find control element of type: close
	struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *control = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *) lib_interface_element_by_id( fm_interface, 0 );
	control -> event = (void *) close;	// assign executable function to element

	// update window content on screen
	fm_interface -> descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_resizable | STD_WINDOW_FLAG_flush;

	// main loop
	while( TRUE ) {
		// free up AP time
		sleep( TRUE );

		// check events from interface
		struct LIB_INTERFACE_STRUCTURE *new = EMPTY;
		if( (new = lib_interface_event( fm_interface )) ) {
			// update interface pointer
			fm_interface = new;

			// update window content on screen
			fm_interface -> descriptor -> flags |= STD_WINDOW_FLAG_resizable | STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;
		}

		// check events from keyboard
		uint16_t key = lib_interface_event_keyboard( fm_interface );

		// exit?
		if( key == STD_ASCII_ESC ) break;	// yes
	}

	// end of execution
	return 0;
}