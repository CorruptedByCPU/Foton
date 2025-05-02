/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/interface.h"
	#include	"../library/window.h"
	//----------------------------------------------------------------------

	//----------------------------------------------------------------------
	// structures, definitions
	//----------------------------------------------------------------------
	#include	"so/config.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"so/data.c"
	//----------------------------------------------------------------------

uint64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// create interface instance
	struct LIB_INTERFACE_STRUCTURE *so_interface = (struct LIB_INTERFACE_STRUCTURE *) malloc( sizeof( struct LIB_INTERFACE_STRUCTURE ) );

	// set default properties of our interface
	so_interface -> x = STD_MAX_unsigned;	// window at center of desktop
	so_interface -> y = STD_MAX_unsigned;
	so_interface -> width = 320;
	so_interface -> height = 240;

	// initialize interface
	lib_interface( so_interface );

	// add window name
	for( uint8_t i = 0; i < sizeof( so_window_name ) - 1; i++ ) so_interface -> descriptor -> name[ so_interface -> descriptor -> name_length++ ] = so_window_name[ i ];
	lib_interface_name_rewrite( so_interface );

	// allow window to be resiable
	so_interface -> descriptor -> flags = LIB_WINDOW_FLAG_resizable;

	// update window content on screen
	so_interface -> descriptor -> flags |= LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_flush;

	// main loop
	while( TRUE ) {
		// check incomming events
		struct LIB_INTERFACE_STRUCTURE *new = EMPTY;
		if( (new = lib_interface_event( so_interface )) ) {
			// update interface pointer
			so_interface = new;

			// update window content on screen
			so_interface -> descriptor -> flags |= LIB_WINDOW_FLAG_resizable | LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_flush;
		}

		// recieve key
		uint16_t key = getkey();

		// exit game?
		if( key == STD_ASCII_ESC ) return 0;	// yes

		// release CPU ticks
		sleep( TRUE );
	}

	// done
	return 0;
}
