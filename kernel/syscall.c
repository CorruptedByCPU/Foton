/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_syscall_file_close( uint64_t socket_id ) {
	// properties of opened file
	struct KERNEL_STRUCTURE_VFS_SOCKET *socket = (struct KERNEL_STRUCTURE_VFS_SOCKET *) &kernel -> vfs_base_address[ socket_id ];

	// close connection to file
	kernel_vfs_socket_delete( socket );
}

uint64_t kernel_syscall_file_open( uint8_t *path, uint64_t limit ) {
	// current task properties
	struct KERNEL_STRUCTURE_TASK *task = kernel_task_current();

	// default storage
	uint64_t storage = task -> storage;

	// default directory
	uint64_t directory = task -> directory;

	// change to root directory?
	if( *path == STD_ASCII_SLASH ) directory = kernel -> storage_base_address[ storage ].vfs -> root;

	// properties of file from path
	struct KERNEL_STRUCTURE_VFS_FILE file;

	// until found
	while( TRUE ) {
		// remove leading slash
		while( *path == STD_ASCII_SLASH ) path++;

		// calculate file name
		uint64_t name_limit = lib_string_word_end( path, lib_string_length( path ), STD_ASCII_SLASH );

		// search for file inside current directory
		file = kernel -> storage_base_address[ storage ].vfs -> file( (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ storage ], directory, path, name_limit );

		// not found?
		if( ! file.type ) return EMPTY;	// invalid path

		// last file from path?
		if( name_limit == lib_string_length( path ) ) break;	// yes

		// found file type of directory
		if( ! (file.type & STD_FILE_TYPE_directory) ) return EMPTY;	// path error

		// change directory
		directory = file.knot;

		// move pointer to next file name
		path += name_limit;
	}

	// retrieve information about module file
	struct KERNEL_STRUCTURE_VFS_SOCKET *socket = EMPTY;
	if( ! (socket = kernel_vfs_socket( file.knot )) ) return EMPTY;	// no enough resources

	// fill up socket properties
	socket -> storage	= storage;
	socket -> file		= file;
	socket -> pid		= kernel_task_current() -> pid;

	// return socket id
	return ((uintptr_t) socket - (uintptr_t) kernel -> vfs_base_address) / sizeof( struct KERNEL_STRUCTURE_VFS_SOCKET );
}

void kernel_syscall_file_read( uint64_t socket_id, uint8_t *target, uint64_t seek, uint64_t limit ) {
	// properties of opened file
	struct KERNEL_STRUCTURE_VFS_SOCKET *socket = (struct KERNEL_STRUCTURE_VFS_SOCKET *) &kernel -> vfs_base_address[ socket_id ];

}
