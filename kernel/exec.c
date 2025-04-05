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

	// debug
	return EMPTY;
}
