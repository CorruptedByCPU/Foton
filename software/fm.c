/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/image.h"
	#include	"../library/interface.h"

MACRO_IMPORT_FILE_AS_ARRAY( interface, "./software/fm/interface.json" );

struct LIB_INTERFACE_STRUCTURE	fm_interface;

void close( void ) {
	// end of program
	exit();
}

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize interface library
	fm_interface.properties = (uint8_t *) &file_interface_start;
	lib_interface( (struct LIB_INTERFACE_STRUCTURE *) &fm_interface );

	// find control element of type: close
	struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *control = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *) lib_interface_element_by_id( (struct LIB_INTERFACE_STRUCTURE *) &fm_interface, 0 );

	// assign executable function to element
	control -> event = (void *) close;

	// update window content on screen
	fm_interface.descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;

	// main loop
	while( TRUE ) {
		// free up AP time
		std_sleep( TRUE );

		// check events from interface
		lib_interface_event( (struct LIB_INTERFACE_STRUCTURE *) &fm_interface, FALSE );
	}

	// end of execution
	return 0;
}