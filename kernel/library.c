/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_library( struct LIB_ELF_STRUCTURE *elf ) {
	// ELF section properties
	struct LIB_ELF_STRUCTURE_SECTION *sections = (struct LIB_ELF_STRUCTURE_SECTION *) ((uintptr_t) elf + elf -> sections_offset);

	// we need to find 2 sections
	struct LIB_ELF_STRUCTURE_DYNAMIC_ENTRY *s_dynamic = EMPTY;
	uint8_t *s_strtab = EMPTY;

	// retrieve information about dynamic and string table
	for( uint16_t s = 0; s < elf -> s_entry_count; s++ ) {
		// first string table contains library names
		if( ! s_strtab ) if( sections[ s ].type == LIB_ELF_SECTION_TYPE_strtab ) s_strtab = (uint8_t *) ((uint64_t) elf + sections[ s ].file_offset);

		// dynamic section?
		if( sections[ s ].type == LIB_ELF_SECTION_TYPE_dynamic ) s_dynamic = (struct LIB_ELF_STRUCTURE_DYNAMIC_ENTRY *) ((uintptr_t) elf + sections[ s ].file_offset);
	}

	// if dynamic section doesn't exist
	if( ! s_dynamic ) return;	// end of routine

	// load required libraries
	while( s_dynamic -> type == LIB_ELF_SECTION_DYNAMIC_TYPE_needed ) { kernel_library_load( (uint8_t *) &s_strtab[ s_dynamic -> name_offset ], lib_string_length( (uint8_t *) &s_strtab[ s_dynamic -> name_offset ] ) ); s_dynamic++; }
}

uint8_t kernel_library_find( uint8_t *name, uint8_t length ) {
	// check every entry
	for( uint64_t i = 0; i < KERNEL_LIBRARY_limit; i++ )
		// library with exact name and length?
		if( lib_string_compare( name, (uint8_t *) &kernel -> library_base_address[ i ].name, length ) && kernel -> library_base_address[ i ].length == length )
			// yes
			return TRUE;
	
	// nope
	return FALSE;
}

void kernel_library_load( uint8_t *name, uint64_t length ) {
	// if library already registered
	if( kernel_library_find( name, length ) ) return;	// end of routine

	// register new library
	struct KERNEL_LIBRARY_STRUCTURE *library = kernel_library_register();

	// retrieve file properties
	uint8_t path[ LIB_VFS_name_limit + 12 ] = "/system/lib/";
	for( uint64_t i = 0; i < length; i++ ) path[ i + 12 ] = name[ i ];

	// retrieve information about file to execute
	struct STD_FILE_STRUCTURE file = kernel_storage_file( kernel -> storage_root_id, (uint8_t *) &path, length + 12 );
}

struct KERNEL_LIBRARY_STRUCTURE *kernel_library_register() {
	// search for empty entry
	for( uint64_t i = 0; i < KERNEL_LIBRARY_limit; i++ )
		// found?
		if( ! kernel -> library_base_address[ i ].flags ) {
			// mark entry as reserved
			kernel -> library_base_address[ i ].flags |= KERNEL_LIBRARY_FLAG_reserved;
		
			// return pointer to entry
			return (struct KERNEL_LIBRARY_STRUCTURE *) &kernel -> library_base_address[ i ];
		}
	
	// nope
	return EMPTY;
}