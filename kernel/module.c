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

	//----------------------------------------------------------------------

	// prepare Paging table for new process
	module -> cr3 = kernel_memory_alloc_page() | KERNEL_PAGE_logical;

	//----------------------------------------------------------------------

	// insert into paging, context stack of new process
	kernel_page_alloc( (uintptr_t *) module -> cr3, KERNEL_STACK_address, 2, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_process );

	// set initial startup configuration for new process
	struct KERNEL_IDT_STRUCTURE_RETURN *context = (struct KERNEL_IDT_STRUCTURE_RETURN *) (kernel_page_address( (uintptr_t *) module -> cr3, KERNEL_STACK_pointer - STD_PAGE_byte ) + KERNEL_PAGE_logical + (STD_PAGE_byte - sizeof( struct KERNEL_IDT_STRUCTURE_RETURN )));

	// code descriptor
	context -> cs = offsetof( struct KERNEL_GDT_STRUCTURE, cs_ring3 ) | 0x03;

	// basic processor state flags
	context -> eflags = KERNEL_TASK_EFLAGS_default;

	// current top-of-stack pointer for module
	context -> rsp = KERNEL_STACK_pointer;

	// stack descriptor
	context -> ss = offsetof( struct KERNEL_GDT_STRUCTURE, ds_ring3 ) | 0x03;

	// set the process entry address
	context -> rip = elf -> entry_ptr;

	//----------------------------------------------------------------------

	// the context stack top pointer
	module -> rsp = KERNEL_STACK_pointer - sizeof( struct KERNEL_IDT_STRUCTURE_RETURN );

	// process memory usage
	module -> page += MACRO_PAGE_ALIGN_UP( args ) >> STD_SHIFT_PAGE;

	// process stack size
	module -> stack += MACRO_PAGE_ALIGN_UP( args ) >> STD_SHIFT_PAGE;




	MACRO_DEBUF();

	// to be continued
}
