/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// structures, definitions
	//----------------------------------------------------------------------
	#include	"./wm/config.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"./wm/data.c"
	//----------------------------------------------------------------------
	// functions / procedures
	//----------------------------------------------------------------------
	#include	"./wm/event.c"
	#include	"./wm/sync.c"
	#include	"./wm/fill.c"
	#include	"./wm/zone.c"
	#include	"./wm/object.c"
	#include	"./wm/taskbar.c"
	#include	"./wm/release.c"
	#include	"./wm/clock.c"
	#include	"./wm/init.c"
	#include	"./wm/cursor.c"
	#include	"./wm/menu.c"
	// #include	"./wm/workbench.c"

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize environment
	if( ! wm_init() ) return -1;

	// hold the door
	while( TRUE ) {
		// check for incomming events
		wm_event();

		// which objects have been recently updated?
		wm_object();

		// remove object no more used
		wm_release();

		// assign objects to zones
		wm_zone();

		// fill zones with fragments of objects
		wm_fill();

		// show cursor
		wm_cursor();

		// synchronize workbench with framebuffer
		wm_sync();

		// free up AP time
		sleep( TRUE );
	}
}
