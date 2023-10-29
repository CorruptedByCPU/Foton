/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/image.h"
	//----------------------------------------------------------------------
	// variables, structures, definitions of Window Manager
	//----------------------------------------------------------------------
	#include	"./wm/config.h"
	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#include	"./gui/config.h"
	//----------------------------------------------------------------------
	// variables, routines, procedures
	//----------------------------------------------------------------------
	#include	"./gui/data.c"
	#include	"./gui/init.c"
	#include	"./gui/event.c"

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize desktop
	if( ! gui_init() ) return FALSE;

	// debug
	int64_t console_pid = std_exec( (uint8_t *) "console", 7, EMPTY );

	// hold the door
	while( TRUE ) {
		// check for incomming events
		gui_event();
	}
}