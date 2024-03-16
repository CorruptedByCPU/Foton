/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_module_load( uint8_t *name, uint64_t length ) {
	// default location of modules
	uint8_t path_default[ 20 ] = "/system/lib/modules/";

	// assign area for path to file
	uint64_t path_length = 0;
	uint8_t *path = (uint8_t *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( length + sizeof( path_default ) + 1 ) >> STD_SHIFT_PAGE );

	// set file path name
	for( uint64_t i = 0; i < sizeof( path_default ); i++ ) path[ path_length++ ] = path_default[ i ];
	for( uint64_t i = 0; i < length; i++ ) path[ path_length++ ] = name[ i ]; name[ path_length ] = EMPTY;

	// retrieve information about module file
	// file.id_storage = kernel -> storage_old_root_id;
	struct KERNEL_VFS_STRUCTURE *socket = (struct KERNEL_VFS_STRUCTURE *) kernel_vfs_file_open( path, path_length, KERNEL_VFS_MODE_read );

	// if module does not exist
	if( ! socket ) return;	// ignore

	// gather information about file
	struct KERNEL_VFS_STRUCTURE_PROPERTIES file_properties = kernel_vfs_file_properties( socket );

	// prepare space for workbench
	uint8_t *workbench = EMPTY; 
	if( ! (workbench = (uint8_t *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( file_properties.byte ) >> STD_SHIFT_PAGE )) ) {
		// close file socket
		kernel_vfs_file_close( socket );

		// release path to file
		kernel_memory_release( (uintptr_t) path, MACRO_PAGE_ALIGN_UP( length + sizeof( path_default ) + 1 ) >> STD_SHIFT_PAGE );

		// no enough memory
		return;
	}

	// load whole module into workbench area
	kernel_vfs_file_read( socket, workbench, EMPTY, file_properties.byte );

	//----------------------------------------------------------------------

	// file contains proper ELF header?
	if( ! lib_elf_identify( (uintptr_t) workbench ) ) {
		// release workbench area
		kernel_memory_release( (uintptr_t) workbench, MACRO_PAGE_ALIGN_UP( file_properties.byte ) >> STD_SHIFT_PAGE );

		// close file socket
		kernel_vfs_file_close( socket );

		// release path to file
		kernel_memory_release( (uintptr_t) path, MACRO_PAGE_ALIGN_UP( length + sizeof( path_default ) + 1 ) >> STD_SHIFT_PAGE );

		// no enough memory
		return;
	}

	// ELF structure properties
	struct LIB_ELF_STRUCTURE *elf = (struct LIB_ELF_STRUCTURE *) workbench;

	//----------------------------------------------------------------------

	// create a new job in task queue
	struct KERNEL_TASK_STRUCTURE *module = kernel_task_add( file_properties.name, file_properties.name_length );

	// mark task as module
	module -> flags |= KERNEL_TASK_FLAG_module;

	//----------------------------------------------------------------------

	// prepare Paging table for new process
	module -> cr3 = kernel_memory_alloc_page() | KERNEL_PAGE_logical;

	//----------------------------------------------------------------------

	// insert into paging, context stack of new process
	kernel_page_alloc( (uintptr_t *) module -> cr3, KERNEL_STACK_address, KERNEL_STACK_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_process );

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

	// map module space to paging array
	uintptr_t module_memory = KERNEL_MODULE_base_address + (kernel_memory_acquire( kernel -> module_base_address, module_page ) << STD_SHIFT_PAGE);
	kernel_page_map( (uintptr_t *) module -> cr3, module_content, module_memory, module_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_external );

	// map module space to kernel space
	kernel_page_map( (uintptr_t *) kernel -> page_base_address, module_content, module_memory, module_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_external );

	// set module entry address
	context -> rip = module_memory + elf -> entry_ptr;

	//----------------------------------------------------------------------

	// module uses same memory map as kernel
	module -> memory_map = kernel -> memory_base_address;

	//----------------------------------------------------------------------

	// release workbench area
	kernel_memory_release( (uintptr_t) workbench, MACRO_PAGE_ALIGN_UP( file_properties.byte ) >> STD_SHIFT_PAGE );

	// close file socket
	kernel_vfs_file_close( socket );

	// release path to file
	kernel_memory_release( (uintptr_t) path, MACRO_PAGE_ALIGN_UP( length + sizeof( path_default ) + 1 ) >> STD_SHIFT_PAGE );

	// map kernel space to process
	kernel_page_merge( (uint64_t *) kernel -> page_base_address, (uint64_t *) module -> cr3 );

	// module ready to run
	module -> flags |= KERNEL_TASK_FLAG_active | KERNEL_TASK_FLAG_init;
}
