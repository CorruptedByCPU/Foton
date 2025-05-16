/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint64_t kuro_list( void ) {
	// directory content properties
	struct STD_STRUCTURE_DIR *dir = EMPTY;

	// received directory content?
	if( ! (dir = (struct STD_STRUCTURE_DIR *) std_dir( (uint8_t *) ".", TRUE )) ) exit();	// no

	// amount of registered entries
	uint64_t limit = EMPTY;

	// remember directory content pointer
	uintptr_t dir_ptr = (uintptr_t) dir;

	// foreach file
	while( (++dir) -> type ) {
		// expand file list by this entry
		kuro_files -> entry = realloc( kuro_files -> entry, (limit + TRUE) * sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY ) );

		// add
		kuro_list_insert( (struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY *) &kuro_files -> entry[ limit++ ], dir );
	}

	// file list prepared
	return limit;
}

void kuro_list_insert( struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY *entry, struct STD_STRUCTURE_DIR *file ) {
	// set type and size in Bytes
	entry -> type = file -> type;
	entry -> byte = file -> limit;

	// file name
	entry -> name = (uint8_t *) calloc( file -> name_limit + TRUE );
	for( uint64_t i = 0; i < file -> name_limit; i++ ) entry -> name[ entry -> name_length++ ] = file -> name[ i ];

	// select icon
	kuro_icon_set( entry, file );
}
