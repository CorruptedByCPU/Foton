/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_library_cancel( struct KERNEL_STRUCTURE_LIBRARY_INIT *lib ) {
	// undo performed operations depending on cavity
	switch( lib -> level ) {
		case 6: {
			// cannot foresee an error at this level and above
		}
		
		case 5: {
			// disconnect library area from global paging array
			kernel_page_disconnect( (uint64_t *) kernel -> page_base_address, lib -> base_address, lib -> page );
		}

		case 4: {
			// release library area
			kernel_memory_dispose( kernel -> library_memory_address, (lib -> base_address - KERNEL_LIBRARY_base_address) >> STD_SHIFT_PAGE, lib -> page );
		}

		case 3: {
			// release temporary area
			kernel_memory_release( lib -> workbench, MACRO_PAGE_ALIGN_UP( lib -> socket -> file.limit ) >> STD_SHIFT_PAGE );
		}

		case 2: {
			// close file
			kernel_vfs_socket_delete( lib -> socket );
		}

		case 1: {
			// release library entry
			lib -> entry -> flags = EMPTY;
		}
	}
}

uint8_t kernel_library_find( uint8_t *name, uint8_t limit ) {
	// look for selected library
	for( uint64_t i = INIT; i < kernel -> library_limit; i++ )
		// library with exact name and length?
		if( limit == kernel -> library_base_address[ i ].name_limit && lib_string_compare( name, (uint8_t *) &kernel -> library_base_address[ i ].name, limit ) )
			// yes
			return TRUE;
	
	// no
	return FALSE;
}

struct KERNEL_STRUCTURE_LIBRARY *kernel_library_register( void ) {
	// look for available entry
	for( uint64_t i = INIT; i < kernel -> library_limit; i++ ) {
		// available?
		if( kernel -> library_base_address[ i ].flags ) continue;	// no

		// mark entry as reserved
		kernel -> library_base_address[ i ].flags = KERNEL_LIBRARY_FLAG_reserved;
		
		// return pointer to entry
		return (struct KERNEL_STRUCTURE_LIBRARY *) &kernel -> library_base_address[ i ];
	}

	// nope
	return EMPTY;
}

uintptr_t kernel_library_function( uint8_t *name, uint64_t length ) {
	// search in every loaded library
	for( uint64_t i = INIT; i < kernel -> library_limit; i++ ) {
		// library active?
		if( ! (kernel -> library_base_address[ i ].flags & KERNEL_LIBRARY_FLAG_active) ) continue;	// no

		// search thru available dynamic symbols inside library
		for( uint64_t j = INIT; j < kernel -> library_base_address[ i ].elf_section_dynsym_count; j++ )
			// local function we are looking for?
			if( kernel -> library_base_address[ i ].elf_section_dynsym[ j ].address && lib_string_length( (uint8_t *) &kernel -> library_base_address[ i ].elf_section_strtab[ kernel -> library_base_address[ i ].elf_section_dynsym[ j ].name_offset ] ) == length && lib_string_compare( name, (uint8_t *) &kernel -> library_base_address[ i ].elf_section_strtab[ kernel -> library_base_address[ i ].elf_section_dynsym[ j ].name_offset ], length ) )
				// yes
				return (uintptr_t) (kernel -> library_base_address[ i ].base + kernel -> library_base_address[ i ].elf_section_dynsym[ j ].address);
	}

	// not found
	return EMPTY; 
}

void kernel_library_link( struct LIB_ELF_STRUCTURE *elf, uintptr_t base_address, uint8_t is_library ) {
	// we need to know where is:
	struct LIB_ELF_STRUCTURE_DYNAMIC_RELOCATION	*elf_section_rela		= EMPTY;
	uint64_t					 elf_section_rela_entry_count	= EMPTY;
	uint64_t					*elf_section_rela_got_plt	= EMPTY;
	uint8_t						*elf_section_strtab		= EMPTY;
	uint8_t						*elf_section_shstrtab		= EMPTY;
	struct LIB_ELF_STRUCTURE_DYNAMIC_SYMBOL		*elf_section_dynsym		= EMPTY;

	// ELF section properties
	struct LIB_ELF_STRUCTURE_SECTION *elf_section = (struct LIB_ELF_STRUCTURE_SECTION *) ((uintptr_t) elf + elf -> section_offset);

	// before we find all of them, first must be section strtab
	for( uint64_t i = INIT; i < elf -> section_count; i++ ) {
		// section names?
		if( elf_section[ i ].type == LIB_ELF_SECTION_TYPE_strtab ) if( ! elf_section_strtab ) {
			// try this strtab section
			elf_section_shstrtab = (uint8_t *) ((uintptr_t) elf + elf_section[ i ].file_offset);

			// it's the correct one?
			if( lib_string_compare( &elf_section_shstrtab[ elf_section[ i ].name ], (uint8_t *) ".shstrtab", 9 ) ) break;	// yes

			// nope, look for another one
			elf_section_shstrtab = EMPTY;
		}
	}

	// retrieve information about others
	for( uint64_t i = INIT; i < elf -> section_count; i++ ) {
		// function names?
		if( ! elf_section_strtab && elf_section[ i ].type == LIB_ELF_SECTION_TYPE_strtab ) {
			// it's the correct one?
			if( ! lib_string_compare( &elf_section_shstrtab[ elf_section[ i ].name ], (uint8_t *) ".dynstr", 7 ) ) continue;	// no

			// for library
			if( is_library )
				// try this strtab section
				elf_section_strtab = (uint8_t *) (base_address + elf_section[ i ].file_offset);
			else
				// try relocation of strtab section
				elf_section_strtab = (uint8_t *) (base_address + elf_section[ i ].virtual_address - KERNEL_EXEC_base_address);
		}
		
		// dynamic relocations?
		if( elf_section[ i ].type == LIB_ELF_SECTION_TYPE_rela ) {
			// for library
			if( is_library )
				// get pointer to structure
				elf_section_rela = (struct LIB_ELF_STRUCTURE_DYNAMIC_RELOCATION *) (base_address + elf_section[ i ].file_offset);
			else
				// get relocated pointer to structure
				elf_section_rela = (struct LIB_ELF_STRUCTURE_DYNAMIC_RELOCATION *) (base_address + elf_section[ i ].virtual_address - KERNEL_EXEC_base_address);

			// and number of entries
			elf_section_rela_entry_count = elf_section[ i ].size_byte / sizeof( struct LIB_ELF_STRUCTURE_DYNAMIC_RELOCATION );
		}
		
		// dynamic symbols?
		if( elf_section[ i ].type == LIB_ELF_SECTION_TYPE_dynsym ) {
			// for library
			if( is_library )
				// retrieve address
				elf_section_dynsym = (struct LIB_ELF_STRUCTURE_DYNAMIC_SYMBOL *) (base_address + elf_section[ i ].file_offset);
			else
				// retrieve by relocation
				elf_section_dynsym = (struct LIB_ELF_STRUCTURE_DYNAMIC_SYMBOL *) (base_address + elf_section[ i ].virtual_address - KERNEL_EXEC_base_address);
		}
	}

	// external libraries are not required?
	if( ! elf_section_rela ) return;	// yes

	// for each entry in dynamic symbols
	for( uint64_t i = INIT; i < elf_section_rela_entry_count; i++ ) {
		// prepare .got.plt entry pointer
		if( is_library )
			// in case of library
			elf_section_rela_got_plt = (uint64_t *) (elf_section_rela[ i ].offset + base_address);
		else
			// or software
			elf_section_rela_got_plt = (uint64_t *) (elf_section_rela[ i ].offset + base_address - KERNEL_EXEC_base_address);

		// it's a local function?
		if( elf_section_dynsym[ elf_section_rela[ i ].index ].address )
			// update address of local function
			*elf_section_rela_got_plt = elf_section_dynsym[ elf_section_rela[ i ].index ].address + base_address;
		else
			// retrieve library function address
			*elf_section_rela_got_plt = kernel_library_function( (uint8_t *) &elf_section_strtab[ elf_section_dynsym[ elf_section_rela[ i ].index ].name_offset ], lib_string_length( (uint8_t *) &elf_section_strtab[ elf_section_dynsym[ elf_section_rela[ i ].index ].name_offset ] ) );
	}
}

uint8_t kernel_library_load( uint8_t *name, uint64_t limit ) {
	// library state
	struct KERNEL_STRUCTURE_LIBRARY_INIT lib = { INIT };

	// library already registered?
	if( kernel_library_find( name, limit ) ) return TRUE;	// yes

	// register new library
	if( ! (lib.entry = kernel_library_register()) ) return FALSE;	// something... wrong.

	// checkpoint: entry ---------------------------------------------------
	lib.level++;

	// default location of libraries
	uint8_t path_default[ 5 ] = "/lib/";

	// combine default path with library name
	lib.path = (uint8_t *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( path_default ) + limit ) >> STD_SHIFT_PAGE ); for( uint8_t i = INIT; i < sizeof( path_default ); i++ ) lib.path[ lib.limit++ ] = path_default[ i ]; for( uint64_t i = INIT; i < lib_string_word_end( name, limit, STD_ASCII_SPACE ); i++ ) lib.path[ lib.limit++ ] = name[ i ];

	// open file
	lib.socket = (struct KERNEL_STRUCTURE_VFS_SOCKET *) &kernel -> vfs_base_address[ kernel_syscall_file_open( lib.path, lib.limit ) ];

	// release path, no more needed
	kernel_memory_release( (uintptr_t) lib.path, MACRO_PAGE_ALIGN_UP( sizeof( path_default ) + limit ) >> STD_SHIFT_PAGE );

	// file doesn't exist?
	if( ! lib.socket ) return FALSE;	// yep

	// checkpoint: socket --------------------------------------------------
	lib.level++;

	// assign temporary area for parsing file content
	if( ! (lib.workbench = kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( lib.socket -> file.limit ) >> STD_SHIFT_PAGE )) ) { kernel_library_cancel( (struct KERNEL_STRUCTURE_LIBRARY_INIT *) &lib ); return FALSE; };

	// checkpoint: workbench -----------------------------------------------
	lib.level++;

	// load content of file
	kernel -> storage_base_address[ lib.socket -> storage ].vfs -> file_read( lib.socket, (uint8_t *) lib.workbench, EMPTY, lib.socket -> file.limit );

	//----------------------------------------------------------------------

	// file contains ELF header?
	if( ! lib_elf_identify( lib.workbench ) ) { kernel_library_cancel( (struct KERNEL_STRUCTURE_LIBRARY_INIT *) &lib ); return FALSE; };	// no

	// ELF file properties
	struct LIB_ELF_STRUCTURE *elf = (struct LIB_ELF_STRUCTURE *) lib.workbench;

	// executable?
	if( elf -> type != LIB_ELF_TYPE_shared_object ) { kernel_library_cancel( (struct KERNEL_STRUCTURE_LIBRARY_INIT *) &lib ); return FALSE; }	// no

	// load libraries required by executable
	if( ! kernel_library( elf ) ) { kernel_library_cancel( (struct KERNEL_STRUCTURE_LIBRARY_INIT *) &lib ); return FALSE; }	// something went wrong

	//----------------------------------------------------------------------

	// ELF header properties
	struct LIB_ELF_STRUCTURE_HEADER *elf_header = (struct LIB_ELF_STRUCTURE_HEADER *) ((uint64_t) elf + elf -> header_offset);

	// find furthest position in page of initialized library
	for( uint64_t i = INIT; i < elf -> header_count; i++ ) {
		// ignore blank entry or not loadable
 		if( elf_header[ i ].type != LIB_ELF_HEADER_TYPE_load || ! elf_header[ i ].segment_size  || ! elf_header[ i ].memory_size ) continue;

		// update library area limit?
		if( lib.page < MACRO_PAGE_ALIGN_UP( elf_header[ i ].virtual_address + elf_header[ i ].memory_size ) >> STD_SHIFT_PAGE ) lib.page = MACRO_PAGE_ALIGN_UP( elf_header[ i ].virtual_address + elf_header[ i ].memory_size ) >> STD_SHIFT_PAGE;
	}

	// acquire area for library inside library environment
	if( ! (lib.base_address = (kernel_memory_acquire( kernel -> library_memory_address, lib.page, KERNEL_MEMORY_LOW, kernel -> page_limit ) << STD_SHIFT_PAGE) + KERNEL_LIBRARY_base_address) ) { kernel_library_cancel( (struct KERNEL_STRUCTURE_LIBRARY_INIT *) &lib ); return FALSE; }

	// checkpoint: acquired ------------------------------------------------
	lib.level++;

	// map aquired memory area
	if( ! kernel_page_alloc( kernel -> page_base_address, lib.base_address, lib.page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_user | (KERNEL_PAGE_TYPE_LIBRARY << KERNEL_PAGE_TYPE_offset) ) ) { kernel_library_cancel( (struct KERNEL_STRUCTURE_LIBRARY_INIT *) &lib ); return FALSE; }

	// checkpoint: map -----------------------------------------------------
	lib.level++;

	// preserve that information inside library entry
	lib.entry -> base	= lib.base_address;
	lib.entry -> limit	= lib.page;

	// ELF section properties
	struct LIB_ELF_STRUCTURE_SECTION *elf_section = (struct LIB_ELF_STRUCTURE_SECTION *) ((uint64_t) elf + elf -> section_offset);

	// load library segments in place
	for( uint64_t i = INIT; i < elf -> section_count; i++ ) {
		// ignore blank entries
		if( ! elf_section[ i ].virtual_address || ! elf_section[ i ].size_byte ) continue;

		// ignore not loadable entries
 		if( elf_section[ i ].type != LIB_ELF_SECTION_TYPE_progbits && elf_section[ i ].type != LIB_ELF_SECTION_TYPE_strtab && elf_section[ i ].type != LIB_ELF_SECTION_TYPE_dynsym && elf_section[ i ].type != LIB_ELF_SECTION_TYPE_rela && elf_section[ i ].type != LIB_ELF_SECTION_TYPE_rel ) continue;

		// properties of loadable segment
		uint8_t *source = (uint8_t *) lib.workbench + elf_section[ i ].file_offset;
		uint8_t *target = (uint8_t *) lib.base_address + elf_section[ i ].virtual_address;

		// copy segment content in place
		for( uint64_t j = 0; j < elf_section[ i ].size_byte; j++ ) target[ j ] = source[ j ];
	}

	// retrieve pointers
	for( uint16_t i = INIT; i < elf -> section_count; i++ ) {
		// strtab and first occurence? (look only for first)
		if( ! lib.entry -> elf_section_strtab && elf_section[ i ].type == LIB_ELF_SECTION_TYPE_strtab ) lib.entry -> elf_section_strtab = (uint8_t *) (lib.entry -> base + elf_section[ i ].virtual_address);

		// dynsym?
		if( elf_section[ i ].type == LIB_ELF_SECTION_TYPE_dynsym ) { lib.entry -> elf_section_dynsym = (struct LIB_ELF_STRUCTURE_DYNAMIC_SYMBOL *) ((uintptr_t) lib.entry -> base + elf_section[ i ].virtual_address); lib.entry -> elf_section_dynsym_count = elf_section[ i ].size_byte / sizeof( struct LIB_ELF_STRUCTURE_DYNAMIC_SYMBOL ); }
	}

	//----------------------------------------------------------------------

	// link library with other (if required)
	kernel_library_link( elf, lib.base_address, TRUE );

	//----------------------------------------------------------------------

	// set parsed library name
	lib.entry -> name_limit = limit;
	for( uint8_t i = 0; i < limit; i++ ) lib.entry -> name[ i ] = name[ i ];

	// activate library
	lib.entry -> flags |= KERNEL_LIBRARY_FLAG_active;

	// release temporary area
	kernel_memory_release( lib.workbench, MACRO_PAGE_ALIGN_UP( lib.socket -> file.limit ) >> STD_SHIFT_PAGE );

	// close file
	kernel_vfs_socket_delete( lib.socket );

	// debug
	kernel_log( (uint8_t *) "%s at 0x%16X\n", name, lib.base_address );

	// library loaded
	return TRUE;
}

uint8_t kernel_library( struct LIB_ELF_STRUCTURE *elf ) {
	// ELF section properties
	struct LIB_ELF_STRUCTURE_SECTION *elf_section = (struct LIB_ELF_STRUCTURE_SECTION *) ((uintptr_t) elf + elf -> section_offset);

	// find location of follow sections
	struct LIB_ELF_STRUCTURE_DYNAMIC *elf_section_dynamic	= INIT;
	uint8_t *elf_section_strtab				= INIT;

	// retrieve pointers
	for( uint16_t i = INIT; i < elf -> section_count; i++ ) {
		// strtab and first occurence? (look only for first)
		if( ! elf_section_strtab && elf_section[ i ].type == LIB_ELF_SECTION_TYPE_strtab ) elf_section_strtab = (uint8_t *) ((uint64_t) elf + elf_section[ i ].file_offset);

		// dynamics?
		if( elf_section[ i ].type == LIB_ELF_SECTION_TYPE_dynamic ) elf_section_dynamic = (struct LIB_ELF_STRUCTURE_DYNAMIC *) ((uintptr_t) elf + elf_section[ i ].file_offset);
	}

	// dynamic section exist?
	if( ! elf_section_dynamic ) return FALSE;	// nope

	// load, needed libraries
	while( elf_section_dynamic -> type == LIB_ELF_SECTION_DYNAMIC_TYPE_needed ) {
		// something... wrong?
		if( ! kernel_library_load( (uint8_t *) &elf_section_strtab[ elf_section_dynamic -> name_offset ], lib_string_length( (uint8_t *) &elf_section_strtab[ elf_section_dynamic -> name_offset ] ) ) ) return FALSE;	// yep

		// check next library
		elf_section_dynamic++;
	}

	// libraries parsed
	return TRUE;
}
