/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#include	"./wm/config.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"./wm/data.c"
	#include	"./wm/event.c"
	#include	"./wm/sync.c"
	#include	"./wm/fill.c"
	#include	"./wm/zone.c"
	#include	"./wm/object.c"
	#include	"./wm/init.c"

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize environment
	wm_init();

	// hold the door
	while( TRUE ) {
		// check for incomming events
		wm_event();

		// which objects have been recently updated?
		wm_object();

		// assign objects to zones
		wm_zone();

		// fill zones with fragments of objects
		wm_fill();

		// synchronize workbench with framebuffer
		wm_sync();
	}
}
