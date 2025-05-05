/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

static void kernel_exec_cancel( struct KERNEL_STRUCTURE_EXEC *exec ) {
	// undo performed operations depending on cavity
	switch( exec -> level ) {
		case 6: {
			// cannot foresee any error at this level and above
		}

		case 5: {
			// release memory map from task entry
			kernel_memory_release( (uintptr_t) exec -> task -> memory, MACRO_PAGE_ALIGN_UP( (kernel -> page_limit >> STD_SHIFT_8) + TRUE ) >> STD_SHIFT_PAGE );
		}

		case 4: {
			// release paging structure
			kernel_page_deconstruct( exec -> task -> cr3, exec -> task -> type );
		}

		case 3: {
			// release task entry
			exec -> task -> flags = EMPTY;
		}

		case 2: {
			// release temporary area
			kernel_memory_release( exec -> workbench, MACRO_PAGE_ALIGN_UP( exec -> socket -> file.limit ) >> STD_SHIFT_PAGE );
		}

		case 1: {
			// close file
			kernel_vfs_socket_delete( exec -> socket );
		}
	}
}

uint64_t kernel_exec( uint8_t *name, uint64_t limit, uint8_t stream, uint8_t init ) {
	// execution state
	struct KERNEL_STRUCTURE_EXEC exec = { INIT };

	// default location of executables
	uint8_t path_default[ 5 ] = "/bin/";

	// combine default path with executable name
	exec.path = (uint8_t *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( path_default ) + limit ) >> STD_SHIFT_PAGE ); for( uint8_t i = INIT; i < sizeof( path_default ); i++ ) exec.path[ exec.limit++ ] = path_default[ i ]; for( uint64_t i = INIT; i < lib_string_word_end( name, limit, STD_ASCII_SPACE ); i++ ) exec.path[ exec.limit++ ] = name[ i ];

	// open file
	exec.socket = (struct KERNEL_STRUCTURE_VFS_SOCKET *) &kernel -> vfs_base_address[ kernel_syscall_file_open( exec.path, exec.limit ) ];

	// release path, no more needed
	kernel_memory_release( (uintptr_t) exec.path, MACRO_PAGE_ALIGN_UP( sizeof( path_default ) + limit ) >> STD_SHIFT_PAGE );

	// file doesn't exist?
	if( ! exec.socket ) return EMPTY;	// yep

	// checkpoint: socket --------------------------------------------------
	exec.level++;

	// assign temporary area for parsing file content
	if( ! (exec.workbench = kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( exec.socket -> file.limit ) >> STD_SHIFT_PAGE )) ) { kernel_exec_cancel( (struct KERNEL_STRUCTURE_EXEC *) &exec ); return EMPTY; };

	// checkpoint: workbench -----------------------------------------------
	exec.level++;

	// load content of file
	kernel -> storage_base_address[ exec.socket -> storage ].vfs -> file_read( exec.socket, (uint8_t *) exec.workbench, EMPTY, exec.socket -> file.limit );

	// file contains ELF header?
	if( ! lib_elf_identify( exec.workbench ) ) { kernel_exec_cancel( (struct KERNEL_STRUCTURE_EXEC *) &exec ); return EMPTY; };	// no

	// ELF file properties
	struct LIB_ELF_STRUCTURE *elf = (struct LIB_ELF_STRUCTURE *) exec.workbench;

	// executable?
	if( elf -> type != LIB_ELF_TYPE_executable ) { kernel_exec_cancel( (struct KERNEL_STRUCTURE_EXEC *) &exec ); return EMPTY; }	// no

	// load libraries required by executable
	if( ! kernel_library( elf ) ) { kernel_exec_cancel( (struct KERNEL_STRUCTURE_EXEC *) &exec ); return EMPTY; }	// something went wrong

	// add new task entry
	if( ! (exec.task = kernel_task_add( name, limit )) ) { kernel_exec_cancel( (struct KERNEL_STRUCTURE_EXEC *) &exec ); return EMPTY; }	// end of resources

	// mark task type
	exec.task -> type = KERNEL_TASK_TYPE_PROCESS;

	// checkpoint: task ----------------------------------------------------
	exec.level++;

	// create paging table
	uint64_t cr3 = EMPTY;
	if( ! (cr3 = kernel_memory_alloc_page()) ) { kernel_exec_cancel( (struct KERNEL_STRUCTURE_EXEC *) &exec ); return EMPTY; }

	// set paging address
	exec.task -> cr3 = (uint64_t *) (cr3 | KERNEL_MEMORY_mirror);

	// checkpoint: paging --------------------------------------------------
	exec.level++;

	// describe area under context stack
	if( ! kernel_page_alloc( exec.task -> cr3, KERNEL_STACK_address, KERNEL_STACK_LIMIT_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | (exec.task -> type << KERNEL_PAGE_TYPE_offset) ) ) { kernel_exec_cancel( (struct KERNEL_STRUCTURE_EXEC *) &exec ); return EMPTY; }

	// set initial startup configuration for new process
	struct KERNEL_STRUCTURE_IDT_RETURN *context = (struct KERNEL_STRUCTURE_IDT_RETURN *) (kernel_page_address( exec.task -> cr3, KERNEL_STACK_pointer - STD_PAGE_byte ) + KERNEL_MEMORY_mirror + (STD_PAGE_byte - sizeof( struct KERNEL_STRUCTURE_IDT_RETURN )));

	// set the process entry address
	context -> rip		= elf -> entry_ptr;

	// code descriptor
	context -> cs		= offsetof( struct KERNEL_STRUCTURE_GDT, cs_ring3 ) | 0x03;

	// basic processor state flags
	context -> eflags	= KERNEL_TASK_EFLAGS_default;

	// the context stack top pointer
	exec.task -> rsp	= KERNEL_STACK_pointer - sizeof( struct KERNEL_STRUCTURE_IDT_RETURN );

	// stack descriptor
	context -> ss		= offsetof( struct KERNEL_STRUCTURE_GDT, ds_ring3 ) | 0x03;

	//----------------------------------------------------------------------

	// insert on stack, whole provided command line
	exec.stack_byte = (limit & ~STD_MASK_byte_half) + 0x18;

	// allocate area for stack
	if( ! (exec.stack = (uint8_t *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( exec.stack_byte ) >> STD_SHIFT_PAGE )) ) { kernel_exec_cancel( (struct KERNEL_STRUCTURE_EXEC *) &exec ); return EMPTY; }

	// new context stack pointer
	context -> rsp = KERNEL_TASK_STACK_pointer - exec.stack_byte;

	// share argc
	*((uint64_t *) &exec.stack[ MACRO_PAGE_ALIGN_UP( exec.stack_byte ) - exec.stack_byte ]) = limit;

	// and argv
	for( uint64_t i = INIT; i < limit; i++ ) exec.stack[ MACRO_PAGE_ALIGN_UP( exec.stack_byte ) - exec.stack_byte + 0x08 + i ] = name[ i ];

	// map as stack
	if( ! kernel_page_map( exec.task -> cr3, (uintptr_t) exec.stack & ~KERNEL_MEMORY_mirror, MACRO_PAGE_ALIGN_DOWN( context -> rsp ), MACRO_PAGE_ALIGN_UP( exec.stack_byte ) >> STD_SHIFT_PAGE, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | (exec.task -> type << KERNEL_PAGE_TYPE_offset) ) ) { kernel_memory_release( (uintptr_t) exec.stack, MACRO_PAGE_ALIGN_UP( exec.stack_byte ) >> STD_SHIFT_PAGE ); kernel_exec_cancel( (struct KERNEL_STRUCTURE_EXEC *) &exec ); return EMPTY; }

	// stack length
	exec.task -> stack_page += MACRO_PAGE_ALIGN_UP( exec.stack_byte ) >> STD_SHIFT_PAGE;

	//----------------------------------------------------------------------

	// ELF header properties
	struct LIB_ELF_STRUCTURE_HEADER *elf_header = (struct LIB_ELF_STRUCTURE_HEADER *) ((uint64_t) elf + elf -> header_offset);

	// find furthest position in page of initialized executable
	for( uint16_t i = INIT; i < elf -> header_count; i++ ) {
		// ignore blank entry or not loadable
		if( elf_header[ i ].type != LIB_ELF_HEADER_TYPE_load || ! elf_header[ i ].segment_size  || ! elf_header[ i ].memory_size ) continue;

		// update executable area limit?
		if( exec.page < (MACRO_PAGE_ALIGN_UP( elf_header[ i ].virtual_address + elf_header[ i ].memory_size ) - KERNEL_EXEC_base_address) >> STD_SHIFT_PAGE ) exec.page = (MACRO_PAGE_ALIGN_UP( elf_header[ i ].virtual_address + elf_header[ i ].memory_size ) - KERNEL_EXEC_base_address) >> STD_SHIFT_PAGE;
	}

	// acquire area for executable
	if( ! (exec.base = kernel_memory_alloc( exec.page )) ) { kernel_exec_cancel( (struct KERNEL_STRUCTURE_EXEC *) &exec ); return EMPTY; }

	// load executable segments in place
	for( uint16_t i = INIT; i < elf -> header_count; i++ ) {
		// ignore blank entry or not loadable
 		if( elf_header[ i ].type != LIB_ELF_HEADER_TYPE_load || ! elf_header[ i ].segment_size || ! elf_header[ i ].memory_size ) continue;

		// segment source
		uint8_t *source = (uint8_t *) ((uintptr_t) elf + elf_header[ i ].segment_offset);

		// segment destination
		uint8_t *destination = (uint8_t *) ((elf_header[ i ].virtual_address - KERNEL_EXEC_base_address) + exec.base);

		// copy segment content into place
		for( uint64_t j = INIT; j < elf_header[ i ].memory_size; j++ ) destination[ j ] = source[ j ];
	}

	// map executable area
	if( ! kernel_page_map( (uint64_t *) exec.task -> cr3, exec.base & ~KERNEL_MEMORY_mirror, KERNEL_EXEC_base_address, exec.page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | (exec.task -> type << KERNEL_PAGE_TYPE_offset) ) ) { kernel_memory_release( (uintptr_t) exec.base, exec.page ); kernel_exec_cancel( (struct KERNEL_STRUCTURE_EXEC *) &exec ); return EMPTY; }

	// process memory usage
	exec.task -> page += exec.page;

	//----------------------------------------------------------------------

	// create virtual memory map
	if( ! (exec.task -> memory = (uint32_t *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( (kernel -> page_limit >> STD_SHIFT_8) + TRUE ) >> STD_SHIFT_PAGE )) ) { kernel_exec_cancel( (struct KERNEL_STRUCTURE_EXEC *) &exec ); return EMPTY; }

	// checkpoint: memory
	exec.level++;

	// fill up
	for( uint64_t i = (KERNEL_EXEC_base_address >> STD_SHIFT_PAGE) >> STD_SHIFT_32; i < kernel -> page_limit >> STD_SHIFT_32; i++ ) exec.task -> memory[ i ] = STD_MAX_unsigned;

	// mark as occupied pages used by the executable
	kernel_memory_acquire( exec.task -> memory, exec.page, KERNEL_EXEC_base_address >> STD_SHIFT_PAGE, kernel -> page_limit );

	// unlock access to binary memory map
	MACRO_UNLOCK( *((uint8_t *) exec.task -> memory + MACRO_PAGE_ALIGN_UP( (kernel -> page_limit >> STD_SHIFT_8) + TRUE ) - STD_SIZE_BYTE_byte) );

	//----------------------------------------------------------------------

	// connect required functions new locations / from another library
	kernel_library_link( elf, exec.base, FALSE );

	//----------------------------------------------------------------------

	// map kernel
	kernel_page_merge( (uint64_t *) kernel -> page_base_address, (uint64_t *) exec.task -> cr3 );

	// release workbench
	kernel_memory_release( exec.workbench, MACRO_PAGE_ALIGN_UP( exec.socket -> file.limit ) >> STD_SHIFT_PAGE );

	// close file
	kernel_vfs_socket_delete( exec.socket );

	//----------------------------------------------------------------------

	// process ready to run
	exec.task -> flags |= KERNEL_TASK_FLAG_active | KERNEL_TASK_FLAG_init;

	// PID of new task
	return exec.task -> pid;;
}
