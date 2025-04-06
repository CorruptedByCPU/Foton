/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

static void kernel_exec_cancel( struct KERNEL_STRUCTURE_EXEC *exec ) {
	// undo performed operations depending on cavity
	switch( exec -> level ) {
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
	if( ! (exec.task -> cr3 = (uint64_t *) kernel_memory_alloc( TRUE )) ) { kernel_exec_cancel( (struct KERNEL_STRUCTURE_EXEC *) &exec ); return EMPTY; }

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


	

// debug
kernel_log( (uint8_t *) "Exec, OK.\n" );

	// debug
	return EMPTY;
}
