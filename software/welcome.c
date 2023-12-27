/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/image.h"
	#include	"../library/interface.h"

MACRO_IMPORT_FILE_AS_ARRAY( interface, "./software/welcome/interface.json" );

struct LIB_INTERFACE_STRUCTURE	welcome_interface;

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize interface library
	welcome_interface.properties = (uint8_t *) &file_interface_start;
	lib_interface( (struct LIB_INTERFACE_STRUCTURE *) &welcome_interface );

	// find control element of type: close
	struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *control = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *) lib_interface_element_by_id( (struct LIB_INTERFACE_STRUCTURE *) &welcome_interface, 0 );
	if( ! control ) { return -1; }	// element not found

	// assign executable function to element
	control -> event = (void *) exit;

	// update window content on screen
	welcome_interface.descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;

	// main loop
	while( TRUE ) {
		// free up AP time
		std_sleep( TRUE );

		// check events from interface
		lib_interface_event( (struct LIB_INTERFACE_STRUCTURE *) &welcome_interface );

		// recieve key
		uint16_t key = getkey();

		// exit?
		if( key == STD_ASCII_ESC ) return 0;	// yes
	}

	// end of execution
	return 0;
}