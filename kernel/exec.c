/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

static void kernel_exec_cancel( struct KERNEL_EXEC_STRUCTURE_INIT *exec ) {
	// undo performed operations depending on cavity
	switch( exec -> level ) {
		case 8: {
			// cannot foresee an error at this level and above
		}
		case 7: {
			// release memory map from task entry
			kernel_memory_release( (uintptr_t) exec -> task -> memory_map, MACRO_PAGE_ALIGN_UP( kernel -> page_limit >> STD_SHIFT_8 ) >> STD_SHIFT_PAGE );
		}
		case 6: {
			// release process area
			kernel_memory_release( (uintptr_t) exec -> base_address, exec -> page );
		}
		case 5: {
			// release stack area
			kernel_memory_release( (uintptr_t) exec -> stack, MACRO_PAGE_ALIGN_UP( exec -> stack_byte ) >> STD_SHIFT_PAGE );
		}
		case 4: {
			// release paging structure
			kernel_page_deconstruct( (uintptr_t *) exec -> task -> cr3 );
		}
		case 3: {
			// release task entry
			exec -> task -> flags = EMPTY;
		}
		case 2: {
			// release workbench area
			kernel_memory_release( exec -> workbench_address, MACRO_PAGE_ALIGN_UP( exec -> properties.byte ) >> STD_SHIFT_PAGE );
		}
		case 1: {
			// close file
			kernel_vfs_file_close( exec -> socket );
		}
	}
}

int64_t kernel_exec( uint8_t *name, uint64_t length, uint8_t stream_flow ) {
	// prepare temporary execution area
	struct KERNEL_EXEC_STRUCTURE_INIT exec = { EMPTY };

	// file name length allowed?
	if( length > (LIB_VFS_NAME_limit - length) ) return STD_ERROR_syntax_error;	// no

	// default location of executables
	uint64_t path_length = 0;
	uint8_t path_default[ 12 ] = "/system/bin/";

	// length of exec name
	uint64_t exec_length = lib_string_word( name, length );

	// set file path name
	uint8_t path[ 12 + LIB_VFS_NAME_limit ];
	for( uint64_t i = 0; i < 12; i++ ) path[ path_length++ ] = path_default[ i ];
	for( uint64_t i = 0; i < exec_length; i++ ) path[ path_length++ ] = name[ i ];

	// retrieve information about executable file
	exec.socket = (struct KERNEL_VFS_STRUCTURE *) kernel_vfs_file_open( path, path_length );

	// if executable does not exist
	if( ! exec.socket ) return STD_ERROR_file_not_found;

	// checkpoint reached: file socket opened
	exec.level++;

	// gather information about file
	kernel_vfs_file_properties( exec.socket, (struct KERNEL_VFS_STRUCTURE_PROPERTIES *) &exec.properties );

	// assign area for workbench
	if( ! (exec.workbench_address = kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( exec.properties.byte ) >> STD_SHIFT_PAGE )) ) { kernel_exec_cancel( (struct KERNEL_EXEC_STRUCTURE_INIT *) &exec ); return STD_ERROR_memory_low; };

	// checkpoint reached: assigned area for temporary file
	exec.level++;

	// load executable into workbench space
	kernel_vfs_file_read( exec.socket, (uint8_t *) exec.workbench_address, EMPTY, exec.properties.byte );

	// close file
	kernel_vfs_file_close( exec.socket );

	//----------------------------------------------------------------------

	// file contains proper ELF header?
	if( ! lib_elf_identify( exec.workbench_address ) ) { kernel_exec_cancel( (struct KERNEL_EXEC_STRUCTURE_INIT *) &exec ); return STD_ERROR_file_unknown; }	// no

	// ELF structure properties
	struct LIB_ELF_STRUCTURE *elf = (struct LIB_ELF_STRUCTURE *) exec.workbench_address;

	// it's an executable file?
	if( elf -> type != LIB_ELF_TYPE_executable ) { kernel_exec_cancel( (struct KERNEL_EXEC_STRUCTURE_INIT *) &exec ); return STD_ERROR_file_not_executable; }	// no

	// load libraries required by file
	int64_t result;
	if( (result = kernel_library( elf ) ) ) {
		// cancel execution
		kernel_exec_cancel( (struct KERNEL_EXEC_STRUCTURE_INIT *) &exec );
		
		// return last error
		return result;
	}

	//----------------------------------------------------------------------

	// create a new job in task queue
	if( ! (exec.task = kernel_task_add( name, length )) ) { kernel_exec_cancel( (struct KERNEL_EXEC_STRUCTURE_INIT *) &exec ); return STD_ERROR_task_limit; }

	// checkpoint reached: prepared task entry for new process
	exec.level++;

	//----------------------------------------------------------------------

	// prepare Paging table for new process
	if( ! (exec.task -> cr3 = kernel_memory_alloc_page() | KERNEL_PAGE_logical) ) { kernel_exec_cancel( (struct KERNEL_EXEC_STRUCTURE_INIT *) &exec ); return STD_ERROR_memory_low; }

	// checkpoint reached: assigned default paging array
	exec.level++;

	//----------------------------------------------------------------------

	// describe space under exec context stack or low memory occured
	if( ! kernel_page_alloc( (uintptr_t *) exec.task -> cr3, KERNEL_STACK_address, KERNEL_STACK_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_process ) ) { kernel_exec_cancel( (struct KERNEL_EXEC_STRUCTURE_INIT *) &exec ); return STD_ERROR_memory_low; }

	// set initial startup configuration for new process
	struct KERNEL_IDT_STRUCTURE_RETURN *context = (struct KERNEL_IDT_STRUCTURE_RETURN *) (kernel_page_address( (uintptr_t *) exec.task -> cr3, KERNEL_STACK_pointer - STD_PAGE_byte ) + KERNEL_PAGE_logical + (STD_PAGE_byte - sizeof( struct KERNEL_IDT_STRUCTURE_RETURN )));

	// code descriptor
	context -> cs = offsetof( struct KERNEL_GDT_STRUCTURE, cs_ring3 ) | 0x03;

	// basic processor state flags
	context -> eflags = KERNEL_TASK_EFLAGS_default;

	// stack descriptor
	context -> ss = offsetof( struct KERNEL_GDT_STRUCTURE, ds_ring3 ) | 0x03;

	// the context stack top pointer
	exec.task -> rsp = KERNEL_STACK_pointer - sizeof( struct KERNEL_IDT_STRUCTURE_RETURN );

	// set the process entry address
	context -> rip = elf -> entry_ptr;

	//----------------------------------------------------------------------

	// length of name with arguments properties
	exec.stack_byte = (length & ~0x0F) + 0x18;

	// assign area for process stack or low memory occured
	if( ! (exec.stack = (uint8_t *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( exec.stack_byte ) >> STD_SHIFT_PAGE )) ) { kernel_exec_cancel( (struct KERNEL_EXEC_STRUCTURE_INIT *) &exec ); return STD_ERROR_memory_low; }

	// checkpoint reached: assigned area for stack
	exec.level++;

	// stack pointer of process
	context -> rsp = KERNEL_TASK_STACK_pointer - exec.stack_byte;

	// share to process:

	// length of args in Bytes
	uint64_t *arg_length = (uint64_t *) &exec.stack[ MACRO_PAGE_ALIGN_UP( exec.stack_byte ) - exec.stack_byte ]; *arg_length = length;

	// and string itself
	for( uint64_t i = 0; i < length; i++ ) exec.stack[ MACRO_PAGE_ALIGN_UP( exec.stack_byte ) - exec.stack_byte + 0x08 + i ] = name[ i ];

	// map stack space to process paging array
	if( ! kernel_page_map( (uintptr_t *) exec.task -> cr3, (uintptr_t) exec.stack, MACRO_PAGE_ALIGN_DOWN( context -> rsp ), MACRO_PAGE_ALIGN_UP( exec.stack_byte ) >> STD_SHIFT_PAGE, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | KERNEL_PAGE_FLAG_process ) ) { kernel_exec_cancel( (struct KERNEL_EXEC_STRUCTURE_INIT *) &exec ); return STD_ERROR_memory_low; }

	// process stack size
	exec.task -> stack += MACRO_PAGE_ALIGN_UP( exec.stack_byte ) >> STD_SHIFT_PAGE;

	//----------------------------------------------------------------------

	// ELF header properties
	struct LIB_ELF_STRUCTURE_HEADER *elf_h = (struct LIB_ELF_STRUCTURE_HEADER *) ((uint64_t) elf + elf -> headers_offset);

	// prepare the memory space for segments used by the process
	for( uint16_t i = 0; i < elf -> h_entry_count; i++ ) {
		// ignore blank entry or not loadable
 		if( elf_h[ i ].type != LIB_ELF_HEADER_TYPE_load || ! elf_h[ i ].memory_size ) continue;

		// update executable space size?
		if( exec.page < (MACRO_PAGE_ALIGN_UP( elf_h[ i ].virtual_address + elf_h[ i ].memory_size ) - KERNEL_EXEC_base_address) >> STD_SHIFT_PAGE ) exec.page = (MACRO_PAGE_ALIGN_UP( elf_h[ i ].virtual_address + elf_h[ i ].memory_size ) - KERNEL_EXEC_base_address) >> STD_SHIFT_PAGE;
	}

	// allocate calculated space
	if( ! (exec.base_address = kernel_memory_alloc( exec.page )) ) { kernel_exec_cancel( (struct KERNEL_EXEC_STRUCTURE_INIT *) &exec ); return STD_ERROR_memory_low; }

	// checkpoint reached: assigned area for process
	exec.level++;

	// load executable segments in place
	for( uint16_t i = 0; i < elf -> h_entry_count; i++ ) {
		// ignore blank entry or not loadable
 		if( ! elf_h[ i ].type || ! elf_h[ i ].segment_size || ! elf_h[ i ].memory_size || elf_h[ i ].type != LIB_ELF_HEADER_TYPE_load ) continue;

		// segment destination
		uint8_t *destination = (uint8_t *) ((elf_h[ i ].virtual_address - KERNEL_EXEC_base_address) + exec.base_address);

		// segment source
		uint8_t *source = (uint8_t *) ((uintptr_t) elf + elf_h[ i ].segment_offset);

		// copy segment content into place
		for( uint64_t j = 0; j < elf_h[ i ].memory_size; j++ ) destination[ j ] = source[ j ];
	}

	// map executable space to paging array
	if( ! kernel_page_map( (uintptr_t *) exec.task -> cr3, exec.base_address, KERNEL_EXEC_base_address, exec.page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | KERNEL_PAGE_FLAG_process ) ) { kernel_exec_cancel( (struct KERNEL_EXEC_STRUCTURE_INIT *) &exec ); return STD_ERROR_memory_low; }

	// process memory usage
	exec.task -> page += exec.page;

	//----------------------------------------------------------------------

	// create virtual memory map for process, no less than kernels
	if( ! (exec.task -> memory_map = (uint32_t *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( kernel -> page_limit >> STD_SHIFT_8 ) >> STD_SHIFT_PAGE )) ) { kernel_exec_cancel( (struct KERNEL_EXEC_STRUCTURE_INIT *) &exec ); return STD_ERROR_memory_low; }

	// checkpoint reached: assigned memory map
	exec.level++;

	// fill in binary memory map
	for( uint64_t i = (MACRO_PAGE_ALIGN_DOWN( KERNEL_EXEC_base_address ) >> STD_SHIFT_PAGE) >> STD_SHIFT_32; i < kernel -> page_limit >> STD_SHIFT_32; i++ ) exec.task -> memory_map[ i ] = -1;

	// mark as occupied pages used by the executable
	kernel_memory_acquire( exec.task -> memory_map, exec.page );

	// define memory semaphore location
	uint8_t *semaphore = (uint8_t *) exec.task -> memory_map + MACRO_PAGE_ALIGN_UP( kernel -> page_limit >> STD_SHIFT_8 ) - STD_SIZE_BYTE_byte;

	// unlock access to binary memory map
	MACRO_UNLOCK( *semaphore );

	//----------------------------------------------------------------------

	// connect required functions new locations / from another library
	kernel_library_link( elf, exec.base_address, FALSE );

	//----------------------------------------------------------------------

	// make a default input stream
	if( ! (exec.task -> stream_in = kernel_stream()) ) { kernel_exec_cancel( (struct KERNEL_EXEC_STRUCTURE_INIT *) &exec ); return STD_ERROR_memory_low; }

	// checkpoint reached: assigned stream in/out
	exec.level++;

	// properties of parent task
	struct KERNEL_TASK_STRUCTURE *parent = (struct KERNEL_TASK_STRUCTURE *) kernel_task_active();

	// set default output stream based on flag
	switch( stream_flow ) {
		case STD_STREAM_FLOW_out_to_parent_in: {
			// childs output to parents input
			exec.task -> stream_out = parent -> stream_in;

			// done
			break;
		}

		case STD_STREAM_FLOW_out_to_in: {
			// loopback stream
			exec.task -> stream_out = exec.task -> stream_in;
		
			// done
			break;
		}

		default: {
			// inherit parents output
			exec.task -> stream_out = parent -> stream_out;
		}
	}

	// stream used by new exec
	exec.task -> stream_out -> lock++;

	//----------------------------------------------------------------------

	// map kernel space to process
	kernel_page_merge( (uint64_t *) kernel -> page_base_address, (uint64_t *) exec.task -> cr3 );

	// release workbench
	kernel_memory_release( exec.workbench_address, MACRO_PAGE_ALIGN_UP( exec.properties.byte ) >> STD_SHIFT_PAGE );

	//----------------------------------------------------------------------

	// exec inherites root directory of parent
	exec.task -> directory = parent -> directory;

	//----------------------------------------------------------------------

	// process ready to run
	exec.task -> flags |= KERNEL_TASK_FLAG_active | KERNEL_TASK_FLAG_init;

	// return PID of created job
	return exec.task -> pid;
}