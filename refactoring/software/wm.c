/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/color.h"
	#include	"../library/window.h"
	//----------------------------------------------------------------------

	//----------------------------------------------------------------------
	// structures, definitions
	//----------------------------------------------------------------------
	#include	"./wm/config.h"
	#include	"./wm/object.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"./wm/data.c"
	//----------------------------------------------------------------------
	// functions / procedures
	//----------------------------------------------------------------------
	#include	"./wm/event.c"
	#include	"./wm/cursor.c"
	#include	"./wm/fill.c"
	#include	"./wm/init.c"
	#include	"./wm/object.c"
	#include	"./wm/release.c"
	#include	"./wm/sync.c"
	#include	"./wm/zone.c"
	//----------------------------------------------------------------------

uint64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize Window Manager environment
	if( wm_init() ) return STD_ERROR_locked;

	// debug
	std_exec( (uint8_t *) "de", 2, EMPTY, TRUE );

	// hold the door
	while( TRUE ) {
		// remove objects no more used
		wm_release();

		// parse incomming events from input devices
		wm_event();

		// which objects have been recently updated?
		wm_object();

		// assign objects to zones
		wm_zone();

		// fill zones with fragments of objects
		wm_fill();

		// show cursor
		wm_cursor();

		// synchronize workbench with framebuffer
		wm_sync();

		// release CPU
		sleep( TRUE );
	}
}
