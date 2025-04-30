/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
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
	// create window
	so_window = (struct LIB_WINDOW_DESCRIPTOR *) lib_window( -1, -1, SO_WINDOW_WIDTH, SO_WINDOW_HEIGHT );

	// set window name
	for( uint8_t i = 0; i < sizeof( so_window_name ); i++ ) so_window -> name[ so_window -> name_length++ ] = so_window_name[ i ];

	// debug
	uint32_t *pixel_a = (uint32_t *) ((uintptr_t) so_window + sizeof( struct LIB_WINDOW_DESCRIPTOR ));
	for( uint16_t y = 0; y < SO_WINDOW_HEIGHT; y++ ) for( uint16_t x = 0; x < SO_WINDOW_WIDTH; x++ ) pixel_a[ (y * SO_WINDOW_WIDTH) + x ] = 0xFFFF0000;

	// window ready, flush!
	so_window -> flags = STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_resizable | STD_WINDOW_FLAG_flush;

	// main loop
	while( TRUE ) {
		// release CPU ticks
		sleep( TRUE );
	}

	// done
	return 0;
}
