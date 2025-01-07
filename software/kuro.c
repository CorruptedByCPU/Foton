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

size_t kuro_reload( struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST_ENTRY *entry ) {
	// properties of directory
	FILE *dir = EMPTY;
	uint8_t *dir_content = EMPTY;

	// get directory properties
	if( ! (dir = fopen( (uint8_t *) "." )) ) { log( "Critical error!\n" ); return STD_ERROR_file_not_found; }

	// assign area for directory content
	if( ! (dir_content = malloc( dir -> byte )) ) { log( "No enough memory!" ); return STD_ERROR_memory_low; }

	// read directory content
	fread( dir, dir_content, dir -> byte );

	// default
	size_t local_list_entry_count = FALSE;

	// block by block
	for( size_t b = 0; b < (dir -> byte >> STD_SHIFT_PAGE); b++ ) {
		// properties of directory entries
		struct LIB_VFS_STRUCTURE *vfs = (struct LIB_VFS_STRUCTURE *) ((uintptr_t) dir_content + (b << STD_SHIFT_PAGE));

		// for every possible entry
		for( size_t e = 0; e < STD_PAGE_byte / sizeof( struct LIB_VFS_STRUCTURE ); e++ ) {
			// ignore symlink of current directory
			if( vfs[ e ].name_length == TRUE && vfs[ e ].name[ FALSE ] == STD_ASCII_DOT ) continue;

			// set longest file name as column width
			if( vfs[ e ].name_length ) {
				// prepare area for entry
				entry = realloc( entry, (local_list_entry_count + 1) * sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST_ENTRY ) );

				// create new list entry
				entry[ local_list_entry_count ].flags = EMPTY;
				entry[ local_list_entry_count ].byte = vfs[ e ].byte;

				// define entry type
				entry[ local_list_entry_count ].type = vfs[ e ].type;

				// copy entry name
				entry[ local_list_entry_count ].name_length = vfs[ e ].name_length;
				entry[ local_list_entry_count ].name = calloc( entry[ local_list_entry_count ].name_length + 1 );
				for( size_t i = 0; i < entry[ local_list_entry_count ].name_length; i++ ) entry[ local_list_entry_count ].name[ i ] = vfs[ e ].name[ i ];

				// set icon
				switch( vfs[ e ].type ) {
					case STD_FILE_TYPE_link: {
						// special purpose?
						if( e == TRUE ) entry[ local_list_entry_count ].icon = kuro_icon[ KURO_ICON_TYPE_up ];
						
						// done
						break;
					}
					case STD_FILE_TYPE_directory: {
						// load directory icon, if not present
						if( ! kuro_icon[ KURO_ICON_TYPE_directory ] ) kuro_icon_register( KURO_ICON_TYPE_directory, (uint8_t *) "/system/var/gfx/icons/folder-green.tga" );

						// set directory icon
						entry[ local_list_entry_count ].icon = kuro_icon[ KURO_ICON_TYPE_directory ];
						
						// done
						break;
					}

					default: {
						// load default icon, if not present
						if( ! kuro_icon[ KURO_ICON_TYPE_default ] ) kuro_icon_register( KURO_ICON_TYPE_default, (uint8_t *) "/system/var/gfx/icons/text-plain.tga" );

						// set default icon
						entry[ local_list_entry_count ].icon = kuro_icon[ KURO_ICON_TYPE_default ];
						
						// properties of file
						struct LIB_ELF_STRUCTURE *elf = (struct LIB_ELF_STRUCTURE *) malloc( sizeof( struct LIB_ELF_STRUCTURE ) );

						// retrieve part of file content
						FILE *file = fopen( entry[ local_list_entry_count ].name ); fread( file, (uint8_t *) elf, sizeof( struct LIB_ELF_STRUCTURE ) ); fclose( file );

						// file type of ELF
						if( lib_elf_identify( (uintptr_t) elf ) ) {	// yes
							// library?
							if( elf -> type == LIB_ELF_TYPE_shared_object ) {
								// load library icon, if not present
								if( ! kuro_icon[ KURO_ICON_TYPE_library ] ) kuro_icon_register( KURO_ICON_TYPE_library, (uint8_t *) "/system/var/gfx/icons/application-x-sharedlib.tga" );

								// set library icon
								entry[ local_list_entry_count ].icon = kuro_icon[ KURO_ICON_TYPE_library ];
							}
							
							// module?
							else if( entry[ local_list_entry_count ].name_length > 3 && lib_string_compare( (uint8_t *) &entry[ local_list_entry_count ].name[ entry[ local_list_entry_count ].name_length - 3 ], (uint8_t *) ".ko", 3 ) ) {
								// load module icon, if not present
								if( ! kuro_icon[ KURO_ICON_TYPE_module ] ) kuro_icon_register( KURO_ICON_TYPE_module, (uint8_t *) "/system/var/gfx/icons/application-octet-stream.tga" );

								// set module icon
								entry[ local_list_entry_count ].icon = kuro_icon[ KURO_ICON_TYPE_module ];
							}
							
							// executable
							else {
								// load module icon, if not present
								if( ! kuro_icon[ KURO_ICON_TYPE_executable ] ) kuro_icon_register( KURO_ICON_TYPE_executable, (uint8_t *) "/system/var/gfx/icons/application-x-executable.tga" );

								// set executable icon
								entry[ local_list_entry_count ].icon = kuro_icon[ KURO_ICON_TYPE_executable ];
							}
						}

						// image extension?
						if( entry[ local_list_entry_count ].name_length > 4 && lib_string_compare( (uint8_t *) &entry[ local_list_entry_count ].name[ entry[ local_list_entry_count ].name_length - 4 ], (uint8_t *) ".tga", 4 ) ) {
							// load image icon, if not present
							if( ! kuro_icon[ KURO_ICON_TYPE_image ] ) kuro_icon_register( KURO_ICON_TYPE_image, (uint8_t *) "/system/var/gfx/icons/image.tga" );

							// set image icon
							entry[ local_list_entry_count ].icon = kuro_icon[ KURO_ICON_TYPE_image ];
						}

						// release file properties
						free( elf );
					}
				}

				// entry registered
				local_list_entry_count++;
			}
		}
	}

	// release directory content
	free( dir_content );

	// close directory
	fclose( dir );

	return local_list_entry_count;
}

uint8_t compare_names( struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST_ENTRY *first, struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST_ENTRY *second ) {
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
void dir_sort( struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST_ENTRY *entry, uint64_t local_list_entry_count ) {
	// prepare area for separated directories and files
	struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST_ENTRY *directories = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST_ENTRY *) malloc( (local_list_entry_count + 1) * sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST_ENTRY ) );
	struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST_ENTRY *files = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST_ENTRY *) malloc( (local_list_entry_count + 1) * sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST_ENTRY ) );

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
	loop = TRUE; while( loop ) {
		// until raedy
		loop = FALSE;

		// compare all entries
		for( uint64_t i = 0; i < directory - 1; i++ ) {
			// if first entry name is higher alphabetically than second
			if( compare_names( (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST_ENTRY *) &directories[ i ], (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST_ENTRY *) &directories[ i + 1 ] ) ) {
				// replace them
				struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST_ENTRY tmp = directories[ i ]; directories[ i ] = directories[ i + 1 ]; directories[ i + 1 ] = tmp;

				// and check all entries again
				loop = FALSE;
			}
		}
	}

	// sort other files
	loop = TRUE; while( loop ) {
		// until raedy
		loop = FALSE;

		// compare all entries
		for( uint64_t i = 1; i < file - 1; i++ ) {
			// if first entry name is higher alphabetically than second
			if( compare_names( (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST_ENTRY *) &files[ i ], (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST_ENTRY *) &files[ i + 1 ] ) ) {
				// replace them
				struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST_ENTRY tmp = files[ i ]; files[ i ] = files[ i + 1 ]; files[ i + 1 ] = tmp;

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
	kuro_icon[ type ] = lib_image_scale( lib_interface_icon( path, lib_string_length( path ) ), 48, 48, 16, 16 );
}

void release( void ) {
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
	kuro_icon_register( KURO_ICON_TYPE_up, (uint8_t *) "/system/var/gfx/icons/go-up.tga" );

	//----------------------------------------------------------------------

	// find element of ID: 1
	kuro_files = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST *) lib_interface_element_by_id( kuro_interface, 1 );

	// alloc initial area for list entries
	kuro_files -> entry = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST_ENTRY *) malloc( TRUE );

	// create list of entries
	kuro_files -> limit = kuro_reload( kuro_files -> entry );

	// show content from beginning
	kuro_files -> offset = EMPTY;

	// set order of files, by name and directories first
	// dir_sort( kuro_files -> entry, kuro_files -> limit );

	// update content of list
	lib_interface_element_list( kuro_interface, kuro_files );

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

		// check if there is any action required with entry
		for( uint64_t i = 0; i < kuro_files -> limit; i++ )
			if( kuro_files -> entry[ i ].flags & LIB_INTERFACE_ELEMENT_LIST_FLAG_run ) {
				// directory of directory symlink?
				if( kuro_files -> entry[ i ].type == STD_FILE_TYPE_directory || kuro_files -> entry[ i ].name_length == 2 && lib_string_compare( kuro_files -> entry[ i ].name, (uint8_t *) "..", 2 ) ) {
					// change home directory
					std_cd( kuro_files -> entry[ i ].name, kuro_files -> entry[ i ].name_length );

					// release current entry list
					release();

					// alloc initial area for list entries
					kuro_files -> entry = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST_ENTRY *) malloc( TRUE );

					// create list of entries
					kuro_files -> limit = kuro_reload( kuro_files -> entry );

					// show content from beginning
					kuro_files -> offset = EMPTY;

					// set order of files, by name and directories first
					// dir_sort( kuro_files -> entry, kuro_files -> limit );

					// update content of list
					lib_interface_element_list( kuro_interface, kuro_files );

					// update window content on screen
					kuro_interface -> descriptor -> flags |= STD_WINDOW_FLAG_flush;

					// done
					break;
				}
			}

		// exit?
		if( key == STD_ASCII_ESC ) break;	// yes
	}

	// exit
	return 0;
}
