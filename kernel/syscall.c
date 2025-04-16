/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_syscall_exit( void ) {
	// current task properties
	struct KERNEL_STRUCTURE_TASK *current = kernel_task_current();

	// mark task as not active and ready to close
	current -> flags &= ~STD_TASK_FLAG_active;
	current -> flags |= STD_TASK_FLAG_close;

	// release left of BS/A time
	kernel -> apic_base_address -> tic = TRUE;
	__asm__ volatile( "hlt" ); while( TRUE );
}

void kernel_syscall_file( struct STD_STRUCTURE_FILE *file ) {
	// incorrect socket number?
	if( file -> socket >= KERNEL_VFS_limit ) return;	// yes, ignore

	// properties of socket
	struct KERNEL_STRUCTURE_VFS_SOCKET *socket = (struct KERNEL_STRUCTURE_VFS_SOCKET *) &kernel -> vfs_base_address[ file -> socket ];

	// share important values

	// type of file
	file -> type = socket -> file.type;

	// file size in Bytes
	file -> byte = socket -> file.limit;
}

void kernel_syscall_file_close( uint64_t socket_id ) {
	// valid socket number?
	if( socket_id >= KERNEL_VFS_limit ) return;	// no, ignore

	// close connection to file
	kernel_vfs_socket_delete( (struct KERNEL_STRUCTURE_VFS_SOCKET *) &kernel -> vfs_base_address[ socket_id ] );
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
	// valid socket number?
	if( socket_id >= KERNEL_VFS_limit ) return;	// no, ignore

	// current task properties
	struct KERNEL_STRUCTURE_TASK *current = (struct KERNEL_STRUCTURE_TASK *) kernel_task_current();

	// properties of socket
	struct KERNEL_STRUCTURE_VFS_SOCKET *socket = (struct KERNEL_STRUCTURE_VFS_SOCKET *) &kernel -> vfs_base_address[ socket_id ];

	// socket belongs to process?
	if( current -> pid != socket -> pid ) return;	// no, ignore

	// pass content of file
	((struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ socket -> storage ]) -> vfs -> file_read( socket, target, seek, limit );
}

void kernel_syscall_framebuffer( struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER *framebuffer ) {
	// return information about existing framebuffer
	framebuffer -> base_address	= kernel -> framebuffer_base_address;
	framebuffer -> width_pixel	= kernel -> framebuffer_width_pixel;
	framebuffer -> height_pixel	= kernel -> framebuffer_height_pixel;
	framebuffer -> pitch_byte	= kernel -> framebuffer_pitch_byte;
	framebuffer -> pid		= INIT;	// kernel, by default

	// current task properties
	struct KERNEL_STRUCTURE_TASK *current = (struct KERNEL_STRUCTURE_TASK *) kernel_task_current();

	// change framebuffer owner if possible (first come, first serve)
	if( ! __sync_val_compare_and_swap( (uint64_t *) &kernel -> framebuffer_pid, EMPTY, current -> pid ) ) {
		// acquire N continuous pages
		uintptr_t n = INIT;
		if( (n = kernel_memory_acquire( current -> memory, MACRO_PAGE_ALIGN_UP( kernel -> framebuffer_pitch_byte * kernel -> framebuffer_height_pixel ) >> STD_SHIFT_PAGE, EMPTY, kernel -> page_limit )) ) {
			// alloc memory inside current task
			if( ! kernel_page_map( (uint64_t *) current -> cr3, (uintptr_t) kernel -> framebuffer_base_address & ~KERNEL_MEMORY_mirror, (uintptr_t) (n << STD_SHIFT_PAGE), MACRO_PAGE_ALIGN_UP( kernel -> framebuffer_pitch_byte * kernel -> framebuffer_height_pixel ) >> STD_SHIFT_PAGE, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | (KERNEL_PAGE_TYPE_SHARED << KERNEL_PAGE_TYPE_offset) ) ) {
				// release acquired pages
				kernel_memory_dispose( current -> memory, n, MACRO_PAGE_ALIGN_UP( kernel -> framebuffer_pitch_byte * kernel -> framebuffer_height_pixel ) >> STD_SHIFT_PAGE );

				// do not share information about framebuffer location
				framebuffer -> base_address = EMPTY;

				// done
				return;
			}

			// shared pages
			kernel -> page_shared += MACRO_PAGE_ALIGN_UP( kernel -> framebuffer_pitch_byte * kernel -> framebuffer_height_pixel ) >> STD_SHIFT_PAGE;

			// return the address of the first page in the collection
			framebuffer -> base_address = (uint32_t *) (n << STD_SHIFT_PAGE);
		}
	}

	// return information about framebuffer owner
	framebuffer -> pid = kernel -> framebuffer_pid;
}

uintptr_t kernel_syscall_memory_alloc( uint64_t n ) {
	// current task properties
	struct KERNEL_STRUCTURE_TASK *current = (struct KERNEL_STRUCTURE_TASK *) kernel_task_current();

	// id of first logical page
	uintptr_t p = INIT;

	// acquired N continuous pages?
	if( ! (p = kernel_memory_acquire( current -> memory, n, KERNEL_MEMORY_HIGH, kernel -> page_limit )) ) return EMPTY;	// no

	// allocate acquired area
	if( ! kernel_page_alloc( (uint64_t *) current -> cr3, p << STD_SHIFT_PAGE, n, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | (current -> type << KERNEL_PAGE_TYPE_offset) ) ) {
		// release acquired pages
		kernel_memory_dispose( current -> memory, p, n );

		// conflict
		return EMPTY;
	}

	// reload paging structure
	kernel_page_flush();

	// process memory usage
	current -> page += n;

	// return the address of the first page in the collection
	return p << STD_SHIFT_PAGE;
}

void kernel_syscall_memory_release( uintptr_t address, uint64_t n ) {
	// memory area inside software environment?
	if( address + (n << STD_SHIFT_PAGE) >= KERNEL_TASK_STACK_pointer ) return;	// no, ignore

	// current task properties
	struct KERNEL_STRUCTURE_TASK *current = (struct KERNEL_STRUCTURE_TASK *) kernel_task_current();

	// remove page from paging structure
	if( ! kernel_page_release( (uint64_t *) current -> cr3, address, n ) ) return;	// no asssignment

	// reload paging structure
	kernel_page_flush();

	// release page in binary memory map of process
	kernel_memory_dispose( current -> memory, address >> STD_SHIFT_PAGE, n );

	// process memory usage
	current -> page -= n;
}

uint64_t kernel_syscall_microtime( void ) {
	// elapsed time units
	return kernel -> time_units;
}

void kernel_syscall_mouse( struct STD_STRUCTURE_MOUSE_SYSCALL *mouse ) {
	// return information about mouse device
	mouse -> status	= kernel -> device_mouse_status;
	mouse -> x	= kernel -> device_mouse_x;
	mouse -> y	= kernel -> device_mouse_y;
	mouse -> z	= kernel -> device_mouse_z;
}

uint64_t kernel_syscall_pid( void ) {
	// task pid
	return kernel_task_current() -> pid;
}

uint8_t kernel_syscall_pid_exist( uint64_t pid ) {
	// find an entry with selected ID
	for( uint64_t i = INIT; i < kernel -> task_limit; i++ ) {
		// entry occupied?
		if( ! kernel -> task_base_address[ i ].flags ) continue;	// no

		// found?
		if( kernel -> task_base_address[ i ].pid == pid ) return TRUE;
	}

	// process not found
	return FALSE;
}

uint64_t kernel_syscall_uptime( void ) {
	// units of time
	return kernel -> time_units;
}
