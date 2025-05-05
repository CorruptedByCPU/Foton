/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct KERNEL_STRUCTURE_MODULE {
	uint8_t					level;
	uint8_t					*path;
	uint64_t				limit;
	struct KERNEL_STRUCTURE_VFS_SOCKET	*socket;
	uintptr_t				workbench;
	struct KERNEL_STRUCTURE_TASK		*task;
	uint8_t					*stack;
	uint64_t				stack_byte;
	uint64_t				page;
	uintptr_t				base;
};

static void kernel_module_cancel( struct KERNEL_STRUCTURE_MODULE *module ) {
	// undo performed operations depending on cavity
	switch( module -> level ) {
		case 5: {
			// cannot foresee any error at this level and above
		}

		case 4: {
			// release paging structure
			kernel_page_deconstruct( module -> task -> cr3, module -> task -> type );
		}

		case 3: {
			// release task entry
			module -> task -> flags = EMPTY;
		}

		case 2: {
			// release temporary area
			kernel_memory_release( module -> workbench, MACRO_PAGE_ALIGN_UP( module -> socket -> file.limit ) >> STD_SHIFT_PAGE );
		}

		case 1: {
			// close file
			kernel_vfs_socket_delete( module -> socket );
		}
	}
}

uint64_t kernel_module( uint8_t *name, uint64_t limit ) {
	// module state
	struct KERNEL_STRUCTURE_MODULE module = { INIT };

	// default location of modules
	uint8_t path_default[ 13 ] = "/lib/modules/";

	// combine default path with module name
	module.path = (uint8_t *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( path_default ) + limit ) >> STD_SHIFT_PAGE ); for( uint8_t i = INIT; i < sizeof( path_default ); i++ ) module.path[ module.limit++ ] = path_default[ i ]; for( uint64_t i = INIT; i < lib_string_word_end( name, limit, STD_ASCII_SPACE ); i++ ) module.path[ module.limit++ ] = name[ i ];

	// open file
	module.socket = (struct KERNEL_STRUCTURE_VFS_SOCKET *) &kernel -> vfs_base_address[ kernel_syscall_file_open( module.path, module.limit ) ];

	// release path, no more needed
	kernel_memory_release( (uintptr_t) module.path, MACRO_PAGE_ALIGN_UP( sizeof( path_default ) + limit ) >> STD_SHIFT_PAGE );

	// file doesn't exist?
	if( ! module.socket ) return EMPTY;	// yep

	// checkpoint: socket --------------------------------------------------
	module.level++;

	// assign temporary area for parsing file content
	if( ! (module.workbench = kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( module.socket -> file.limit ) >> STD_SHIFT_PAGE )) ) { kernel_module_cancel( (struct KERNEL_STRUCTURE_MODULE *) &module ); return EMPTY; };

	// checkpoint: workbench -----------------------------------------------
	module.level++;

	// load content of file
	kernel -> storage_base_address[ module.socket -> storage ].vfs -> file_read( module.socket, (uint8_t *) module.workbench, EMPTY, module.socket -> file.limit );

	// file contains ELF header?
	if( ! lib_elf_identify( module.workbench ) ) { kernel_module_cancel( (struct KERNEL_STRUCTURE_MODULE *) &module ); return EMPTY; };	// no

	// ELF file properties
	struct LIB_ELF_STRUCTURE *elf = (struct LIB_ELF_STRUCTURE *) module.workbench;

	// executable?
	if( elf -> type != LIB_ELF_TYPE_executable ) { kernel_module_cancel( (struct KERNEL_STRUCTURE_MODULE *) &module ); return EMPTY; }	// no

	// add new task entry
	if( ! (module.task = kernel_task_add( name, limit )) ) { kernel_module_cancel( (struct KERNEL_STRUCTURE_MODULE *) &module ); return EMPTY; }	// end of resources

	// mark task as module
	module.task -> flags |= STD_TASK_FLAG_module;
	module.task -> type = KERNEL_TASK_TYPE_MODULE;

	// checkpoint: task ----------------------------------------------------
	module.level++;

	// create paging table
	if( ! (module.task -> cr3 = (uint64_t *) (kernel_memory_alloc_page() | KERNEL_MEMORY_mirror)) ) { kernel_module_cancel( (struct KERNEL_STRUCTURE_MODULE *) &module ); return EMPTY; }

	// checkpoint: paging --------------------------------------------------
	module.level++;

	// describe area under context stack
	if( ! kernel_page_alloc( module.task -> cr3, KERNEL_STACK_address, KERNEL_STACK_LIMIT_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | (module.task -> type << KERNEL_PAGE_TYPE_offset) ) ) { kernel_module_cancel( (struct KERNEL_STRUCTURE_MODULE *) &module ); return EMPTY; }

	// set initial startup configuration for new process
	struct KERNEL_STRUCTURE_IDT_RETURN *context = (struct KERNEL_STRUCTURE_IDT_RETURN *) (kernel_page_address( module.task -> cr3, KERNEL_STACK_pointer - STD_PAGE_byte ) + KERNEL_MEMORY_mirror + (STD_PAGE_byte - sizeof( struct KERNEL_STRUCTURE_IDT_RETURN )));

	// set the process entry address
	context -> rip		= elf -> entry_ptr;

	// code descriptor
	context -> cs		= offsetof( struct KERNEL_STRUCTURE_GDT, cs_ring0 );

	// basic processor state flags
	context -> eflags	= KERNEL_TASK_EFLAGS_default;

	// stack top pointer
	context -> rsp		= KERNEL_STACK_pointer;

	// stack descriptor
	context -> ss		= offsetof( struct KERNEL_STRUCTURE_GDT, ds_ring0 );

	//----------------------------------------------------------------------

	// context stack top pointer
	module.task -> rsp	= KERNEL_STACK_pointer - sizeof( struct KERNEL_STRUCTURE_IDT_RETURN );

	//----------------------------------------------------------------------

	// ELF header properties
	struct LIB_ELF_STRUCTURE_HEADER *elf_header = (struct LIB_ELF_STRUCTURE_HEADER *) ((uint64_t) elf + elf -> header_offset);

	// find furthest position in page of initialized module
	for( uint16_t i = INIT; i < elf -> header_count; i++ ) {
		// ignore blank entry or not loadable
		if( elf_header[ i ].type != LIB_ELF_HEADER_TYPE_load || ! elf_header[ i ].segment_size  || ! elf_header[ i ].memory_size ) continue;

		// update module area limit?
		if( module.page < MACRO_PAGE_ALIGN_UP( elf_header[ i ].virtual_address + elf_header[ i ].memory_size ) >> STD_SHIFT_PAGE ) module.page = MACRO_PAGE_ALIGN_UP( elf_header[ i ].virtual_address + elf_header[ i ].memory_size ) >> STD_SHIFT_PAGE;
	}

	// acquire area for module
	if( ! (module.base = kernel_memory_alloc( module.page )) ) { kernel_module_cancel( (struct KERNEL_STRUCTURE_MODULE *) &module ); return EMPTY; }

	// load executable segments in place
	for( uint16_t i = INIT; i < elf -> header_count; i++ ) {
		// ignore blank entry or not loadable
 		if( elf_header[ i ].type != LIB_ELF_HEADER_TYPE_load || ! elf_header[ i ].segment_size || ! elf_header[ i ].memory_size ) continue;

		// segment source
		uint8_t *source = (uint8_t *) ((uintptr_t) elf + elf_header[ i ].segment_offset);

		// segment destination
		uint8_t *destination = (uint8_t *) (elf_header[ i ].virtual_address + module.base);

		// copy segment content into place
		for( uint64_t j = INIT; j < elf_header[ i ].memory_size; j++ ) destination[ j ] = source[ j ];
	}

	// process memory usage
	module.task -> page += module.page;

	//----------------------------------------------------------------------

	// update module entry address
	context -> rip += module.base;

	//----------------------------------------------------------------------

	// module use same memory map as kernel
	module.task -> memory = kernel -> memory_base_address;

	//----------------------------------------------------------------------

	// map kernel
	kernel_page_merge( (uint64_t *) kernel -> page_base_address, (uint64_t *) module.task -> cr3 );

	// release workbench
	kernel_memory_release( module.workbench, MACRO_PAGE_ALIGN_UP( module.socket -> file.limit ) >> STD_SHIFT_PAGE );

	// close file
	kernel_vfs_socket_delete( module.socket );

	//----------------------------------------------------------------------

	// process ready to run
	module.task -> flags |= KERNEL_TASK_FLAG_active | KERNEL_TASK_FLAG_init;

	// PID of new task
	return module.task -> pid;
}

uint64_t kernel_module_thread( uintptr_t function, uint8_t *name, uint64_t length ) {
	// create a new thread in task queue
	struct KERNEL_STRUCTURE_TASK *thread = kernel_task_add( name, length );

	//----------------------------------------------------------------------

	// prepare Paging table for new process
	thread -> cr3 = (uint64_t *) (kernel_memory_alloc_page() | KERNEL_MEMORY_mirror);

	// mark task as thread
	thread -> flags |= STD_TASK_FLAG_thread;
	thread -> type = KERNEL_TASK_TYPE_THREAD;

	//----------------------------------------------------------------------

	// describe space under thread context stack
	kernel_page_alloc( (uint64_t *) thread -> cr3, KERNEL_STACK_address, KERNEL_STACK_LIMIT_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | (thread -> type << KERNEL_PAGE_TYPE_offset) );

	// set initial startup configuration for new process
	struct KERNEL_STRUCTURE_IDT_RETURN *context = (struct KERNEL_STRUCTURE_IDT_RETURN *) (kernel_page_address( (uint64_t *) thread -> cr3, KERNEL_STACK_pointer - STD_PAGE_byte ) + KERNEL_MEMORY_mirror + (STD_PAGE_byte - sizeof( struct KERNEL_STRUCTURE_IDT_RETURN )));

	// set the process entry address
	context -> rip		= function;

	// code descriptor
	context -> cs		= offsetof( struct KERNEL_STRUCTURE_GDT, cs_ring0 );

	// basic processor state flags
	context -> eflags	= KERNEL_TASK_EFLAGS_default;

	// stack top pointer
	context -> rsp		= KERNEL_STACK_pointer;

	// stack descriptor
	context -> ss		= offsetof( struct KERNEL_STRUCTURE_GDT, ds_ring0 );

	//----------------------------------------------------------------------

	// context stack top pointer
	thread -> rsp		= KERNEL_STACK_pointer - sizeof( struct KERNEL_STRUCTURE_IDT_RETURN );

	//----------------------------------------------------------------------

	// aquire parent task properties
	struct KERNEL_STRUCTURE_TASK *current = kernel_task_current();

	// threads use same memory map as parent
	thread -> memory = current -> memory;

	//----------------------------------------------------------------------

	// map kernel
	kernel_page_merge( (uint64_t *) current -> cr3, (uint64_t *) thread -> cr3 );

	// thread ready to run
	thread -> flags |= STD_TASK_FLAG_active | STD_TASK_FLAG_module | STD_TASK_FLAG_thread | STD_TASK_FLAG_init;

	// PID of new thread
	return thread -> pid;
}
