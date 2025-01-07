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

MACRO_IMPORT_FILE_AS_ARRAY( interface, "./software/test/interface.json" );

struct LIB_INTERFACE_STRUCTURE	*test_interface;

FILE *dir = EMPTY;
uint8_t *dir_content = EMPTY;
size_t list_entry_count = FALSE;

	#define	LIB_ICON_TYPE_up		0x01
	#define	LIB_ICON_TYPE_default		0x02
	#define	LIB_ICON_TYPE_directory		0x03
	#define	LIB_ICON_TYPE_link		0x04
	#define	LIB_ICON_TYPE_executable	0x05
	#define	LIB_ICON_TYPE_library		0x06
	#define	LIB_ICON_TYPE_module		0x07

	struct LIB_STRUCTURE_ICON {
		uint32_t	*pixel;
	};

uint32_t **icon = EMPTY;

void close( void ) {
	// end of program
	exit();
}

size_t reload( struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST_ENTRY *entry ) {
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
						if( e == TRUE ) entry[ local_list_entry_count ].icon = icon[ LIB_ICON_TYPE_up ];
						
						// done
						break;
					}
					case STD_FILE_TYPE_directory: { entry[ local_list_entry_count ].icon = icon[ LIB_ICON_TYPE_directory ]; break; }
					default: { entry[ local_list_entry_count ].icon = icon[ LIB_ICON_TYPE_default ]; }
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

void icons( void ) {
	// initialize icon list
	icon = (uint32_t **) malloc( TRUE );

	// insert up icon
	uint8_t path_up[] = "/system/var/gfx/icons/go-up.tga";
	icon = (uint32_t **) realloc( icon, LIB_ICON_TYPE_up * sizeof( uint32_t * ) );
	icon[ LIB_ICON_TYPE_up ] = lib_image_scale( lib_interface_icon( (uint8_t *) &path_up, sizeof( path_up ) - 1 ), 48, 48, 16, 16 );

	// insert default icon
	uint8_t path_default[] = "/system/var/gfx/icons/application-octet-stream.tga";
	icon = (uint32_t **) realloc( icon, LIB_ICON_TYPE_default * sizeof( uint32_t * ) );
	icon[ LIB_ICON_TYPE_default ] = lib_image_scale( lib_interface_icon( (uint8_t *) &path_default, sizeof( path_default ) - 1 ), 48, 48, 16, 16 );

	// insert directory icon
	uint8_t path_directory[] = "/system/var/gfx/icons/folder-green.tga";
	icon = (uint32_t **) realloc( icon, LIB_ICON_TYPE_directory * sizeof( uint32_t * ) );
	icon[ LIB_ICON_TYPE_directory ] = lib_image_scale( lib_interface_icon( (uint8_t *) &path_directory, sizeof( path_directory ) - 1 ), 48, 48, 16, 16 );
}

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// alloc area for interface properties
	test_interface = (struct LIB_INTERFACE_STRUCTURE *) malloc( sizeof( struct LIB_INTERFACE_STRUCTURE ) );

	// initialize interface library
	test_interface -> properties = (uint8_t *) &file_interface_start;
	if( ! lib_interface( test_interface ) ) { log( "Cannot create window.\n" ); exit(); }

	// set minimal window size as current
	test_interface -> min_width = test_interface -> width;
	test_interface -> min_height = test_interface -> height;

	// find element of ID: 0
	struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *control = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *) lib_interface_element_by_id( test_interface, 0 );
	control -> event = (void *) close;	// assign executable function to element

	// initialize icons library
	icons();

		// find element of ID: 1
		struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST *element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST *) lib_interface_element_by_id( test_interface, 1 );

		// alloc initial area for list entries
		element -> entry = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST_ENTRY *) malloc( TRUE );

		// create list of entries
		element -> limit = reload( element -> entry );

		// show content from beginning
		element -> offset = EMPTY;

		// set order of files, by name and directories first
		// dir_sort( element -> entry, element -> limit );

		// update content of list
		lib_interface_element_list( test_interface, element );

	// update window content on screen
	test_interface -> descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_resizable | STD_WINDOW_FLAG_flush;

	// main loop
	while( TRUE ) {
		// free up AP time
		sleep( TRUE );

		// check events from interface
		struct LIB_INTERFACE_STRUCTURE *new = EMPTY;
		if( (new = lib_interface_event( test_interface )) ) {
			// update interface pointer
			test_interface = new;

			// update window content on screen
			test_interface -> descriptor -> flags |= STD_WINDOW_FLAG_flush;
		}

		// check events from keyboard
		uint16_t key = lib_interface_event_keyboard( test_interface );

		// exit?
		if( key == STD_ASCII_ESC ) break;	// yes
	}

	// exit
	return 0;
}
