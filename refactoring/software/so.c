/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/image.h"
	#include	"../library/interface.h"

MACRO_IMPORT_FILE_AS_ARRAY( interface, "./software/so/interface.json" )

struct LIB_INTERFACE_STRUCTURE	welcome_interface;

void close( void ) {
	// end of program
	exit();
}

uint64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize interface library
	welcome_interface.properties = (uint8_t *) &file_interface_start;
	if( ! lib_interface( (struct LIB_INTERFACE_STRUCTURE *) &welcome_interface ) ) { exit(); }

	// find control element of type: close
	struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *control = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *) lib_interface_element_by_id( (struct LIB_INTERFACE_STRUCTURE *) &welcome_interface, 0 );

	// assign executable function to element
	control -> event = close;

	// update window content on screen
	welcome_interface.descriptor -> flags |= LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_flush;

	// main loop
	while( TRUE ) {
		// free up AP time
		sleep( TRUE );

		// check events from interface
		lib_interface_event( (struct LIB_INTERFACE_STRUCTURE *) &welcome_interface );

		// check events from keyboard
		uint16_t key = lib_interface_event_keyboard( (struct LIB_INTERFACE_STRUCTURE *) &welcome_interface );

		// exit?
		if( key == STD_ASCII_ESC ) break;	// yes
	}

	// end of execution
	return 0;
}
