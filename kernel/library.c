/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

int64_t kernel_library( struct LIB_ELF_STRUCTURE *elf ) {
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
	if( ! s_dynamic ) return EMPTY;	// end of routine

	// load required libraries
	while( s_dynamic -> type == LIB_ELF_SECTION_DYNAMIC_TYPE_needed ) {
		// try to load library
		int64_t result = kernel_library_load( (uint8_t *) &s_strtab[ s_dynamic -> name_offset ], lib_string_length( (uint8_t *) &s_strtab[ s_dynamic -> name_offset ] ) );

		// if error occured, return last last result answer
		if( result ) return result;
		
		// next required library
		s_dynamic++;
	}

	// libraries parsed
	return EMPTY;
}

static void kernel_library_cancel( struct KERNEL_LIBRARY_STRUCTURE_INIT *library ) {
	// undo performed operations depending on cavity
	switch( library -> level ) {
		case 6: {
			// cannot foresee an error at this level and above
		}
		case 5: {
			// detach library area from global paging array
			kernel_page_detach( (uintptr_t *) kernel -> page_base_address, library -> base_address, library -> page );
		}
		case 4: {
			// release library area
			kernel_memory_dispose( kernel -> library_map_address, (library -> base_address - KERNEL_LIBRARY_base_address) >> STD_SHIFT_PAGE, library -> page );
		}
		case 3: {
			// release workbench area
			kernel_memory_release( library -> workbench_address, MACRO_PAGE_ALIGN_UP( library -> properties.byte ) >> STD_SHIFT_PAGE );
		}
		case 2: {
			// close file
			kernel_vfs_file_close( library -> socket );
		}
		case 1: {
			// release library entry
			library -> entry -> flags = EMPTY;
		}
	}
}

uint8_t kernel_library_find( uint8_t *name, uint8_t length ) {
	// check every entry
	for( uint64_t i = 0; i < KERNEL_LIBRARY_limit; i++ )
		// library with exact name and length?
		if( lib_string_compare( name, (uint8_t *) &kernel -> library_base_address[ i ].name, length ) && kernel -> library_base_address[ i ].name_length == length )
			// yes
			return TRUE;
	
	// nope
	return FALSE;
}

uintptr_t kernel_library_function( uint8_t *string, uint64_t length ) {
	// search in every loaded library
	for( uint64_t i = 0; i < KERNEL_LIBRARY_limit; i++ ) {
		// entry active?
		if( ! (kernel -> library_base_address[ i ].flags & KERNEL_LIBRARY_FLAG_active) ) continue;	// no

		// search thru available dynamic symbols inside library
		for( uint64_t j = 0; j < kernel -> library_base_address[ i ].d_entry_count; j++ )
			// local function we are looking for?
			if( kernel -> library_base_address[ i ].dynamic_linking[ j ].address && lib_string_length( (uint8_t *) &kernel -> library_base_address[ i ].strtab[ kernel -> library_base_address[ i ].dynamic_linking[ j ].name_offset ] ) == length && lib_string_compare( string, (uint8_t *) &kernel -> library_base_address[ i ].strtab[ kernel -> library_base_address[ i ].dynamic_linking[ j ].name_offset ], length ) )
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

	// global offset tables
	uint64_t *got		= EMPTY;
	uint64_t *got_plt	= EMPTY;

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
		if( ! strtab && elf_s[ i ].type == LIB_ELF_SECTION_TYPE_strtab ) {
			// it's the correct one?
			if( ! lib_string_compare( &shstrtab[ elf_s[ i ].name ], (uint8_t *) ".dynstr", 7 ) ) continue;	// no

			// for library
			if( library )
				// try this strtab section
				strtab = (uint8_t *) (code_base_address + elf_s[ i ].file_offset);
			else
				// try relocation of strtab section
				strtab = (uint8_t *) (code_base_address + elf_s[ i ].virtual_address - KERNEL_EXEC_base_address);
		}

		// // first global offset table?
		// if( ! got && elf_s[ i ].type == LIB_ELF_SECTION_TYPE_progbits ) {
		// 	// it's the correct one?
		// 	if( ! lib_string_compare( &shstrtab[ elf_s[ i ].name ], (uint8_t *) ".got", 4 ) ) continue;	// no

		// 	// for library
		// 	if( library )
		// 		// retrieve addres
		// 		got = (uint64_t *) (code_base_address + elf_s[ i ].file_offset);
		// 	else
		// 		// try relocation
		// 		got = (uint64_t *) (code_base_address + elf_s[ i ].virtual_address - KERNEL_EXEC_base_address);	
		// }

		// // second global offset table?
		// if( ! got_plt && elf_s[ i ].type == LIB_ELF_SECTION_TYPE_progbits ) {
		// 	// it's the correct one?
		// 	if( ! lib_string_compare( &shstrtab[ elf_s[ i ].name ], (uint8_t *) ".got.plt", 8 ) ) continue;	// no

		// 	// for library
		// 	if( library )
		// 		// retrieve addres
		// 		got_plt = (uint64_t *) (code_base_address + elf_s[ i ].file_offset);
		// 	else
		// 		// try relocation
		// 		got_plt = (uint64_t *) (code_base_address + elf_s[ i ].virtual_address - KERNEL_EXEC_base_address);

		// 	// first 3 entries are reserved
		// 	got_plt += 0x03;
		// }
		
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

	// for each entry in dynamic symbols
	for( uint64_t i = 0; i < rela_entry_count; i++ ) {
		// prepare .got.plt entry pointer
		if( library )
			// in case of library
			got_plt = (uint64_t *) (rela[ i ].offset + code_base_address);
		else
			// or software
			got_plt = (uint64_t *) (rela[ i ].offset + code_base_address - KERNEL_EXEC_base_address);

		// it's a local function?
		if( dynsym[ rela[ i ].index ].address )
			// update address of local function
			*got_plt = dynsym[ rela[ i ].index ].address + code_base_address;
		else
			// retrieve library function address
			*got_plt = kernel_library_function( (uint8_t *) &strtab[ dynsym[ rela[ i ].index ].name_offset ], lib_string_length( (uint8_t *) &strtab[ dynsym[ rela[ i ].index ].name_offset ] ) );
	}
}


int64_t kernel_library_load( uint8_t *name, uint64_t length ) {
	// prepare temporary library area
	struct KERNEL_LIBRARY_STRUCTURE_INIT library = { EMPTY };

	// if library already registered
	if( kernel_library_find( name, length ) ) return EMPTY;	// end of routine

	// register new library
	if( ! (library.entry = kernel_library_register()) ) return STD_ERROR_limit;	// place for new library

	// checkpoint reached: assigned library entry
	library.level++;

	// default location of libraries
	uint64_t path_length = 0;
	uint8_t path_default[ 12 ] = "/system/lib/";

	// set file path name
	uint8_t path[ 12 + LIB_VFS_NAME_limit ];
	for( uint64_t i = 0; i < 12; i++ ) path[ path_length++ ] = path_default[ i ];
	for( uint64_t i = 0; i < length; i++ ) path[ path_length++ ] = name[ i ];

	// retrieve information about library file
	library.socket = (struct KERNEL_VFS_STRUCTURE *) kernel_vfs_file_open( path, path_length );

	// if library does not exist
	if( ! library.socket ) { kernel_library_cancel( (struct KERNEL_LIBRARY_STRUCTURE_INIT *) &library ); return STD_ERROR_file_not_found; };

	// checkpoint reached: file socket opened
	library.level++;

	// gather information about file
	kernel_vfs_file_properties( library.socket, (struct KERNEL_VFS_STRUCTURE_PROPERTIES *) &library.properties );

	// assign area for workbench
	if( ! (library.workbench_address = kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( library.properties.byte ) >> STD_SHIFT_PAGE )) ) { kernel_library_cancel( (struct KERNEL_LIBRARY_STRUCTURE_INIT *) &library ); return STD_ERROR_memory_low; };

	// checkpoint reached: assigned area for temporary file
	library.level++;

	// load library into workbench space
	kernel_vfs_file_read( library.socket, (uint8_t *) library.workbench_address, EMPTY, library.properties.byte );

	//----------------------------------------------------------------------

	// file contains proper ELF header?
	if( ! lib_elf_identify( library.workbench_address ) ) { kernel_library_cancel( (struct KERNEL_LIBRARY_STRUCTURE_INIT *) &library ); return STD_ERROR_file_unknown; }	// no

	// ELF structure properties
	struct LIB_ELF_STRUCTURE *elf = (struct LIB_ELF_STRUCTURE *) library.workbench_address;

	// it's an executable file?
	if( elf -> type != LIB_ELF_TYPE_shared_object ) { kernel_library_cancel( (struct KERNEL_LIBRARY_STRUCTURE_INIT *) &library ); return STD_ERROR_file_not_executable; }	// no

	// load libraries required by file
	int64_t result = kernel_library( elf );
	if( result ) {
		// cancel load
		kernel_library_cancel( (struct KERNEL_LIBRARY_STRUCTURE_INIT *) &library );

		// return last error
		return result;
	}

	//----------------------------------------------------------------------

	// ELF header properties
	struct LIB_ELF_STRUCTURE_HEADER *elf_h = (struct LIB_ELF_STRUCTURE_HEADER *) ((uint64_t) elf + elf -> headers_offset);

	// calculate installed library size in pages
	for( uint64_t i = 0; i < elf -> h_entry_count; i++ ) {
		// ignore blank entry or not loadable
 		if( elf_h[ i ].type != LIB_ELF_HEADER_TYPE_load || ! elf_h[ i ].memory_size ) continue;

		// update executable space size?
		if( library.page < MACRO_PAGE_ALIGN_UP( elf_h[ i ].virtual_address + elf_h[ i ].memory_size ) >> STD_SHIFT_PAGE ) library.page = MACRO_PAGE_ALIGN_UP( elf_h[ i ].virtual_address + elf_h[ i ].memory_size ) >> STD_SHIFT_PAGE;
	}

	// acquire memory space inside library environment
	if( ! (library.base_address = (kernel_memory_acquire( kernel -> library_map_address, library.page ) << STD_SHIFT_PAGE) + KERNEL_LIBRARY_base_address) ) { kernel_library_cancel( (struct KERNEL_LIBRARY_STRUCTURE_INIT *) &library ); return STD_ERROR_memory_low; }

	// checkpoint reached: assigned area for library
	library.level++;

	// map aquired memory space for library
	if( ! kernel_page_alloc( kernel -> page_base_address, library.base_address, library.page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_user | KERNEL_PAGE_FLAG_external ) ) { kernel_library_cancel( (struct KERNEL_LIBRARY_STRUCTURE_INIT *) &library ); return STD_ERROR_memory_low; }

	// checkpoint reached: library area registered in global paging array
	library.level++;

	// preserve that information inside library entry
	library.entry -> pointer = library.base_address;
	library.entry -> size_page = library.page;

	// debug
	kernel -> log( (uint8_t *) "Library: %s at 0x%X\n", name, library.base_address );

	// ELF section properties
	struct LIB_ELF_STRUCTURE_SECTION *elf_s = (struct LIB_ELF_STRUCTURE_SECTION *) ((uint64_t) elf + elf -> sections_offset);

	// copy library segments in place
	for( uint64_t i = 0; i < elf -> s_entry_count; i++ ) {
		// ignore blank entries
		if( ! elf_s[ i ].virtual_address || ! elf_s[ i ].size_byte ) continue;

		// ignore not loadable entry
 		if( elf_s[ i ].type != LIB_ELF_SECTION_TYPE_progbits && elf_s[ i ].type != LIB_ELF_SECTION_TYPE_strtab && elf_s[ i ].type != LIB_ELF_SECTION_TYPE_dynsym && elf_s[ i ].type != LIB_ELF_SECTION_TYPE_rela && elf_s[ i ].type != LIB_ELF_SECTION_TYPE_rel ) continue;

		// properties of loadable segment
		uint8_t *source = (uint8_t *) library.workbench_address + elf_s[ i ].file_offset;
		uint8_t *target = (uint8_t *) library.base_address + elf_s[ i ].virtual_address;

		// copy segment content in place
		for( uint64_t j = 0; j < elf_s[ i ].size_byte; j++ ) target[ j ] = source[ j ];
	}

	// retrieve information about symbol and string tables
	for( uint16_t i = 0; i < elf -> s_entry_count; i++ ) {
		// first string table contains functions names which this library provide
		if( ! library.entry -> strtab ) if( elf_s[ i ].type == LIB_ELF_SECTION_TYPE_strtab ) library.entry -> strtab = (uint8_t *) (library.base_address + elf_s[ i ].virtual_address);

		// dynamic linking section?
		if( elf_s[ i ].type == LIB_ELF_SECTION_TYPE_dynsym ) { library.entry -> dynamic_linking = (struct LIB_ELF_STRUCTURE_DYNAMIC_SYMBOL *) (library.base_address + elf_s[ i ].virtual_address); library.entry -> d_entry_count = elf_s[ i ].size_byte / sizeof( struct LIB_ELF_STRUCTURE_DYNAMIC_SYMBOL ); }
	}

	// connect required functions new locations / from another library
	kernel_library_link( elf, library.base_address, TRUE );

	// set parsed library name
	library.entry -> name_length = length;
	for( uint8_t i = 0; i < length; i++ ) library.entry -> name[ i ] = name[ i ];

	// library parsed
	library.entry -> flags |= KERNEL_LIBRARY_FLAG_active;

	// release workbench space
	kernel_memory_release( library.workbench_address, MACRO_PAGE_ALIGN_UP( library.properties.byte ) >> STD_SHIFT_PAGE );

	// close file
	kernel_vfs_file_close( library.socket );

	// library loaded
	return EMPTY;
}

struct KERNEL_LIBRARY_STRUCTURE *kernel_library_register( void ) {
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