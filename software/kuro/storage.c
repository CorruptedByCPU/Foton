/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kuro_storage( void ) {
	// release old files list
	if( kuro_storages -> entry ) kuro_list_release( kuro_storages );

	// properties of available storages
	struct STD_STRUCTURE_STORAGE *storage = EMPTY;
	
	// received storage list?
	if( ! (storage = (struct STD_STRUCTURE_STORAGE *) std_storage()) ) exit();	// no

	// default
	kuro_storages -> limit = EMPTY;

	// alloc initial area for list entries
	kuro_storages -> entry = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY *) malloc( TRUE );

	// storage by storage
	uint64_t s = EMPTY; while( storage[ s ].type ) {
		// expand storage list by this entry
		kuro_storages -> entry = realloc( kuro_storages -> entry, (kuro_storages -> limit + TRUE) * sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY ) );

		// add
		kuro_storage_insert( (struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY *) &kuro_storages -> entry[ kuro_storages -> limit++ ], (struct STD_STRUCTURE_STORAGE *) &storage[ s++ ] );
	}

	// release obtained storages properties
	std_memory_release( (uintptr_t) storage, MACRO_PAGE_ALIGN_UP( sizeof( struct STD_STRUCTURE_STORAGE ) * ++s ) >> STD_SHIFT_PAGE );
}

void kuro_storage_icon( struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY *entry ) {
	// set icon
	switch( entry -> type ) {
		case STD_STORAGE_TYPE_memory: {
			// load memory icon, if not present
			if( ! kuro_icons[ KURO_MIMETYPE_memory ] ) kuro_icon_register( KURO_MIMETYPE_memory, (uint8_t *) "/var/share/media/icon/default/devices/media-memory.tga" );

			// set memory icon
			entry -> mimetype = KURO_MIMETYPE_memory;
			entry -> icon = kuro_icons[ KURO_MIMETYPE_memory ];
			
			// done
			break;
		}
		case STD_STORAGE_TYPE_disk: {
			// load disk icon, if not present
			if( ! kuro_icons[ KURO_MIMETYPE_disk ] ) kuro_icon_register( KURO_MIMETYPE_disk, (uint8_t *) "/var/share/media/icon/default/devices/drive-harddisk.tga" );

			// set disk icon
			entry -> mimetype = KURO_MIMETYPE_disk;
			entry -> icon = kuro_icons[ KURO_MIMETYPE_disk ];
			
			// done
			break;
		}
	}
}

void kuro_storage_insert( struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY *entry, struct STD_STRUCTURE_STORAGE *storage ) {
	// set id, type and don't show size...
	entry -> id	= storage -> id;
	entry -> type	= storage -> type;
	entry -> byte	= STD_MAX_unsigned;

	// its name
	entry -> name = (uint8_t *) calloc( storage -> name_limit + TRUE );
	for( uint8_t n = 0; n < storage -> name_limit; n++ ) entry -> name[ entry -> name_length++ ] = storage -> name[ n ];

	// select icon
	kuro_storage_icon( entry );
}
