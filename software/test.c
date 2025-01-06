/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/interface.h"
	#include	"../library/string.h"
	#include	"../library/vfs.h"
	//----------------------------------------------------------------------

FILE *dir = EMPTY;
uint8_t *dir_content = EMPTY;
size_t list_entry_count = FALSE;

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

				// new create list entry
				entry[ local_list_entry_count ].flags = EMPTY;
				entry[ local_list_entry_count ].type = vfs[ e ].type;
				entry[ local_list_entry_count ].name_length = vfs[ e ].name_length;

				// copy entry name
				entry[ local_list_entry_count ].name = calloc( entry[ local_list_entry_count ].name_length + 1 );
				for( size_t i = 0; i < entry[ local_list_entry_count ].name_length; i++ ) entry[ local_list_entry_count ].name[ i ] = vfs[ e ].name[ i ];

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

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// parse directory content
	struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST_ENTRY *entry = malloc( TRUE );

	// create list of entries
	list_entry_count = reload( entry );

	// set order of files, by name and directories first
	dir_sort( entry, list_entry_count );

	// debug
	for( size_t i = 0; i < list_entry_count; i++ ) printf( "%s\n", entry[ i ].name );

	// exit
	return 0;
}
