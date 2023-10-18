/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/font.h"
	#include	"../library/interface.h"
	#include	"../library/terminal.h"
	//----------------------------------------------------------------------
	// variables, structures, definitions of Graphical User Interface
	//----------------------------------------------------------------------
	#include	"./gui/config.h"
	#include	"./wm/config.h"
	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#include	"./console/config.h"
	//----------------------------------------------------------------------
	// variables, routines, procedures
	//----------------------------------------------------------------------
	#include	"./console/data.c"
	#include	"./console/init.c"

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize console window
	console_init();

	// hold the door
	while( TRUE );
}