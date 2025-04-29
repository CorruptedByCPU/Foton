/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/color.h"
	#include	"../library/font.h"
	#include	"../library/image.h"
	#include	"../library/random.h"
	#include	"../library/window.h"
	//----------------------------------------------------------------------

	//----------------------------------------------------------------------
	// structures, definitions
	//----------------------------------------------------------------------
	#include	"tiwyn/config.h"
	#include	"tiwyn/event.h"
	#include	"tiwyn/fill.h"
	#include	"tiwyn/object.h"
	#include	"tiwyn/zone.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"tiwyn/data.c"
	//----------------------------------------------------------------------
	// functions / procedures
	//----------------------------------------------------------------------
	#include	"tiwyn/cursor.c"
	#include	"tiwyn/event.c"
	#include	"tiwyn/fill.c"
	#include	"tiwyn/init.c"
	#include	"tiwyn/object.c"
	#include	"tiwyn/sync.c"
	#include	"tiwyn/zone.c"
	//----------------------------------------------------------------------

uint64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize Desktop environment
	if( tiwyn_init() ) return STD_ERROR_locked;

	// hold the door
	while( TRUE ) {
		// check for incomming events
		tiwyn_event();

		// which objects have been recently updated?
		tiwyn_object();

		// assign objects to zones
		tiwyn_zone();

		// fill zones with fragments of objects
		tiwyn_fill();

		// show cursor
		tiwyn_cursor();

		// synchronize workbench with framebuffer
		tiwyn_sync();

		// release CPU
		sleep( TRUE );
	}
}
