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
	// variables, routines, procedures
	//----------------------------------------------------------------------
	#include	"./kuro/config.h"
	#include	"./kuro/data.c"

void kuro_close( void ) {
	// end of program
	exit();
}

size_t kuro_reload( struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY *entry ) {
	// properties of directory
	FILE *dir = EMPTY;
	struct LIB_VFS_STRUCTURE *vfs = EMPTY;

	// get directory properties
	if( ! (dir = fopen( (uint8_t *) ".", EMPTY )) ) { log( "Critical error!\n" ); return STD_ERROR_file_not_found; }

	// assign area for directory content
	if( ! (vfs = (struct LIB_VFS_STRUCTURE *) malloc( dir -> byte )) ) { log( "No enough memory!" ); return STD_ERROR_memory_low; }

	// read directory content
	fread( dir, (uint8_t *) vfs, dir -> byte );

	// default
	size_t local_list_entry_count = FALSE;

	// file by file
	uint64_t e = EMPTY; while( vfs[ ++e ].name_length ) {
		// prepare area for entry
		entry = realloc( entry, (local_list_entry_count + 1) * sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY ) );

		// create new list entry
		if( ! local_list_entry_count ) entry[ local_list_entry_count ].flags = LIB_INTERFACE_ELEMENT_LIST_FLAG_select;
		else entry[ local_list_entry_count ].flags = EMPTY;

		// define entry type and size
		entry[ local_list_entry_count ].type = vfs[ e ].type;
		entry[ local_list_entry_count ].byte = vfs[ e ].byte;

		// copy entry name
		entry[ local_list_entry_count ].name_length = vfs[ e ].name_length;
		entry[ local_list_entry_count ].name = calloc( entry[ local_list_entry_count ].name_length + 1 );
		for( size_t i = 0; i < entry[ local_list_entry_count ].name_length; i++ ) entry[ local_list_entry_count ].name[ i ] = vfs[ e ].name[ i ];

		// set icon
		switch( vfs[ e ].type ) {
			case STD_FILE_TYPE_link: {
				// special purpose?
				if( e == TRUE ) {
					// yes
					entry[ local_list_entry_count ].mimetype = KURO_MIMETYPE_up;
					entry[ local_list_entry_count ].icon = kuro_icon[ KURO_MIMETYPE_up ];
				}
				
				// done
				break;
			}
			case STD_FILE_TYPE_directory: {
				// load directory icon, if not present
				if( ! kuro_icon[ KURO_MIMETYPE_directory ] ) kuro_icon_register( KURO_MIMETYPE_directory, (uint8_t *) "/system/var/gfx/icons/folder-green.tga" );

				// set directory icon
				entry[ local_list_entry_count ].mimetype = KURO_MIMETYPE_directory;
				entry[ local_list_entry_count ].icon = kuro_icon[ KURO_MIMETYPE_directory ];
				
				// done
				break;
			}

			default: {
				// load default icon, if not present
				if( ! kuro_icon[ KURO_MIMETYPE_unknown ] ) kuro_icon_register( KURO_MIMETYPE_unknown, (uint8_t *) "/system/var/gfx/icons/unknown.tga" );

				// set default icon
				entry[ local_list_entry_count ].mimetype = KURO_MIMETYPE_unknown;
				entry[ local_list_entry_count ].icon = kuro_icon[ KURO_MIMETYPE_unknown ];
				
				// properties of file
				struct LIB_ELF_STRUCTURE *elf = (struct LIB_ELF_STRUCTURE *) malloc( sizeof( struct LIB_ELF_STRUCTURE ) );

				// retrieve part of file content
				FILE *file = fopen( entry[ local_list_entry_count ].name, EMPTY ); fread( file, (uint8_t *) elf, sizeof( struct LIB_ELF_STRUCTURE ) ); fclose( file );

				// file type of ELF
				if( lib_elf_identify( (uintptr_t) elf ) ) {	// yes
					// library?
					if( elf -> type == LIB_ELF_TYPE_shared_object ) {
						// load library icon, if not present
						if( ! kuro_icon[ KURO_MIMETYPE_library ] ) kuro_icon_register( KURO_MIMETYPE_library, (uint8_t *) "/system/var/gfx/icons/application-x-sharedlib.tga" );

						// set library icon
						entry[ local_list_entry_count ].mimetype = KURO_MIMETYPE_library;
						entry[ local_list_entry_count ].icon = kuro_icon[ KURO_MIMETYPE_library ];
					}
					
					// module?
					else if( entry[ local_list_entry_count ].name_length > 3 && lib_string_compare( (uint8_t *) &entry[ local_list_entry_count ].name[ entry[ local_list_entry_count ].name_length - 3 ], (uint8_t *) ".ko", 3 ) ) {
						// load module icon, if not present
						if( ! kuro_icon[ KURO_MIMETYPE_module ] ) kuro_icon_register( KURO_MIMETYPE_module, (uint8_t *) "/system/var/gfx/icons/application-octet-stream.tga" );

						// set module icon
						entry[ local_list_entry_count ].mimetype = KURO_MIMETYPE_module;
						entry[ local_list_entry_count ].icon = kuro_icon[ KURO_MIMETYPE_module ];
					}
					
					// executable
					else {
						// load module icon, if not present
						if( ! kuro_icon[ KURO_MIMETYPE_executable ] ) kuro_icon_register( KURO_MIMETYPE_executable, (uint8_t *) "/system/var/gfx/icons/application-x-executable.tga" );

						// set executable icon
						entry[ local_list_entry_count ].mimetype = KURO_MIMETYPE_executable;
						entry[ local_list_entry_count ].icon = kuro_icon[ KURO_MIMETYPE_executable ];
					}
				}

				// image extension?
				if( entry[ local_list_entry_count ].name_length > 4 && lib_string_compare( (uint8_t *) &entry[ local_list_entry_count ].name[ entry[ local_list_entry_count ].name_length - 4 ], (uint8_t *) ".tga", 4 ) ) {
					// load image icon, if not present
					if( ! kuro_icon[ KURO_MIMETYPE_image ] ) kuro_icon_register( KURO_MIMETYPE_image, (uint8_t *) "/system/var/gfx/icons/image.tga" );

					// set image icon
					entry[ local_list_entry_count ].mimetype = KURO_MIMETYPE_image;
					entry[ local_list_entry_count ].icon = kuro_icon[ KURO_MIMETYPE_image ];
				}

				// plain text?
				if(
					// .txt
					entry[ local_list_entry_count ].name_length > 4 && lib_string_compare( (uint8_t *) &entry[ local_list_entry_count ].name[ entry[ local_list_entry_count ].name_length - 4 ], (uint8_t *) ".txt", 4 ) ||
					// .log
					entry[ local_list_entry_count ].name_length > 4 && lib_string_compare( (uint8_t *) &entry[ local_list_entry_count ].name[ entry[ local_list_entry_count ].name_length - 4 ], (uint8_t *) ".log", 4 ) ||
					// .cfg
					entry[ local_list_entry_count ].name_length > 4 && lib_string_compare( (uint8_t *) &entry[ local_list_entry_count ].name[ entry[ local_list_entry_count ].name_length - 4 ], (uint8_t *) ".cfg", 4 )
				) {
					// load text icon, if not present
					if( ! kuro_icon[ KURO_MIMETYPE_plain_text ] ) kuro_icon_register( KURO_MIMETYPE_plain_text, (uint8_t *) "/system/var/gfx/icons/text-plain.tga" );

					// set image icon
					entry[ local_list_entry_count ].mimetype = KURO_MIMETYPE_plain_text;
					entry[ local_list_entry_count ].icon = kuro_icon[ KURO_MIMETYPE_plain_text ];
				}

				// 3D object?
				if(
					// .obj
					entry[ local_list_entry_count ].name_length > 4 && lib_string_compare( (uint8_t *) &entry[ local_list_entry_count ].name[ entry[ local_list_entry_count ].name_length - 4 ], (uint8_t *) ".obj", 4 )
				) {
					// load text icon, if not present
					if( ! kuro_icon[ KURO_MIMETYPE_3d_object ] ) kuro_icon_register( KURO_MIMETYPE_3d_object, (uint8_t *) "/system/var/gfx/icons/3d.tga" );

					// set image icon
					entry[ local_list_entry_count ].mimetype = KURO_MIMETYPE_3d_object;
					entry[ local_list_entry_count ].icon = kuro_icon[ KURO_MIMETYPE_3d_object ];
				}

				// release file properties
				free( elf );
			}
		}

		// entry registered
		local_list_entry_count++;
	}

	// release directory content
	free( vfs );

	// close directory
	fclose( dir );

	return local_list_entry_count;
}

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

void kuro_icon_register( uint8_t type, uint8_t *path ) {
	// register new icon
	kuro_icon = (uint32_t **) realloc( kuro_icon, type * sizeof( uint32_t * ) );
	kuro_icon[ type ] = lib_image_scale( lib_interface_icon( path ), 48, 48, 16, 16 );
}

void kuro_release( void ) {
	// check if there is any action required with entry
	for( uint64_t i = 0; i < kuro_files -> limit; i++ )
		// release entry name area
		free( kuro_files -> entry[ i ].name );

	// release entry list
	free( kuro_files -> entry );
}

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	//----------------------------------------------------------------------

	// alloc area for interface properties
	kuro_interface = (struct LIB_INTERFACE_STRUCTURE *) malloc( sizeof( struct LIB_INTERFACE_STRUCTURE ) );

	// initialize interface library
	kuro_interface -> properties = (uint8_t *) &file_interface_start;
	if( ! lib_interface( kuro_interface ) ) { log( "Cannot create window.\n" ); exit(); }

	// set minimal window size as current
	kuro_interface -> min_width = kuro_interface -> width;
	kuro_interface -> min_height = kuro_interface -> height;

	//----------------------------------------------------------------------

	// find entry of ID: 0
	struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *control = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *) lib_interface_element_by_id( kuro_interface, 0 );
	control -> event = (void *) kuro_close;	// assign executable function to element

	//----------------------------------------------------------------------

	// initialize icon list
	kuro_icon = (uint32_t **) malloc( TRUE );

	// register initial icon (directory change)
	kuro_icon_register( KURO_MIMETYPE_up, (uint8_t *) "/system/var/gfx/icons/go-up.tga" );

	//----------------------------------------------------------------------

	// find element of ID: 1
	kuro_files = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE *) lib_interface_element_by_id( kuro_interface, 1 );

	// alloc initial area for list entries
	kuro_files -> entry = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY *) malloc( TRUE );

	// create list of entries
	kuro_files -> limit = kuro_reload( kuro_files -> entry );

	// show content from beginning
	kuro_files -> offset = EMPTY;

	// set order of files, by name and directories first
	kuro_dir_sort( kuro_files -> entry, kuro_files -> limit );

	// update content of list
	lib_interface_element_file( kuro_interface, kuro_files );

	// update window content on screen
	kuro_interface -> descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_resizable | STD_WINDOW_FLAG_flush;

	//----------------------------------------------------------------------

	// main loop
	while( TRUE ) {
		// free up AP time
		sleep( TRUE );

		// check events from interface
		struct LIB_INTERFACE_STRUCTURE *new = EMPTY;
		if( (new = lib_interface_event( kuro_interface )) ) {
			// update interface pointer
			kuro_interface = new;

			// update window content on screen
			kuro_interface -> descriptor -> flags |= STD_WINDOW_FLAG_flush;
		}

		// check events from keyboard
		uint16_t key = lib_interface_event_keyboard( kuro_interface );

		// keep Left CTRL semaphore
		if( key == STD_KEY_CTRL_LEFT ) kuro_key_ctrl_semaphore = TRUE;
		if( key == (STD_KEY_CTRL_LEFT | STD_KEY_RELEASE) ) kuro_key_ctrl_semaphore = FALSE;

		// special action?
		if( key == 'c' && kuro_key_ctrl_semaphore ) {

		}

		// check if there is any action required with entry
		for( uint64_t i = 0; i < kuro_files -> limit; i++ )
			if( kuro_files -> entry[ i ].flags & LIB_INTERFACE_ELEMENT_LIST_FLAG_run ) {
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

						// release current entry list
						kuro_release();

						// alloc initial area for list entries
						kuro_files -> entry = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY *) malloc( TRUE );

						// create list of entries
						kuro_files -> limit = kuro_reload( kuro_files -> entry );

						// show content from beginning
						kuro_files -> offset = EMPTY;

						// set order of files, by name and directories first
						kuro_dir_sort( kuro_files -> entry, kuro_files -> limit );

						// update content of list
						lib_interface_element_file( kuro_interface, kuro_files );

						// update window content on screen
						kuro_interface -> descriptor -> flags |= STD_WINDOW_FLAG_flush;

						// done
						break;
					}

					case KURO_MIMETYPE_plain_text: {
						// combine command with file name
						uint8_t *command = malloc( TRUE );
						sprintf( "console moko %s", command, kuro_files -> entry[ i ].name );

						// open text file in Moko editor
						std_exec( command, lib_string_length( command ), EMPTY, TRUE );

						// done
						break;
					}

					case KURO_MIMETYPE_3d_object: {
						// combine command with file name
						uint8_t *command = malloc( TRUE );
						sprintf( "3d %s", command, kuro_files -> entry[ i ].name );

						// open object file in 3D Viewer
						std_exec( command, lib_string_length( command ), EMPTY, TRUE );

						// done
						break;
					}

					case KURO_MIMETYPE_image: {
						// combine command with file name
						uint8_t *command = malloc( TRUE );
						sprintf( "image %s", command, kuro_files -> entry[ i ].name );

						// open object file in Image Viewer
						std_exec( command, lib_string_length( command ), EMPTY, TRUE );

						// done
						break;
					}
				}

				// remove flag
				kuro_files -> entry[ i ].flags &= ~LIB_INTERFACE_ELEMENT_LIST_FLAG_run;
			}

		// exit?
		if( key == STD_ASCII_ESC ) break;	// yes
	}

	// exit
	return 0;
}
