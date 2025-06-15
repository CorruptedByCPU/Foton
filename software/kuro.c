/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/image.h"
	#include	"../library/integer.h"
	#include	"../library/string.h"
	#include	"../library/ui.h"
	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#include	"./kuro/config.h"
	#include	"./kuro/dir.h"
	#include	"./kuro/icon.h"
	//----------------------------------------------------------------------
	// variables, routines, procedures
	//----------------------------------------------------------------------
	#include	"./kuro/data.c"
	#include	"./kuro/dir.c"
	#include	"./kuro/icon.c"
	#include	"./kuro/init.c"
	//----------------------------------------------------------------------

uint64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize Kuro environment
	kuro_init();

reload:
	// reload loop
	while( TRUE ) {
		// add content to table
		kuro_dir();

		// event loop
		while( TRUE ) {
			// check for
			lib_ui_event( ui );

			// check if there is any action required with entry of table
			for( uint64_t i = 0; i < table_row; i++ ) {
				if( table_content[ i ].flag & LIB_UI_ELEMENT_FLAG_event ) {
					// based on mimetype
					switch( table_content[ i ].reserved ) {
						case UP:
						case DIRECTORY: {
							// // change home directory
							// std_cd( table_content[ i ].cell[ 0 ].name, lib_string_length( table_content[ i ].cell[ 0 ].name ) );

							// // done
							// goto reload;
						}
					}
				}

				// remove flag
				table_content[ i ].flag &= ~LIB_UI_ELEMENT_FLAG_event;
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
