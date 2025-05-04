/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/color.h"
	#include	"../library/font.h"
	#include	"../library/image.h"
	#include	"../library/integer.h"
	#include	"../library/window.h"
	//----------------------------------------------------------------------

	//----------------------------------------------------------------------
	// structures, definitions
	//----------------------------------------------------------------------
	#include	"wm/config.h"
	#include	"wm/fill.h"
	#include	"wm/object.h"
	#include	"wm/panel.h"
	#include	"wm/zone.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"wm/data.c"
	//----------------------------------------------------------------------
	// functions / procedures
	//----------------------------------------------------------------------
	#include	"wm/cursor.c"
	#include	"wm/event.c"
	#include	"wm/fill.c"
	#include	"wm/init.c"
	#include	"wm/object.c"
	#include	"wm/panel.c"
	#include	"wm/release.c"
	#include	"wm/sync.c"
	#include	"wm/zone.c"
	//----------------------------------------------------------------------

uint64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize Desktop environment
	wm_init();

	// hold the door
	while( TRUE ) {
		// remove obsolete objects
		wm_release();

		// check for incomming events
		wm_event();

		// refresh panel state if required
		wm_panel();

		// which objects have been recently updated?
		wm_object();

		// if nothing to parse
		if( ! wm -> zone_limit ) {
			// release CPU time
			sleep( TRUE );

			// lets go again
			continue;
		}

		// assign objects to zones
		wm_zone();

		// fill zones with fragments of objects
		wm_fill();

		// show cursor
		wm_cursor();

		// synchronize workbench with framebuffer
		wm_sync();
	}
}
