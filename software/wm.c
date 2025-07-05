/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// default
	//----------------------------------------------------------------------
	#include	"../default.h"
	//----------------------------------------------------------------------

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/color.h"
	#include	"../library/font.h"
	#include	"../library/image.h"
	#include	"../library/integer.h"
	#include	"../library/window.h"
	#include	"../library/ui.h"
	//----------------------------------------------------------------------
	// static, structures, definitions
	//----------------------------------------------------------------------
	#include	"wm/config.h"
	#include	"wm/fill.h"
	#include	"wm/menu.h"
	#include	"wm/object.h"
	#include	"wm/panel.h"
	#include	"wm/zone.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"wm/data.c"
	//----------------------------------------------------------------------
	// routines, procedures
	//----------------------------------------------------------------------
	#include	"wm/event.c"
	#include	"wm/fill.c"
	#include	"wm/init.c"
	#include	"wm/menu.c"
	#include	"wm/object.c"
	#include	"wm/panel.c"
	#include	"wm/release.c"
	#include	"wm/sync.c"
	#include	"wm/zone.c"
	//----------------------------------------------------------------------

uint64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize Desktop environment
	wm_init();

// debug
uint64_t phase = 0;

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

// debug
// log( "[%u] -----------------------------------------------------\n", phase++ );
// log( "Zone list:\n" );
// for( uint64_t i = 0; i < wm -> zone_limit; i++ ) log( "z|%u: %u, %u (%u x %u)\n", i, wm -> zone[ i ].x, wm -> zone[ i ].y, wm -> zone[ i ].width, wm -> zone[ i ].height );

		// assign objects to zones
		wm_zone();

		// fill zones with fragments of objects
		wm_fill();

		// synchronize workbench with framebuffer
		wm_sync();
	}
}
