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
		log( "nadchodzi nowy dzien (%u)\n", std_microtime() );

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

		struct WM_STRUCTURE_ZONE *zone = wm -> zone;
		log( "faza 1: lista przestrzeni liczy %u stref(y)\n", wm -> zone_limit );
		for( uint64_t i = 0; i < wm -> zone_limit; i++ ) { log( " przestrzen (%u) x:%u, w:%u, y:%u, h:%u - nazwa wypelniania ", i, zone[ i ].x, zone[ i ].width, zone[ i ].y, zone[ i ].height ); if( zone[ i ].object ) log( "'%s'\n", zone[ i ].object -> descriptor -> name ); else log( "BRAK\n" ); }

		// remove any overlapping
		wm_zone_substract();

		log( "faza 2: lista przestrzeni liczy %u stref(y)\n", wm -> zone_limit );
		for( uint64_t i = 0; i < wm -> zone_limit; i++ ) { log( " przestrzen (%u) x:%u, w:%u, y:%u, h:%u - nazwa wypelniania ", i, zone[ i ].x, zone[ i ].width, zone[ i ].y, zone[ i ].height ); if( zone[ i ].object ) log( "'%s'\n", zone[ i ].object -> descriptor -> name ); else log( "BRAK\n" ); }

		// assign objects to zones
		wm_zone();

		log( "faza 3: lista przestrzeni liczy %u stref(y)\n", wm -> zone_limit );
		for( uint64_t i = 0; i < wm -> zone_limit; i++ ) { log( " przestrzen (%u) x:%u, w:%u, y:%u, h:%u - nazwa wypelniania ", i, zone[ i ].x, zone[ i ].width, zone[ i ].y, zone[ i ].height ); if( zone[ i ].object ) log( "'%s'\n", zone[ i ].object -> descriptor -> name ); else log( "BRAK\n" ); }

		// fill zones with fragments of objects
		wm_fill();

		// synchronize workbench with framebuffer
		wm_sync();
	}
}
