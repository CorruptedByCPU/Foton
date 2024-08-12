/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_module_load( uint8_t *name, uint64_t length ) {
	// default location of modules
	uint64_t path_length = 0;
	uint8_t path_default[ 20 ] = "/system/lib/modules/";

	// set file path name
	uint8_t path[ 20 + LIB_VFS_NAME_limit ];
	for( uint64_t i = 0; i < 20; i++ ) path[ path_length++ ] = path_default[ i ];
	for( uint64_t i = 0; i < length; i++ ) path[ path_length++ ] = name[ i ];

	// retrieve information about module file
	struct KERNEL_STRUCTURE_VFS *socket = (struct KERNEL_STRUCTURE_VFS *) kernel_vfs_file_open( path, path_length );

	// if module does not exist
	if( ! socket ) return;	// ignore

	// gather information about file
	struct KERNEL_STRUCTURE_VFS_PROPERTIES properties;
	kernel_vfs_file_properties( socket, (struct KERNEL_STRUCTURE_VFS_PROPERTIES *) &properties );

	// assign area for workbench
	uintptr_t workbench;
	if( ! (workbench = kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( properties.byte ) >> STD_SHIFT_PAGE )) ) {
		// close file
		kernel_vfs_file_close( socket );

		// done
		return;
	}

	// load module into workbench space
	kernel_vfs_file_read( socket, (uint8_t *) workbench, EMPTY, properties.byte );

	// close file
	kernel_vfs_file_close( socket );

	//----------------------------------------------------------------------

	// ELF structure properties
	struct LIB_ELF_STRUCTURE *elf = (struct LIB_ELF_STRUCTURE *) workbench;

	// create a new job in task queue
	struct KERNEL_STRUCTURE_TASK *module = kernel_task_add( name, length );

	// mark task as module
	module -> flags |= STD_TASK_FLAG_module;

	//----------------------------------------------------------------------

	// prepare Paging table for new process
	module -> cr3 = kernel_memory_alloc( TRUE );

	// page used for structure
	kernel -> page_structure++;

	// all allocated pages, mark as type of MODULE
	module -> page_type = KERNEL_PAGE_TYPE_MODULE;

	//----------------------------------------------------------------------

	// insert into paging, context stack
	kernel_page_alloc( (uint64_t *) module -> cr3, KERNEL_STACK_address, KERNEL_STACK_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | (module -> page_type << KERNEL_PAGE_TYPE_offset) );

	// set initial startup configuration for new process
	struct KERNEL_STRUCTURE_IDT_RETURN *context = (struct KERNEL_STRUCTURE_IDT_RETURN *) (kernel_page_address( (uint64_t *) module -> cr3, KERNEL_STACK_pointer - STD_PAGE_byte ) + KERNEL_PAGE_mirror + (STD_PAGE_byte - sizeof( struct KERNEL_STRUCTURE_IDT_RETURN )));

	// code descriptor
	context -> cs = offsetof( struct KERNEL_STRUCTURE_GDT, cs_ring0 );

	// basic processor state flags
	context -> eflags = KERNEL_TASK_EFLAGS_default;

	// current top-of-stack pointer for module
	context -> rsp = KERNEL_STACK_pointer;

	// stack descriptor
	context -> ss = offsetof( struct KERNEL_STRUCTURE_GDT, ds_ring0 );

	// set process entry address
	context -> rip = elf -> entry_ptr;

	//----------------------------------------------------------------------

	// context stack top pointer
	module -> rsp = KERNEL_STACK_pointer - sizeof( struct KERNEL_STRUCTURE_IDT_RETURN );

	//----------------------------------------------------------------------

	// calculate unpacked module size in Pages
	uint64_t module_page = EMPTY;

	// ELF header properties
	struct LIB_ELF_STRUCTURE_HEADER *elf_h = (struct LIB_ELF_STRUCTURE_HEADER *) ((uint64_t) elf + elf -> headers_offset);

	// calculate memory space of segments used by module
	for( uint16_t i = 0; i < elf -> h_entry_count; i++ ) {
		// ignore blank entry or not loadable
 		if( elf_h[ i ].type != LIB_ELF_HEADER_TYPE_load || ! elf_h[ i ].memory_size ) continue;

		// update executable space size?
		if( module_page < MACRO_PAGE_ALIGN_UP( elf_h[ i ].virtual_address + elf_h[ i ].memory_size ) >> STD_SHIFT_PAGE ) module_page = MACRO_PAGE_ALIGN_UP( elf_h[ i ].virtual_address + elf_h[ i ].memory_size ) >> STD_SHIFT_PAGE;
	}

	// allocate module space
	uintptr_t module_content = kernel_memory_alloc( module_page );

	// debug
	kernel_log( (uint8_t *) "Module %s at 0x%X\n", name, module_content );

	// load module segments in place
	for( uint16_t i = 0; i < elf -> h_entry_count; i++ ) {
		// ignore blank entry or not loadable
 		if( ! elf_h[ i ].type || ! elf_h[ i ].memory_size || elf_h[ i ].type != LIB_ELF_HEADER_TYPE_load ) continue;

		// segment destination
		uint8_t *destination = (uint8_t *) (elf_h[ i ].virtual_address + module_content);

		// segment source
		uint8_t *source = (uint8_t *) ((uintptr_t) elf + elf_h[ i ].segment_offset);

		// copy segment content into place
		for( uint64_t j = 0; j < elf_h[ i ].memory_size; j++ ) destination[ j ] = source[ j ];
	}

	// insert into paging, module area
	// uintptr_t module_memory = KERNEL_MODULE_base_address + (kernel_memory_acquire( kernel -> module_map_address, module_page, KERNEL_MEMORY_HIGH, kernel -> page_limit ) << STD_SHIFT_PAGE);
	// kernel_page_map( (uint64_t *) module -> cr3, module_content & ~KERNEL_PAGE_mirror, module_memory, module_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | (module -> page_type << KERNEL_PAGE_TYPE_offset) );

	// map module space to kernel space
	// kernel_page_map( (uint64_t *) kernel -> page_base_address, module_content & ~KERNEL_PAGE_mirror, module_memory, module_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | (module -> page_type << KERNEL_PAGE_TYPE_offset) );

	// update module entry address
	context -> rip += module_content;	// module_memory

	//----------------------------------------------------------------------

	// module uses same memory map as kernel
	module -> memory_map = kernel -> memory_base_address;

	//----------------------------------------------------------------------

	// release workbench
	kernel_memory_release( workbench, MACRO_PAGE_ALIGN_UP( properties.byte ) >> STD_SHIFT_PAGE );

	// map kernel space to process
	kernel_page_merge( (uint64_t *) kernel -> page_base_address, (uint64_t *) module -> cr3 );

	// module ready to run
	module -> flags |= KERNEL_TASK_FLAG_active | KERNEL_TASK_FLAG_init;
}

int64_t kernel_module_thread( uintptr_t function, uint8_t *name, uint64_t length ) {
	// create a new thread in task queue
	struct KERNEL_STRUCTURE_TASK *thread = kernel_task_add( name, length );

	//----------------------------------------------------------------------

	// prepare Paging table for new process
	thread -> cr3 = kernel_memory_alloc_page() | KERNEL_PAGE_mirror;

	// all allocated pages, mark as type of THREAD
	thread -> page_type = KERNEL_PAGE_TYPE_THREAD;

	//----------------------------------------------------------------------

	// describe space under thread context stack
	kernel_page_alloc( (uint64_t *) thread -> cr3, KERNEL_STACK_address, KERNEL_STACK_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | (thread -> page_type << KERNEL_PAGE_TYPE_offset) );

	// set initial startup configuration for new process
	struct KERNEL_STRUCTURE_IDT_RETURN *context = (struct KERNEL_STRUCTURE_IDT_RETURN *) (kernel_page_address( (uint64_t *) thread -> cr3, KERNEL_STACK_pointer - STD_PAGE_byte ) + KERNEL_PAGE_mirror + (STD_PAGE_byte - sizeof( struct KERNEL_STRUCTURE_IDT_RETURN )));

	// code descriptor
	context -> cs = offsetof( struct KERNEL_STRUCTURE_GDT, cs_ring0 );

	// basic processor state flags
	context -> eflags = KERNEL_TASK_EFLAGS_default;

	// stack descriptor
	context -> ss = offsetof( struct KERNEL_STRUCTURE_GDT, ds_ring0 );

	// current top-of-stack pointer for module
	context -> rsp = KERNEL_STACK_pointer;

	// set thread entry address
	context -> rip = function;

	//----------------------------------------------------------------------

	// context stack top pointer
	thread -> rsp = KERNEL_STACK_pointer - sizeof( struct KERNEL_STRUCTURE_IDT_RETURN );

	//----------------------------------------------------------------------

	// aquire parent task properties
	struct KERNEL_STRUCTURE_TASK *parent = kernel_task_active();

	// threads use same memory map as parent
	thread -> memory_map = parent -> memory_map;

	//----------------------------------------------------------------------

	// map parent space to thread
	kernel_page_merge( (uint64_t *) parent -> cr3, (uint64_t *) thread -> cr3 );

	// thread ready to run
	thread -> flags |= STD_TASK_FLAG_active | STD_TASK_FLAG_module | STD_TASK_FLAG_thread | STD_TASK_FLAG_init;

	// return process ID of new thread
	return thread -> pid;
}