/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kuro_list( void ) {
	// release old files list
	if( kuro_files -> entry ) kuro_list_release( kuro_files );

	// directory content properties
	struct STD_STRUCTURE_DIR *dir = EMPTY;

	// received directory content?
	if( ! (dir = (struct STD_STRUCTURE_DIR *) std_dir( (uint8_t *) ".", TRUE )) ) exit();	// no

	// amount of registered entries
	kuro_files -> limit = EMPTY;

	// alloc initial area for list entries
	kuro_files -> entry = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY *) malloc( TRUE );

	// foreach file
	uint64_t d = TRUE; while( dir[ d ].type ) {
		// expand file list by this entry
		kuro_files -> entry = realloc( kuro_files -> entry, (kuro_files -> limit + TRUE) * sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY ) );

		// add
		kuro_list_insert( (struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY *) &kuro_files -> entry[ kuro_files -> limit++ ], (struct STD_STRUCTURE_DIR *) &dir[ d++ ] );
	}

	// release directory content
	std_memory_release( (uintptr_t) dir, MACRO_PAGE_ALIGN_UP( sizeof( struct STD_STRUCTURE_DIR ) * ++d ) >> STD_SHIFT_PAGE );
}

void kuro_list_insert( struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY *entry, struct STD_STRUCTURE_DIR *file ) {
	// set type and size in Bytes
	entry -> type = file -> type;
	entry -> byte = file -> limit;

	// links doesn't have size
	if( entry -> type == STD_FILE_TYPE_link ) entry -> byte = STD_MAX_unsigned;

	// file name
	entry -> name = (uint8_t *) calloc( file -> name_limit + TRUE );
	for( uint64_t i = 0; i < file -> name_limit; i++ ) entry -> name[ entry -> name_length++ ] = file -> name[ i ];

	// select icon
	kuro_icon_set( entry );
}

void kuro_list_release( struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE *list ) {
	// check if there is any action required with entry
	for( uint64_t i = 0; i < list -> limit; i++ )
		// release entry name area
		free( list -> entry[ i ].name );

	// release entry list
	free( list -> entry );
}
