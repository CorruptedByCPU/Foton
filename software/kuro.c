/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/image.h"
	#include	"../library/kuro.h"
	#include	"../library/string.h"
	#include	"../library/ui.h"
	//----------------------------------------------------------------------
	// static, structures, definitions
	//----------------------------------------------------------------------
	#include	"./kuro/config.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"./kuro/data.c"
	//----------------------------------------------------------------------
	// routines, procedures
	//----------------------------------------------------------------------
	#include	"./kuro/init.c"
	//----------------------------------------------------------------------

uint64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize Kuro environment
	kuro_init();

	// first execution
	uint8_t init = TRUE;

reload:
	// reload loop
	while( TRUE ) {
		// create table of files
		kuro.flag = LIB_KURO_FLAG_header;	// show Header
		kuro.flag |= LIB_KURO_FLAG_size;	// add Size column
		lib_kuro( (struct LIB_KURO_STRUCTURE *) &kuro );
	
		if( init ) {
			// add table to ui interface
			id = lib_ui_add_table( ui, LIB_UI_MARGIN_DEFAULT, LIB_UI_HEADER_HEIGHT, -1, -1, kuro.header, kuro.entries, kuro.cols, kuro.rows );

			// done
			init = FALSE;
		} else
			// update table properties
			lib_ui_update_table( ui, id, kuro.entries, kuro.rows );

		// sync all interface elements with window
		lib_ui_flush( ui );	// even window name

		// window ready for show up
		ui -> window -> flags |= LIB_WINDOW_FLAG_flush;

		// event loop
		while( TRUE ) {
			// check for
			uint16_t key = lib_ui_event( ui );

			// exit?
			if( key == STD_ASCII_ESC ) return EMPTY;	// yes

			// check if there is any action required with entry of table
			for( uint64_t i = 0; i < kuro.rows; i++ ) {
				if( kuro.entries[ i ].flag & LIB_UI_ELEMENT_FLAG_event ) {
					// based on mimetype
					switch( kuro.entries[ i ].reserved ) {
						case LIB_KURO_MIMETYPE_UP: {
							// change home directory
							std_cd( (uint8_t *) "..", 2 );

							// done
							goto reload;
						}

						case LIB_KURO_MIMETYPE_DIRECTORY: {
							// change home directory
							std_cd( kuro.entries[ i ].cell[ 0 ].name, lib_string_length( kuro.entries[ i ].cell[ 0 ].name ) );

							// done
							goto reload;
						}

						case LIB_KURO_MIMETYPE_EXECUTABLE: {
							// open object file with 3D Viewer
							std_exec( kuro.entries[ i ].cell[ 0 ].name, lib_string_length( kuro.entries[ i ].cell[ 0 ].name ), EMPTY, TRUE );

							// done
							break;
						}
					}
				}

				// remove flag
				kuro.entries[ i ].flag &= ~LIB_UI_ELEMENT_FLAG_event;
			}

			// release CPU time
			sleep( TRUE );
		}

		// release CPU time
		sleep( TRUE );
	}

	// exit
	return 0;
}
