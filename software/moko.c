/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/input.h"
	#include	"../library/integer.h"
	#include	"../library/string.h"
	#include	"../library/vfs.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"moko/data.c"
	//----------------------------------------------------------------------
	// routines, procedures
	//----------------------------------------------------------------------
	#include	"moko/config.h"
	#include	"moko/init.c"
	#include	"moko/interface.c"
	#include	"moko/document.c"
	#include	"moko/key.c"
	#include	"moko/event.c"

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize document
	moko_init( argc, argv );

	// main loop
	while( TRUE ) {
		// release cpu time
		std_sleep( TRUE );

		// check incomming events
		moko_event();

		//-------------------------------------------------------------

		// recieve key
		uint16_t key = getkey(); if( ! key ) continue;	// nothing

		// menu interaction parsed?
		if( moko_key_ctrl( key ) ) continue;	// yes

		// special key?
		if( moko_key( key ) ) continue;	// yes

		//-------------------------------------------------------------

		// check if key is printable
		if( key < STD_ASCII_SPACE || key > STD_ASCII_TILDE) continue;	// no, we are done

		// insert key inside document
		moko_document_insert( key );
	}

	// process ended properly
	return EMPTY;
}