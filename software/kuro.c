/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/elf.h"
	#include	"../library/image.h"
	#include	"../library/interface.h"
	#include	"../library/string.h"
	#include	"../library/type.h"
	#include	"../library/vfs.h"
	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#include	"./kuro/icon.h"
	#include	"./kuro/list.h"
	#include	"./kuro/storage.h"
	//----------------------------------------------------------------------
	// variables, routines, procedures
	//----------------------------------------------------------------------
	#include	"./kuro/config.h"
	#include	"./kuro/data.c"
	#include	"./kuro/icon.c"
	#include	"./kuro/init.c"
	#include	"./kuro/list.c"
	#include	"./kuro/storage.c"

uint8_t kuro_compare_names( struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY *first, struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY *second ) {
	// minimal length
	size_t length = first -> name_length;
	if( second -> name_length < length ) length = second -> name_length;

	// check which name is lower in alphabetical order
	for( size_t i = 0; i < length; i++ ) {
		// retrieve characters
		uint8_t first_char = first -> name[ i ]; if( first_char > '`' && first_char < '{' ) first_char -= STD_ASCII_SPACE;
		uint8_t second_char = second -> name[ i ]; if( second_char > '`' && second_char < '{' ) second_char -= STD_ASCII_SPACE;

		// compare characters, return higher
		if( first_char > second_char ) return TRUE;
		else return FALSE;
	}

	// both are equal
	return FALSE;
}

// bubble sort, thats only for pre-alpha release, so why not?
void kuro_dir_sort( struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY *entry, uint64_t local_list_entry_count ) {
	// prepare area for separated directories and files
	struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY *directories = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY *) malloc( (local_list_entry_count + 1) * sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY ) );
	struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY *files = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY *) malloc( (local_list_entry_count + 1) * sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY ) );

	// separate files
	uint64_t directory = EMPTY;
	uint64_t file = EMPTY;
	for( uint64_t i = 1; i < local_list_entry_count; i++ ) {
		// based on type
		switch( entry[ i ].type ) {
			// for directories
			case STD_FILE_TYPE_directory: { directories[ directory++ ] = entry[ i ]; break; }
			// and other files
			default: files[ file++ ] = entry[ i ];
		}
	}

	// reserve infinity for bubble sort
	uint8_t loop;

	// sort directories
	loop = FALSE; while( directory && ! loop ) {
		// until raedy
		loop = TRUE;

		// compare all entries
		for( uint64_t i = 0; i < directory; i++ ) {
			// if first entry name is higher alphabetically than second
			if( kuro_compare_names( (struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY *) &directories[ i ], (struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY *) &directories[ i + 1 ] ) ) {
				// replace them
				struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY tmp = directories[ i ]; directories[ i ] = directories[ i + 1 ]; directories[ i + 1 ] = tmp;

				// and check all entries again
				loop = FALSE;
			}
		}
	}

	// sort other files
	loop = FALSE; while( file && ! loop ) {
		// until raedy
		loop = TRUE;

		// compare all entries
		for( uint64_t i = 0; i < file; i++ ) {
			// if first entry name is higher alphabetically than second
			if( kuro_compare_names( (struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY *) &files[ i ], (struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY *) &files[ i + 1 ] ) ) {
				// replace them
				struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY tmp = files[ i ]; files[ i ] = files[ i + 1 ]; files[ i + 1 ] = tmp;

				// and check all entries again
				loop = FALSE;
			}
		}
	}

	// combine results
	for( uint64_t i = 0; i < directory; i++ ) entry[ i + 1 ] = directories[ i ];
	for( uint64_t i = 0; i < file; i++ ) entry[ i + 1 + directory ] = files[ i ];
}

uint64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize Kuro environment
	kuro_init();


	// create list of entries
	// kuro_storage();

	// show content from beginning
	// kuro_storages -> offset = EMPTY;

	// // do not allow deselection, do not allow more than 1 at a time, immedietly action
	// kuro_storages -> flags = LIB_INTERFACE_ELEMENT_LIST_FLAG_persistent | LIB_INTERFACE_ELEMENT_LIST_FLAG_individual | LIB_INTERFACE_ELEMENT_LIST_FLAG_single_click;

	// // first entry selected as default
	// kuro_storages -> entry[ FALSE ].flags = LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_active;

	// // define our own colors
	// kuro_storages -> color_default = 0xFF141414;
	// kuro_storages -> color_odd = 0xFF141414;
	// kuro_storages -> color_selected = 0xFF222222;

	// // update content of list
	// lib_interface_element_file( kuro_interface, kuro_storages );

	// //----------------------------------------------------------------------

	//----------------------------------------------------------------------

	// create list of entries
	kuro_storage();

	// show content from beginning
	kuro_storages -> offset = EMPTY;

	// update content of list
	lib_interface_element_file( kuro_interface, kuro_storages );

reload:
	// reload loop
	while( TRUE ) {
		// create list of entries
		kuro_list();

		// show content from beginning
		kuro_files -> offset = EMPTY;

		// set order of files, by name and directories first
		kuro_dir_sort( kuro_files -> entry, kuro_files -> limit );

		// update content of list
		lib_interface_element_file( kuro_interface, kuro_files );

		// update window content on screen
		kuro_interface -> descriptor -> flags |= LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_resizable | LIB_WINDOW_FLAG_flush;

		// event loop
		while( TRUE ) {
			// check events from interface
			struct LIB_INTERFACE_STRUCTURE *new = EMPTY;
			if( (new = lib_interface_event( kuro_interface )) ) {
				// update interface pointer
				kuro_interface = new;

				// update window content on screen
				kuro_interface -> descriptor -> flags |= LIB_WINDOW_FLAG_flush;
			}

			// check events from keyboard
			uint16_t key = lib_interface_event_keyboard( kuro_interface );

			// keep Left CTRL semaphore
			if( key == STD_KEY_CTRL_LEFT ) kuro_key_ctrl_semaphore = TRUE;
			if( key == (STD_KEY_CTRL_LEFT | STD_KEY_RELEASE) ) kuro_key_ctrl_semaphore = FALSE;

			// // special action?
			// if( key == 'c' && kuro_key_ctrl_semaphore ) {

			// }

			// // check if there is any action required with entry
			// for( uint64_t i = 0; i < kuro_storages -> limit; i++ )
			// 	if( kuro_storages -> entry[ i ].flags & LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_run ) {
			// 		// remove flag
			// 		kuro_storages -> entry[ i ].flags &= ~LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_run;

			// 		// change storage
			// 		if( ! std_storage_select( kuro_storages -> entry[ i ].name ) ) break;

			// 		// release current entry list
			// 		kuro_release( kuro_files );

			// 		// alloc initial area for list entries
			// 		kuro_files -> entry = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY *) malloc( TRUE );

			// 		// create list of entries
			// 		kuro_files -> limit = kuro_reload( kuro_files -> entry );

			// 		// show content from beginning
			// 		kuro_files -> offset = EMPTY;

			// 		// set order of files, by name and directories first
			// 		kuro_dir_sort( kuro_files -> entry, kuro_files -> limit );

			// 		// update content of list
			// 		lib_interface_element_file( kuro_interface, kuro_files );

			// 		// update window content on screen
			// 		kuro_interface -> descriptor -> flags |= LIB_WINDOW_FLAG_flush;
			// 	}

			// check if there is any action required with entry
			for( uint64_t i = 0; i < kuro_files -> limit; i++ )
				if( kuro_files -> entry[ i ].flags & LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_run ) {
					// based on mimetype
					switch( kuro_files -> entry[ i ].mimetype ) {
						case KURO_MIMETYPE_executable: {
							// try to execute file
							std_exec( kuro_files -> entry[ i ].name, kuro_files -> entry[ i ].name_length, EMPTY, TRUE );

							// done
							break;
						}

						case KURO_MIMETYPE_up:
						case KURO_MIMETYPE_directory: {
							// change home directory
							std_cd( kuro_files -> entry[ i ].name, kuro_files -> entry[ i ].name_length );

							// done
							goto reload;
						}

			// 			case KURO_MIMETYPE_plain_text: {
			// 				// combine command with file name
			// 				uint8_t *command = malloc( TRUE );
			// 				sprintf( "console moko %s", command, kuro_files -> entry[ i ].name );

			// 				// open text file in Moko editor
			// 				std_exec( command, lib_string_length( command ), EMPTY, TRUE );

			// 				// done
			// 				break;
			// 			}

						case KURO_MIMETYPE_3d_object: {
							// combine command with file name
							uint8_t *command = malloc( TRUE );
							sprintf( "3d %s", command, kuro_files -> entry[ i ].name );

							// open object file in 3D Viewer
							std_exec( command, lib_string_length( command ), EMPTY, TRUE );

							// done
							break;
						}

			// 			case KURO_MIMETYPE_image: {
			// 				// combine command with file name
			// 				uint8_t *command = malloc( TRUE );
			// 				sprintf( "image %s", command, kuro_files -> entry[ i ].name );

			// 				// open object file in Image Viewer
			// 				std_exec( command, lib_string_length( command ), EMPTY, TRUE );

			// 				// done
			// 				break;
			// 			}
					}

					// remove flag
					kuro_files -> entry[ i ].flags &= ~LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_run;
				}

			// exit?
			if( key == STD_ASCII_ESC ) exit();	// yes

			// release CPU time
			sleep( TRUE );
		}
	}

	// exit
	return 0;
}
