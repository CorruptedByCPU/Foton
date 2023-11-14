/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

static void kernel_exec_cancel( struct KERNEL_EXEC_STRUCTURE_TMP *tmp ) {
	// undo performed operations depending on cavity
	switch( tmp -> level ) {
		case 9: {
			// cannot foresee an error at this level and above
		}
		case 8: {
			// release memory map from task entry
			kernel_memory_release( (uintptr_t) tmp -> exec -> memory_map, MACRO_PAGE_ALIGN_UP( kernel -> page_limit >> STD_SHIFT_8 ) >> STD_SHIFT_PAGE );
		}
		case 7: {
			// detach process area from paging structure
			kernel_page_detach( (uintptr_t *) tmp -> exec -> cr3, KERNEL_EXEC_base_address, tmp -> exec_page );
		}
		case 6: {
			// release process area
			kernel_memory_release( (uintptr_t) tmp -> exec_base_address, tmp -> exec_page );
		}
		case 5: {
			// detach stack from paging structure
			kernel_page_detach( (uintptr_t *) tmp -> exec -> cr3, MACRO_PAGE_ALIGN_DOWN( KERNEL_TASK_STACK_pointer - tmp -> stack_byte ), MACRO_PAGE_ALIGN_UP( tmp -> stack_byte ) >> STD_SHIFT_PAGE );
		}
		case 4: {
			// release stack area
			kernel_memory_release( (uintptr_t) tmp -> stack, MACRO_PAGE_ALIGN_UP( tmp -> stack_byte ) >> STD_SHIFT_PAGE );
		}
		case 3: {
			// release paging structure
			kernel_page_deconstruct( (uintptr_t *) tmp -> exec -> cr3 );
		}
		case 2: {
			// release task entry
			tmp -> exec -> flags = EMPTY;
		}
		case 1: {
			// release workbench area
			kernel_memory_release( tmp -> workbench, MACRO_PAGE_ALIGN_UP( tmp -> file.size_byte ) >> STD_SHIFT_PAGE );
		}
	}
}

int64_t kernel_exec( uint8_t *name, uint64_t length, uint8_t stream_flow ) {
	// prepare temporary execution area
	struct KERNEL_EXEC_STRUCTURE_TMP tmp = { EMPTY };

	// length of exec name
	uint64_t exec_length = lib_string_word( name, length );

	// retrieve file properties
	uint8_t path[ 12 + LIB_VFS_name_limit + 1 ] = "/system/bin/";
	for( uint64_t i = 0; i < exec_length; i++ ) path[ i + 12 ] = name[ i ];

	// retrieve information about file to execute
	tmp.file = kernel_storage_file( kernel -> storage_root_id, path, 12 + exec_length );

	// if file doesn't exist
	if( ! tmp.file.id ) return STD_ERROR_file_not_found;	// yep

	// prepare area for workbench
	if( ! (tmp.workbench = kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( tmp.file.size_byte ) >> STD_SHIFT_PAGE )) ) return STD_ERROR_memory_low;

	// checkpoint reached: assigned area for temporary file
	tmp.level++;

	// load file into workbench space
	kernel_storage_read( kernel -> storage_root_id, tmp.file.id, tmp.workbench );

	//----------------------------------------------------------------------

	// file contains proper ELF header?
	if( ! lib_elf_identify( tmp.workbench ) ) { kernel_exec_cancel( (struct KERNEL_EXEC_STRUCTURE_TMP *) &tmp ); return STD_ERROR_file_not_elf; }	// no

	// ELF structure properties
	struct LIB_ELF_STRUCTURE *elf = (struct LIB_ELF_STRUCTURE *) tmp.workbench;

	// it's an executable file?
	if( elf -> type != LIB_ELF_TYPE_executable ) { kernel_exec_cancel( (struct KERNEL_EXEC_STRUCTURE_TMP *) &tmp ); return STD_ERROR_file_not_executable; }	// no

	// load libraries required by file or low memory occured
	if( ! kernel_library( elf ) ) { kernel_exec_cancel( (struct KERNEL_EXEC_STRUCTURE_TMP *) &tmp ); return STD_ERROR_memory_low; }

	//----------------------------------------------------------------------

	// create a new job in task queue or low memory occured
	if( ! (tmp.exec = kernel_task_add( name, length )) ) { kernel_exec_cancel( (struct KERNEL_EXEC_STRUCTURE_TMP *) &tmp ); return STD_ERROR_memory_low; }

	// checkpoint reached: prepared task entry for new process
	tmp.level++;

	kernel -> log( (uint8_t *) "[Exec: %s]\n", name );

	//----------------------------------------------------------------------

	// prepare Paging table for new process
	if( ! (tmp.exec -> cr3 = kernel_memory_alloc_page() | KERNEL_PAGE_logical) ) { kernel_exec_cancel( (struct KERNEL_EXEC_STRUCTURE_TMP *) &tmp ); return STD_ERROR_memory_low; }

	// checkpoint reached: assigned default paging array
	tmp.level++;

	//----------------------------------------------------------------------

	// describe space under exec context stack or low memory occured
	if( ! kernel_page_alloc( (uintptr_t *) tmp.exec -> cr3, KERNEL_STACK_address, KERNEL_STACK_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_process ) ) { kernel_exec_cancel( (struct KERNEL_EXEC_STRUCTURE_TMP *) &tmp ); return STD_ERROR_memory_low; }

	// set initial startup configuration for new process
	struct KERNEL_IDT_STRUCTURE_RETURN *context = (struct KERNEL_IDT_STRUCTURE_RETURN *) (kernel_page_address( (uintptr_t *) tmp.exec -> cr3, KERNEL_STACK_pointer - STD_PAGE_byte ) + KERNEL_PAGE_logical + (STD_PAGE_byte - sizeof( struct KERNEL_IDT_STRUCTURE_RETURN )));

	// code descriptor
	context -> cs = offsetof( struct KERNEL_GDT_STRUCTURE, cs_ring3 ) | 0x03;

	// basic processor state flags
	context -> eflags = KERNEL_TASK_EFLAGS_default;

	// stack descriptor
	context -> ss = offsetof( struct KERNEL_GDT_STRUCTURE, ds_ring3 ) | 0x03;

	// the context stack top pointer
	tmp.exec -> rsp = KERNEL_STACK_pointer - sizeof( struct KERNEL_IDT_STRUCTURE_RETURN );

	// set the process entry address
	context -> rip = elf -> entry_ptr;

	//----------------------------------------------------------------------

	// length of name with arguments properties
	tmp.stack_byte = (length & ~0x0F) + 0x18;

	// assign area for process stack or low memory occured
	if( ! (tmp.stack = (uint8_t *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( tmp.stack_byte ) >> STD_SHIFT_PAGE )) ) { kernel_exec_cancel( (struct KERNEL_EXEC_STRUCTURE_TMP *) &tmp ); return STD_ERROR_memory_low; }

	// checkpoint reached: assigned area for stack
	tmp.level++;

	// stack pointer of process
	context -> rsp = KERNEL_TASK_STACK_pointer - tmp.stack_byte;

	// share to process:

	// length of args in Bytes
	uint64_t *arg_length = (uint64_t *) &tmp.stack[ MACRO_PAGE_ALIGN_UP( tmp.stack_byte ) - tmp.stack_byte ]; *arg_length = length;

	// and string itself
	for( uint64_t i = 0; i < length; i++ ) tmp.stack[ MACRO_PAGE_ALIGN_UP( tmp.stack_byte ) - tmp.stack_byte + 0x08 + i ] = name[ i ];

	// map stack space to process paging array
	if( ! kernel_page_map( (uintptr_t *) tmp.exec -> cr3, (uintptr_t) tmp.stack, MACRO_PAGE_ALIGN_DOWN( context -> rsp ), MACRO_PAGE_ALIGN_UP( tmp.stack_byte ) >> STD_SHIFT_PAGE, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | KERNEL_PAGE_FLAG_process ) ) { kernel_exec_cancel( (struct KERNEL_EXEC_STRUCTURE_TMP *) &tmp ); return STD_ERROR_memory_low; }

	// checkpoint reached: stack attached to process paging
	tmp.level++;

	// process memory usage
	tmp.exec -> page += MACRO_PAGE_ALIGN_UP( tmp.stack_byte ) >> STD_SHIFT_PAGE;

	// process stack size
	tmp.exec -> stack += MACRO_PAGE_ALIGN_UP( tmp.stack_byte ) >> STD_SHIFT_PAGE;

	//----------------------------------------------------------------------

	// ELF header properties
	struct LIB_ELF_STRUCTURE_HEADER *elf_h = (struct LIB_ELF_STRUCTURE_HEADER *) ((uint64_t) elf + elf -> headers_offset);

	// prepare the memory space for segments used by the process
	for( uint16_t i = 0; i < elf -> h_entry_count; i++ ) {
		// ignore blank entry or not loadable
 		if( elf_h[ i ].type != LIB_ELF_HEADER_TYPE_load || ! elf_h[ i ].memory_size ) continue;

		// update executable space size?
		if( tmp.exec_page < (MACRO_PAGE_ALIGN_UP( elf_h[ i ].virtual_address + elf_h[ i ].memory_size ) - KERNEL_EXEC_base_address) >> STD_SHIFT_PAGE ) tmp.exec_page = (MACRO_PAGE_ALIGN_UP( elf_h[ i ].virtual_address + elf_h[ i ].memory_size ) - KERNEL_EXEC_base_address) >> STD_SHIFT_PAGE;
	}

	// allocate calculated space
	if( ! (tmp.exec_base_address = kernel_memory_alloc( tmp.exec_page )) ) { kernel_exec_cancel( (struct KERNEL_EXEC_STRUCTURE_TMP *) &tmp ); return STD_ERROR_memory_low; }

	// checkpoint reached: assigned area for process
	tmp.level++;

	// load executable segments in place
	for( uint16_t i = 0; i < elf -> h_entry_count; i++ ) {
		// ignore blank entry or not loadable
 		if( ! elf_h[ i ].type || ! elf_h[ i ].memory_size || elf_h[ i ].type != LIB_ELF_HEADER_TYPE_load ) continue;

		// segment destination
		uint8_t *destination = (uint8_t *) ((elf_h[ i ].virtual_address - KERNEL_EXEC_base_address) + tmp.exec_base_address);

		// segment source
		uint8_t *source = (uint8_t *) ((uintptr_t) elf + elf_h[ i ].segment_offset);

		// copy segment content into place
		for( uint64_t j = 0; j < elf_h[ i ].memory_size; j++ ) destination[ j ] = source[ j ];
	}

	// map executable space to paging array
	if( ! kernel_page_map( (uintptr_t *) tmp.exec -> cr3, tmp.exec_base_address, KERNEL_EXEC_base_address, tmp.exec_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | KERNEL_PAGE_FLAG_process ) ) { kernel_exec_cancel( (struct KERNEL_EXEC_STRUCTURE_TMP *) &tmp ); return STD_ERROR_memory_low; }

	// checkpoint reached: process area attached to paging
	tmp.level++;

	// process memory usage
	tmp.exec -> page += tmp.exec_page;

	//----------------------------------------------------------------------

	// create virtual memory map for process, no less than kernels
	if( ! (tmp.exec -> memory_map = (uint32_t *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( kernel -> page_limit >> STD_SHIFT_8 ) >> STD_SHIFT_PAGE )) ) { kernel_exec_cancel( (struct KERNEL_EXEC_STRUCTURE_TMP *) &tmp ); return STD_ERROR_memory_low; }

	// checkpoint reached: assigned memory map
	tmp.level++;

	// fill in binary memory map
	for( uint64_t i = 0; i < kernel -> page_limit >> STD_SHIFT_32; i++ ) tmp.exec -> memory_map[ i ] = -1;

	// mark as occupied pages used by the executable
	kernel_memory_acquire_secured( tmp.exec, tmp.exec_page );

	//----------------------------------------------------------------------

	// connect required functions new locations / from another library
	kernel_library_link( elf, tmp.exec_base_address, FALSE );

	//----------------------------------------------------------------------

	// make a default input stream
	if( ! (tmp.exec -> stream_in = kernel_stream()) ) { kernel_exec_cancel( (struct KERNEL_EXEC_STRUCTURE_TMP *) &tmp ); return STD_ERROR_memory_low; }

	// checkpoint reached: assigned stream in/out
	tmp.level++;

	// properties of parent task
	struct KERNEL_TASK_STRUCTURE *parent = (struct KERNEL_TASK_STRUCTURE *) kernel_task_active();

	// set default output stream based on flag
	switch( stream_flow ) {
		case STD_STREAM_FLOW_out_to_parent_in: {
			// childs output to parents input
			tmp.exec -> stream_out = parent -> stream_in;

			// done
			break;
		}

		case STD_STREAM_FLOW_out_to_in: {
			// loopback stream
			tmp.exec -> stream_out = tmp.exec -> stream_in;
		
			// done
			break;
		}

		default: {
			// inherit parents output
			tmp.exec -> stream_out = parent -> stream_out;
		}
	}

	// stream used by new exec
	tmp.exec -> stream_out -> lock++;

	//----------------------------------------------------------------------

	// release workbench
	kernel_memory_release( tmp.workbench, MACRO_PAGE_ALIGN_UP( tmp.file.size_byte ) >> STD_SHIFT_PAGE );

	// map kernel space to process
	kernel_page_merge( (uint64_t *) kernel -> page_base_address, (uint64_t *) tmp.exec -> cr3 );

	// process ready to run
	tmp.exec -> flags |= KERNEL_TASK_FLAG_active | KERNEL_TASK_FLAG_init;

	// return PID of created job
	return tmp.exec -> pid;
}