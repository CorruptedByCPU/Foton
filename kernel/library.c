/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_library( struct LIB_ELF_STRUCTURE *elf ) {
	// ELF section properties
	struct LIB_ELF_STRUCTURE_SECTION *elf_s = (struct LIB_ELF_STRUCTURE_SECTION *) ((uintptr_t) elf + elf -> sections_offset);

	// we need to find 2 sections
	struct LIB_ELF_STRUCTURE_DYNAMIC_ENTRY *s_dynamic = EMPTY;
	uint8_t *s_strtab = EMPTY;

	// retrieve information about dynamic and string tables
	for( uint16_t i = 0; i < elf -> s_entry_count; i++ ) {
		// first string table contains library names
		if( ! s_strtab ) if( elf_s[ i ].type == LIB_ELF_SECTION_TYPE_strtab ) s_strtab = (uint8_t *) ((uint64_t) elf + elf_s[ i ].file_offset);

		// dynamic section?
		if( elf_s[ i ].type == LIB_ELF_SECTION_TYPE_dynamic ) s_dynamic = (struct LIB_ELF_STRUCTURE_DYNAMIC_ENTRY *) ((uintptr_t) elf + elf_s[ i ].file_offset);
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

uintptr_t kernel_library_function( uint8_t *string, uint64_t length ) {
// debug
// lib_terminal_printf( &kernel_terminal, (uint8_t *) "  {looking for '%s'}\n", string );

	// search in every loaded library
	for( uint64_t i = 0; i < KERNEL_LIBRARY_limit; i++ ) {
		// entry active?
		if( ! (kernel -> library_base_address[ i ].flags & KERNEL_LIBRARY_FLAG_active) ) continue;	// no

		// search thru available dynamic symbols inside library
		for( uint64_t j = 0; j < kernel -> library_base_address[ i ].d_entry_count; j++ )
			// function we are looking for?
			if( lib_string_length( (uint8_t *) &kernel -> library_base_address[ i ].strtab[ kernel -> library_base_address[ i ].dynamic_linking[ j ].name_offset ] ) == length && lib_string_compare( string, (uint8_t *) &kernel -> library_base_address[ i ].strtab[ kernel -> library_base_address[ i ].dynamic_linking[ j ].name_offset ], length ) )
				// yes
				return (uintptr_t) (kernel -> library_base_address[ i ].pointer + kernel -> library_base_address[ i ].dynamic_linking[ j ].address);
	}

	// not found
	return EMPTY;
}

void kernel_library_link( struct LIB_ELF_STRUCTURE *elf, uintptr_t code_base_address, uint8_t library ) {
	// we need to know where is

	// dynamic relocations
	struct LIB_ELF_STRUCTURE_DYNAMIC_RELOCATION *rela = EMPTY;
	uint64_t rela_entry_count = EMPTY;

	// global offset table
	uint64_t *got = EMPTY;

	// function strtab
	uint8_t *strtab = EMPTY;

	// section strtab
	uint8_t *shstrtab = EMPTY;

	// and at last dynamic symbols
	struct LIB_ELF_STRUCTURE_DYNAMIC_SYMBOL *dynsym = EMPTY;

	// ELF section properties
	struct LIB_ELF_STRUCTURE_SECTION *elf_s = (struct LIB_ELF_STRUCTURE_SECTION *) ((uintptr_t) elf + elf -> sections_offset);

	// before we find all of them, first must be section strtab
	for( uint64_t i = 0; i < elf -> s_entry_count; i++ ) {
		// section names?
		if( elf_s[ i ].type == LIB_ELF_SECTION_TYPE_strtab ) if( ! strtab ) {
			// try this strtab section
			shstrtab = (uint8_t *) ((uintptr_t) elf + elf_s[ i ].file_offset);

			// it's the correct one?
			if( lib_string_compare( &shstrtab[ elf_s[ i ].name ], (uint8_t *) ".shstrtab", 9 ) ) break;	// yes

			// nope, look for another one
			shstrtab = EMPTY;
		}
	}

	// retrieve information about others
	for( uint64_t i = 0; i < elf -> s_entry_count; i++ ) {
		// function names?
		if( elf_s[ i ].type == LIB_ELF_SECTION_TYPE_strtab ) if( ! strtab ) {
			// for library
			if( library )
				// try this strtab section
				strtab = (uint8_t *) (code_base_address + elf_s[ i ].file_offset);
			else
				// try relocation of strtab section
				strtab = (uint8_t *) (code_base_address + elf_s[ i ].virtual_address - KERNEL_EXEC_base_address);

			// it's the correct one?
			if( lib_string_compare( &shstrtab[ elf_s[ i ].name ], (uint8_t *) ".dynstr", 7 ) ) continue;	// yes

			// nope, look for another one
			strtab = EMPTY;
		}

		// global offset table?
		if( elf_s[ i ].type == LIB_ELF_SECTION_TYPE_progbits ) if( ! got ) {
			// it's the correct one?
			if( ! lib_string_compare( &shstrtab[ elf_s[ i ].name ], (uint8_t *) ".got.plt", 8 ) ) continue;	// no

			// for library
			if( library )
				// retrieve addres
				got = (uint64_t *) (code_base_address + elf_s[ i ].file_offset);
			else
				// try relocation
				got = (uint64_t *) (code_base_address + elf_s[ i ].virtual_address - KERNEL_EXEC_base_address);

			// first 3 entries are reserved
			got += 0x03;
		}
		
		// dynamic relocations?
		if( elf_s[ i ].type == LIB_ELF_SECTION_TYPE_rela ) {
			// for library
			if( library )
				// get pointer to structure
				rela = (struct LIB_ELF_STRUCTURE_DYNAMIC_RELOCATION *) (code_base_address + elf_s[ i ].file_offset);
			else
				// get relocated pointer to structure
				rela = (struct LIB_ELF_STRUCTURE_DYNAMIC_RELOCATION *) (code_base_address + elf_s[ i ].virtual_address - KERNEL_EXEC_base_address);

			// and number of entries
			rela_entry_count = elf_s[ i ].size_byte / sizeof( struct LIB_ELF_STRUCTURE_DYNAMIC_RELOCATION );
		}
		
		// dynamic symbols?
		if( elf_s[ i ].type == LIB_ELF_SECTION_TYPE_dynsym ) {
			// for library
			if( library )
				// retrieve address
				dynsym = (struct LIB_ELF_STRUCTURE_DYNAMIC_SYMBOL *) (code_base_address + elf_s[ i ].file_offset);
			else
				// retrieve by relocation
				dynsym = (struct LIB_ELF_STRUCTURE_DYNAMIC_SYMBOL *) (code_base_address + elf_s[ i ].virtual_address - KERNEL_EXEC_base_address);
		}
	}

	// external libraries are not required?
	if( ! rela ) return;	// yes

// debug
// lib_terminal_printf( &kernel_terminal, (uint8_t *) " GOT at offset 0x%X\n", got );

	// for each entry in dynamic symbols
	for( uint64_t i = 0; i < rela_entry_count; i++ ) {
		// it's a local function?
		if( dynsym[ rela[ i ].index ].address ) {
			// update address of local function
			got[ i ] = dynsym[ rela[ i ].index ].address + code_base_address;

// debug
// lib_terminal_printf( &kernel_terminal, (uint8_t *) "  [changed address of local function '%s' to %X]\n", &strtab[ dynsym[ rela[ i ].index ].name_offset ], got[ i ] );
		} else {
			// retrieve library function address
			got[ i ] = kernel_library_function( (uint8_t *) &strtab[ dynsym[ rela[ i ].index ].name_offset ], lib_string_length( (uint8_t *) &strtab[ dynsym[ rela[ i ].index ].name_offset ] ) );

// debug
// lib_terminal_printf( &kernel_terminal, (uint8_t *) "  [acquired function address of '%s' as 0x%X]\n", &strtab[ dynsym[ rela[ i ].index ].name_offset ], got[ i ] );
		}
	}
}


void kernel_library_load( uint8_t *name, uint64_t length ) {
	// if library already registered
	if( kernel_library_find( name, length ) ) return;	// end of routine

	// register new library
	struct KERNEL_LIBRARY_STRUCTURE *library = kernel_library_register();

	// retrieve file properties
	uint8_t path[ 12 + LIB_VFS_name_limit + 1 ] = "/system/lib/";
	for( uint64_t i = 0; i < length; i++ ) path[ i + 12 ] = name[ i ];

	// retrieve information about file to execute
	struct STD_FILE_STRUCTURE file = kernel_storage_file( kernel -> storage_root_id, (uint8_t *) &path, 12 + length );

	// if file doesn't exist
	if( ! file.id ) return;	// end of routine

	// prepare space for workbench
	uintptr_t workbench = EMPTY; 
	if( ! (workbench = kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( file.size_byte ) >> STD_SHIFT_PAGE )) ) return;	// no enough memory

	// load file into workbench space
	kernel_storage_read( kernel -> storage_root_id, file.id, workbench );

	// file contains proper ELF header?
	if( ! lib_elf_identify( workbench ) ) return;	// no

	// ELF structure properties
	struct LIB_ELF_STRUCTURE *elf = (struct LIB_ELF_STRUCTURE *) workbench;

	// it's a shared object file?
	if( elf -> type != LIB_ELF_TYPE_shared_object ) return;	// no

	// load libraries required by file
	kernel_library( elf );

// debug
// lib_terminal_printf( &kernel_terminal, (uint8_t *) "Library: parsing '%s'\n", name );

	// ELF header properties
	struct LIB_ELF_STRUCTURE_HEADER *elf_h = (struct LIB_ELF_STRUCTURE_HEADER *) ((uint64_t) elf + elf -> headers_offset);

	// calculate installed library size in pages
	uint64_t library_space_page = EMPTY;
	for( uint64_t i = 0; i < elf -> h_entry_count; i++ ) {
		// ignore blank entry
 		if( ! elf_h[ i ].type || ! elf_h[ i ].memory_size ) continue;

		// farthest point of loadable segment
		if( elf_h[ i ].type == LIB_ELF_HEADER_TYPE_load )
			library_space_page = MACRO_PAGE_ALIGN_UP( elf_h[ i ].virtual_address + elf_h[ i ].segment_size ) >> STD_SHIFT_PAGE;
	}

// debug
// lib_terminal_printf( &kernel_terminal, (uint8_t *) " Space length: 0x%X Bytes\n", library_space_page << STD_SHIFT_PAGE );

	// acquire memory space inside library environment
	uintptr_t library_base_address = (kernel_memory_acquire( kernel -> library_map_address, library_space_page ) << STD_SHIFT_PAGE) + KERNEL_LIBRARY_base_address;

// debug
// lib_terminal_printf( &kernel_terminal, (uint8_t *) " Base address: 0x%X\n", library_base_address );

	// map aquired memory space for library
	kernel_page_alloc( kernel -> page_base_address, library_base_address, library_space_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_user | KERNEL_PAGE_FLAG_library );

	// preserve that information inside library entry
	library -> pointer = library_base_address;
	library -> size_page = library_space_page;

	// copy library segments in place
	for( uint64_t i = 0; i < elf -> h_entry_count; i++ ) {
		// ignore blank entry or not loadable
 		if( ! elf_h[ i ].type || ! elf_h[ i ].memory_size || elf_h[ i ].type != LIB_ELF_HEADER_TYPE_load ) continue;

		// properties of loadable segment
		uint8_t *source = (uint8_t *) workbench + elf_h[ i ].segment_offset;
		uint8_t *target = (uint8_t *) library_base_address + elf_h[ i ].virtual_address;

// debug
// lib_terminal_printf( &kernel_terminal, (uint8_t *) " Segment offset 0x%X moved to 0x%X\n", source - workbench, target );

		// copy segment content in place
		for( uint64_t j = 0; j < elf_h[ i ].segment_size; j++ ) target[ j ] = source[ j ];
	}

	// ELF section properties
	struct LIB_ELF_STRUCTURE_SECTION *elf_s = (struct LIB_ELF_STRUCTURE_SECTION *) ((uintptr_t) elf + elf -> sections_offset);

	// retrieve information about symbol and string tables
	for( uint16_t i = 0; i < elf -> s_entry_count; i++ ) {
		// first string table contains functions names which this library provide
		if( ! library -> strtab ) if( elf_s[ i ].type == LIB_ELF_SECTION_TYPE_strtab ) library -> strtab = (uint8_t *) (library_base_address + elf_s[ i ].virtual_address);

		// dynamic linking section?
		if( elf_s[ i ].type == LIB_ELF_SECTION_TYPE_dynsym ) { library -> dynamic_linking = (struct LIB_ELF_STRUCTURE_DYNAMIC_SYMBOL *) (library_base_address + elf_s[ i ].virtual_address); library -> d_entry_count = elf_s[ i ].size_byte / sizeof( struct LIB_ELF_STRUCTURE_DYNAMIC_SYMBOL ); }
	}

// debug
// lib_terminal_printf( &kernel_terminal, (uint8_t *) "  String table at 0x%X\n  Dynamic linking information at 0x%X\n", library -> strtab, (uint64_t) library -> dynamic_linking );

	// connect required functions new locations / from another library
	kernel_library_link( elf, library_base_address, TRUE );

	// set parsed library name
	library -> length = length;
	for( uint8_t i = 0; i < length; i++ ) library -> name[ i ] = name[ i ];

	// library parsed
	library -> flags |= KERNEL_LIBRARY_FLAG_active;

// debug
// lib_terminal_printf( &kernel_terminal, (uint8_t *) " +%s installed.\n", name );

	// release workbench space
	kernel_memory_release( workbench, MACRO_PAGE_ALIGN_UP( file.size_byte ) >> STD_SHIFT_PAGE );
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