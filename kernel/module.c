/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_module_load( uint8_t *name, uint64_t length ) {
	// remember module name length
	uint64_t name_length = length;

	// retrieve file properties
	uint8_t path[ 20 + LIB_VFS_name_limit + 1 ] = "/system/lib/modules/";
	for( uint64_t i = 0; i < name_length; i++ ) path[ i + 20 ] = name[ i ];

	// retrieve information about module file
	struct STD_FILE_STRUCTURE file = kernel_storage_file( kernel -> storage_root_id, path, 20 + name_length );

	// if module does not exist
	if( ! file.id ) return;

	// prepare space for workbench
	uintptr_t workbench = EMPTY; 
	if( ! (workbench = kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( file.size_byte ) >> STD_SHIFT_PAGE )) ) return;	// no enough memory

	// load module into workbench space
	kernel_storage_read( kernel -> storage_root_id, file.id, workbench );

	//----------------------------------------------------------------------

	// file contains proper ELF header?
	if( ! lib_elf_identify( workbench ) ) return;	// no

	// ELF structure properties
	struct LIB_ELF_STRUCTURE *elf = (struct LIB_ELF_STRUCTURE *) workbench;

	// it's an executable file?
	if( elf -> type != LIB_ELF_TYPE_executable ) return;	// no

	//----------------------------------------------------------------------

	// create a new job in task queue
	struct KERNEL_TASK_STRUCTURE *module = kernel_task_add( name, length );

	// mark task as module
	module -> flags |= KERNEL_TASK_FLAG_module;

	//----------------------------------------------------------------------

	// prepare Paging table for new process
	module -> cr3 = kernel_memory_alloc_page() | KERNEL_PAGE_logical;

	//----------------------------------------------------------------------

	// insert into paging, context stack of new process
	kernel_page_alloc( (uintptr_t *) module -> cr3, KERNEL_STACK_address, 2, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_process );

	// set initial startup configuration for new process
	struct KERNEL_IDT_STRUCTURE_RETURN *context = (struct KERNEL_IDT_STRUCTURE_RETURN *) (kernel_page_address( (uintptr_t *) module -> cr3, KERNEL_STACK_pointer - STD_PAGE_byte ) + KERNEL_PAGE_logical + (STD_PAGE_byte - sizeof( struct KERNEL_IDT_STRUCTURE_RETURN )));

	// code descriptor
	context -> cs = offsetof( struct KERNEL_GDT_STRUCTURE, cs_ring0 );

	// basic processor state flags
	context -> eflags = KERNEL_TASK_EFLAGS_default;

	// current top-of-stack pointer for module
	context -> rsp = KERNEL_STACK_pointer;

	// stack descriptor
	context -> ss = offsetof( struct KERNEL_GDT_STRUCTURE, ds_ring0 );

	// set process entry address
	context -> rip = elf -> entry_ptr;

	//----------------------------------------------------------------------

	// context stack top pointer
	module -> rsp = KERNEL_STACK_pointer - sizeof( struct KERNEL_IDT_STRUCTURE_RETURN );

	//----------------------------------------------------------------------

	// calculate unpacked module size in Pages
	uint64_t size_page = EMPTY;

	// ELF header properties
	struct LIB_ELF_STRUCTURE_HEADER *elf_h = (struct LIB_ELF_STRUCTURE_HEADER *) ((uint64_t) elf + elf -> headers_offset);

	// calculate memory space of segments used by module
	for( uint16_t i = 0; i < elf -> h_entry_count; i++ ) {
		// ignore blank entry or not loadable
 		if( ! elf_h[ i ].type || ! elf_h[ i ].memory_size || elf_h[ i ].type != LIB_ELF_HEADER_TYPE_load ) continue;

		// update executable space size?
		if( size_page < MACRO_PAGE_ALIGN_UP( elf_h[ i ].virtual_address + elf_h[ i ].segment_size ) >> STD_SHIFT_PAGE ) size_page = MACRO_PAGE_ALIGN_UP( elf_h[ i ].virtual_address + elf_h[ i ].segment_size ) >> STD_SHIFT_PAGE;
	}

	// allocate module space
	uintptr_t module_content = kernel_memory_alloc( size_page );

	// load module segments in place
	for( uint16_t i = 0; i < elf -> h_entry_count; i++ ) {
		// ignore blank entry or not loadable
 		if( ! elf_h[ i ].type || ! elf_h[ i ].memory_size || elf_h[ i ].type != LIB_ELF_HEADER_TYPE_load ) continue;

		// segment destination
		uint8_t *destination = (uint8_t *) (elf_h[ i ].virtual_address + module_content);

		// segment source
		uint8_t *source = (uint8_t *) ((uintptr_t) elf + elf_h[ i ].segment_offset);

		// copy segment content into place
		for( uint64_t j = 0; j < elf_h[ i ].segment_size; j++ ) destination[ j ] = source[ j ];
	}

	// map module space to paging array
	uintptr_t module_memory = KERNEL_MODULE_base_address + (kernel_memory_acquire( kernel -> module_base_address, size_page ) << STD_SHIFT_PAGE);
	kernel_page_map( (uintptr_t *) module -> cr3, module_content, module_memory, size_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_external );

	// map module space to kernel space
	kernel_page_map( (uintptr_t *) kernel -> page_base_address, module_content, module_memory, size_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_external );


	// set module entry address
	context -> rip = module_memory + elf -> entry_ptr;

	//----------------------------------------------------------------------

	// module uses same memory map as kernel
	module -> memory_map = kernel -> memory_base_address;

	//----------------------------------------------------------------------

	// release workbench
	kernel_memory_release( workbench, MACRO_PAGE_ALIGN_UP( file.size_byte ) >> STD_SHIFT_PAGE );

	// map kernel space to process
	kernel_page_merge( (uint64_t *) kernel -> page_base_address, (uint64_t *) module -> cr3 );

	// module ready to run
	module -> flags |= KERNEL_TASK_FLAG_active | KERNEL_TASK_FLAG_init;
}
