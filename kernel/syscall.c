/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

static struct KERNEL_STRUCTURE_VFS_FILE kernel_syscall_support_path( uint8_t *path, uint64_t limit ) {
	// current task properties
	struct KERNEL_STRUCTURE_TASK *current = kernel_task_current();

	// default storage
	uint64_t storage = current -> storage;

	// default directory
	uint64_t directory = current -> directory;

	// properties of file from path
	struct KERNEL_STRUCTURE_VFS_FILE file;

	// change to root directory?
	if( *path == STD_ASCII_SLASH ) {
		// yes
		directory = kernel -> storage_base_address[ storage ].vfs -> root;

		// and only root directory
		if( limit == TRUE ) {
			// obtain root directory properties
			file = kernel -> storage_base_address[ storage ].vfs -> file( (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ storage ], directory, (uint8_t *) ".", limit );

			// end of path resolving
			return file;
		}
	}

	// until found
	while( TRUE ) {
		// remove leading slash
		while( *path == STD_ASCII_SLASH ) path++;

		// calculate file name
		uint64_t name_limit = lib_string_word_end( path, lib_string_length( path ), STD_ASCII_SLASH );

		// search for file inside current directory
		file = kernel -> storage_base_address[ storage ].vfs -> file( (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ storage ], directory, path, name_limit );

		// not found?
		if( ! file.type ) return (struct KERNEL_STRUCTURE_VFS_FILE) { EMPTY };	// invalid path

		// last file from path?
		if( name_limit == lib_string_length( path ) ) break;	// yes

		// found file type of directory
		if( ! (file.type & STD_FILE_TYPE_directory) ) return (struct KERNEL_STRUCTURE_VFS_FILE) { EMPTY };	// path error

		// change directory
		directory = file.knot;

		// move pointer to next file name
		path += name_limit;
	}

	// properties of file
	return file;
}

void kernel_syscall_cd( uint8_t *path, uint64_t limit ) {
	// path outside of software environment?
	if( ((uintptr_t) path + limit) >= KERNEL_TASK_STACK_pointer ) return;	// yes, ignore

	// retrieve properties of file
	struct KERNEL_STRUCTURE_VFS_FILE file = kernel_syscall_support_path( path, limit );

	// it's not a directory?
	if( ! (file.type & STD_FILE_TYPE_directory) ) return;	// path error

	// current task properties
	struct KERNEL_STRUCTURE_TASK *current = kernel_task_current();

	// set new work directory
	current -> directory = file.knot;
}

uint64_t kernel_syscall_exec( uint8_t *name, uint64_t limit, uint8_t stream, uint8_t init ) {
	// software name outside of software environment?
	if( ((uintptr_t) name + limit) >= KERNEL_TASK_STACK_pointer ) return EMPTY;	// yes, ignore

	// return new process ID
	return kernel_exec( name, limit, stream, init );
}

void kernel_syscall_exit( void ) {
	// current task properties
	struct KERNEL_STRUCTURE_TASK *current = kernel_task_current();

	// mark task as not active and ready to close
	current -> flags &= ~STD_TASK_FLAG_active;
	current -> flags |= STD_TASK_FLAG_close;

	// release left of BS/A time
	kernel -> apic_base_address -> tic = TRUE;
	__asm__ volatile( "sti\nhlt" ); while( TRUE );
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
	// path outside of software environment?
	// if( ((uintptr_t) path + limit) >= KERNEL_TASK_STACK_pointer ) return EMPTY;	// yes, ignore

	// retrieve properties of file
	struct KERNEL_STRUCTURE_VFS_FILE file = kernel_syscall_support_path( path, limit ); if( ! file.type ) return EMPTY;	// no file

	// retrieve information about module file
	struct KERNEL_STRUCTURE_VFS_SOCKET *socket = EMPTY;
	if( ! (socket = kernel_vfs_socket( file.knot )) ) return EMPTY;	// no enough resources

	// current task properties
	struct KERNEL_STRUCTURE_TASK *current = kernel_task_current();

	// fill up socket properties
	socket -> storage	= current -> storage;
	socket -> file		= file;
	socket -> pid		= current -> pid;

	// return socket id
	return ((uintptr_t) socket - (uintptr_t) kernel -> vfs_base_address) / sizeof( struct KERNEL_STRUCTURE_VFS_SOCKET );
}	

void kernel_syscall_file_read( uint64_t socket_id, uint8_t *target, uint64_t seek, uint64_t limit ) {
	// valid socket number?
	if( socket_id >= KERNEL_VFS_limit ) return;	// no, ignore

	// target outside of software environment?
	if( ((uintptr_t) target + limit) >= KERNEL_TASK_STACK_pointer ) return;	// yes, ignore

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
	framebuffer -> pid		= 0;	// kernel, by default

	// current task properties
	struct KERNEL_STRUCTURE_TASK *current = (struct KERNEL_STRUCTURE_TASK *) kernel_task_current();

	// change framebuffer owner if possible (first come, first serve)
	if( ! __sync_val_compare_and_swap( (uint64_t *) &kernel -> framebuffer_pid, EMPTY, current -> pid ) ) {
		// acquire N continuous pages
		uintptr_t n = 0;
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

uintptr_t kernel_syscall_dir( uint8_t *path, uint64_t limit ) {
	// try to open directory
	uint64_t socket_id = kernel_syscall_file_open( path, limit );

	// unsuccessful?
	if( ! socket_id ) return EMPTY;	// yep

	// current task properties
	struct KERNEL_STRUCTURE_TASK *current = (struct KERNEL_STRUCTURE_TASK *) kernel_task_current();

	// properties of socket
	struct KERNEL_STRUCTURE_VFS_SOCKET *socket = (struct KERNEL_STRUCTURE_VFS_SOCKET *) &kernel -> vfs_base_address[ socket_id ];

	// allocate maximal required memory area for directory content
	uint64_t allocated = MACRO_PAGE_ALIGN_UP( ((struct LIB_VFS_STRUCTURE *) socket -> file.knot) -> limit + sizeof( struct STD_STRUCTURE_DIR ) ) >> STD_SHIFT_PAGE;
	struct STD_STRUCTURE_DIR *dir = (struct STD_STRUCTURE_DIR *) kernel_syscall_memory_alloc( allocated );

	// acquire content of directory from current storage
	uint64_t used = MACRO_PAGE_ALIGN_UP( kernel -> storage_base_address[ socket -> storage ].vfs -> dir( socket, dir ) ) >> STD_SHIFT_PAGE;

	// release unused area
	if( allocated != used ) kernel_syscall_memory_release( (uintptr_t) dir + (used << STD_SHIFT_PAGE), allocated - used );

	// close directory
	kernel_syscall_file_close( socket_id );

	// return directory content pointer
	return (uintptr_t) dir;
}

void kernel_syscall_ipc_send( uint64_t pid, uint8_t *data ) {
	// data outside of software environment?
	if( ((uintptr_t) data + STD_IPC_SIZE_byte) >= KERNEL_TASK_STACK_pointer ) return;	// yes, ignore

	// deny access, only one logical processor at a time
	MACRO_LOCK( kernel -> ipc_lock );

	// wait for free stack area
	while( TRUE ) {
		// scan whole IPC area
		for( uint64_t i = 0; i < KERNEL_IPC_limit; i++ ) {
			// free entry found?
			if( kernel -> ipc_base_address[ i ].ttl > kernel -> time_units ) continue;	// no

			// send to
			kernel -> ipc_base_address[ i ].target = pid;

			// sent from
			kernel -> ipc_base_address[ i ].source = kernel_task_current() -> pid;

			// load data into message
			for( uint8_t j = 0; j < STD_IPC_SIZE_byte; j++ )
				kernel -> ipc_base_address[ i ].data[ j ] = data[ j ];

			// set message timeout
			kernel -> ipc_base_address[ i ].ttl = kernel -> time_units + KERNEL_IPC_ttl;

			// unlock access to IPC area
			MACRO_UNLOCK( kernel -> ipc_lock );

			// message sent
			return;
		}
	}
}

uint64_t kernel_syscall_ipc_receive( uint8_t *data ) {
	// data outside of software environment?
	if( ((uintptr_t) data + STD_IPC_SIZE_byte) >= KERNEL_TASK_STACK_pointer ) return EMPTY;	// yes, ignore

	// scan whole IPC area
	for( uint64_t i = 0; i < KERNEL_IPC_limit; i++ ) {
		// message alive?
		if( kernel -> time_units > kernel -> ipc_base_address[ i ].ttl ) continue;	// no
	
		// message for us?
		if( kernel -> ipc_base_address[ i ].target != kernel_task_current() -> pid ) continue;	// no

		// load data into message
		for( uint8_t j = 0; j < STD_IPC_SIZE_byte; j++ )
			data[ j ] = kernel -> ipc_base_address[ i ].data[ j ];

		// mark entry as free
		kernel -> ipc_base_address[ i ].ttl = EMPTY;

		// message acquired
		return kernel -> ipc_base_address[ i ].source;
	}

	// no message for process
	return EMPTY;
}

uint8_t kernel_syscall_ipc_receive_by_pid( uint8_t *data, uint64_t pid ) {
	// data outside of software environment?
	if( ((uintptr_t) data + STD_IPC_SIZE_byte) >= KERNEL_TASK_STACK_pointer ) return FALSE;	// yes, ignore

	// scan whole IPC area
	for( uint64_t i = 0; i < KERNEL_IPC_limit; i++ ) {
		// message alive?
		if( kernel -> time_units > kernel -> ipc_base_address[ i ].ttl ) continue;	// no
	
		// message for us?
		if( kernel -> ipc_base_address[ i ].target != kernel_task_current() -> pid ) continue;	// no

		// message from specific process?
		if( kernel -> ipc_base_address[ i ].source != pid ) continue;	// no

		// load data into message
		for( uint8_t j = 0; j < STD_IPC_SIZE_byte; j++ )
			data[ j ] = kernel -> ipc_base_address[ i ].data[ j ];

		// mark entry as free
		kernel -> ipc_base_address[ i ].ttl = EMPTY;

		// message acquired
		return TRUE;
	}

	// no message for process
	return FALSE;
}

uint64_t kernel_syscall_ipc_receive_by_type( uint8_t *data, uint8_t type ) {
	// data outside of software environment?
	if( ((uintptr_t) data + STD_IPC_SIZE_byte) >= KERNEL_TASK_STACK_pointer ) return EMPTY;	// yes, ignore

	// scan whole IPC area
	for( uint64_t i = 0; i < KERNEL_IPC_limit; i++ ) {
		// message alive?
		if( kernel -> time_units > kernel -> ipc_base_address[ i ].ttl ) continue;	// no
	
		// message for us?
		if( kernel -> ipc_base_address[ i ].target != kernel_task_current() -> pid ) continue;	// no

		// message of requested type?
		struct STD_STRUCTURE_IPC_DEFAULT *ipc = (struct STD_STRUCTURE_IPC_DEFAULT *) &kernel -> ipc_base_address[ i ].data;
		if( ipc -> type != type ) continue;	// no

		// load data into message
		for( uint8_t j = 0; j < STD_IPC_SIZE_byte; j++ )
			data[ j ] = kernel -> ipc_base_address[ i ].data[ j ];

		// mark entry as free
		kernel -> ipc_base_address[ i ].ttl = EMPTY;

		// message acquired
		return kernel -> ipc_base_address[ i ].source;
	}

	// no message for process
	return EMPTY;
}

uint16_t kernel_syscall_keyboard( void ) {
	// current task properties
	struct KERNEL_STRUCTURE_TASK *current = kernel_task_current();

	// am I framebuffer manager?
	if( current -> pid != kernel -> framebuffer_pid ) return EMPTY;	// no, return no key

	// get first key code from buffer
	uint16_t key = kernel -> device_keyboard[ 0 ];

	// move all characters inside buffer, forward one position
	for( uint8_t i = 0; i < 7; i++ ) kernel -> device_keyboard[ i ] = kernel -> device_keyboard[ i + 1 ];

	// drain last key
	kernel -> device_keyboard[ 7 ] = EMPTY;

	// return key code
	return key;
}

void kernel_syscall_log( uint8_t *string, uint64_t length ) {
	// if string pointer is above software environment memory area
	if( (uintptr_t) string > KERNEL_STACK_pointer || ((uintptr_t) string + length) > KERNEL_STACK_pointer ) return;	// do not allow it

	// show content of string
	for( uint64_t i = 0; i < length; i++ ) driver_serial_char( string[ i ] );
}

uintptr_t kernel_syscall_memory_alloc( uint64_t n ) {
	// current task properties
	struct KERNEL_STRUCTURE_TASK *current = (struct KERNEL_STRUCTURE_TASK *) kernel_task_current();

	// id of first logical page
	uintptr_t p = 0;

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

uintptr_t kernel_syscall_memory_move( uint64_t pid, uintptr_t source, uint64_t n ) {
	// always aligned
	source &= STD_PAGE_mask;

	// memory area inside software environment?
	if( source + (n << STD_SHIFT_PAGE) >= KERNEL_TASK_STACK_pointer ) return EMPTY;	// no, ignore

	// properties of target task
	struct KERNEL_STRUCTURE_TASK *target = (struct KERNEL_STRUCTURE_TASK *) kernel_task_by_id( pid );

	// process not found?
	if( ! target ) return EMPTY;	// yep, ignore

	// acquire area from target task
	uintptr_t memory = kernel_memory_acquire( target -> memory, n, KERNEL_MEMORY_HIGH, kernel -> page_limit ) << STD_SHIFT_PAGE;

	// such large memory not available?
	if( ! memory ) return EMPTY;	// abort mission

	// connect memory area of parent process with child
	if( ! kernel_page_clang( (uint64_t *) target -> cr3, source, memory, n, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_user | (KERNEL_PAGE_TYPE_EXTERNAL << KERNEL_PAGE_TYPE_offset) ) ) {
		// memory not moved
		memory = EMPTY;

		// do something nasty

		// discipline!
	}

	// release source memory
	kernel_syscall_memory_release( source, n );

	// return address of first page in the collection
	return memory;
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

void kernel_syacall_memory_purge( void ) {
	// release all obsolete pages
	kernel_page_deconstruct( kernel_task_current() -> cr3, KERNEL_PAGE_TYPE_EXTERNAL );
}

uintptr_t kernel_syscall_memory_share( uint64_t pid, uintptr_t source, uint64_t n, uint8_t write ) {
	// memory area inside software environment?
	if( source + (n << STD_SHIFT_PAGE) >= KERNEL_TASK_STACK_pointer ) return EMPTY;	// no, ignore

	// properties of target task
	struct KERNEL_STRUCTURE_TASK *target = (struct KERNEL_STRUCTURE_TASK *) kernel_task_by_id( pid );

	// process not found?
	if( ! target ) return EMPTY;	// yep, ignore

	// acquire area from target task
	uintptr_t memory = kernel_memory_acquire( target -> memory, n, KERNEL_MEMORY_HIGH, kernel -> page_limit ) << STD_SHIFT_PAGE;

	// such large memory not available?
	if( ! memory ) return EMPTY;	// abort mission

	// default flags
	uint16_t flags = KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_user | (KERNEL_PAGE_TYPE_SHARED << KERNEL_PAGE_TYPE_offset);
	if( write ) flags |= KERNEL_PAGE_FLAG_write;	// allow modification by target

	// connect memory area of parent process with child
	if( ! kernel_page_clang( (uint64_t *) target -> cr3, source, memory, n, flags ) ) {
		// do something nasty

		// discipline!
	}

	// return address of first page in the collection
	return memory;
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
	for( uint64_t i = 0; i < kernel -> task_limit; i++ ) {
		// entry occupied?
		if( ! kernel -> task_base_address[ i ].flags ) continue;	// no

		// found?
		if( kernel -> task_base_address[ i ].pid == pid ) return TRUE;
	}

	// process not found
	return FALSE;
}

uintptr_t kernel_syscall_storage( void ) {
	// amount of entries to pass
	uint64_t limit = TRUE;	// last entry, always empty

	// count entries
	for( uint64_t i = 0; i < KERNEL_STORAGE_limit; i++ ) if( kernel -> storage_base_address[ i ].flags & KERNEL_STORAGE_FLAGS_active ) limit++;

	// alloc enough memory for all entries
	struct STD_STRUCTURE_STORAGE *storage = (struct STD_STRUCTURE_STORAGE *) kernel_syscall_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct STD_STRUCTURE_STORAGE ) * limit ) >> STD_SHIFT_PAGE );

	// copy essential information about every storage
	uint64_t entry = 0; for( uint64_t i = 0; i < KERNEL_STORAGE_limit && limit != entry; i++ ) {
		// identificator of storage
		storage[ entry ].id = i;

		// type of storage
		storage[ entry ].type = kernel -> storage_base_address[ i ].type;

		// name
		for( uint8_t n = 0; n < kernel -> storage_base_address[ i ].length; n++ ) storage[ entry ].name[ storage[ entry ].name_limit++ ] = kernel -> storage_base_address[ i ].name[ n ]; storage[ entry ].name[ storage[ entry ].name_limit ] = STD_ASCII_TERMINATOR;

		// next antry position
		entry++;
	}

	// share structure with process
	return (uintptr_t) storage;
}

uint64_t kernel_syscall_thread( uintptr_t function, uint8_t *name, uint64_t length ) {
	// name or function outside of software environment?
	if( (function + sizeof( uintptr_t )) >= KERNEL_TASK_STACK_pointer || ((uintptr_t) name + length) >= KERNEL_TASK_STACK_pointer ) return EMPTY;	// no, ignore

	// create a new thread
	struct KERNEL_STRUCTURE_TASK *thread = (struct KERNEL_STRUCTURE_TASK *) kernel_task_add( name, length );

	// couldn't create new task?
	if( ! thread ) return EMPTY;	// that's lame :)

	// mark task type
	thread -> page = KERNEL_TASK_TYPE_THREAD;

	//----------------------------------------------------------------------

	// create Paging table
	uint64_t cr3 = EMPTY;
	if( ! (cr3 = kernel_memory_alloc_page()) ) { thread -> flags = EMPTY; return EMPTY; }

	// set paging address
	thread -> cr3 = (uint64_t *) (cr3 | KERNEL_MEMORY_mirror);

	//----------------------------------------------------------------------

	// describe area under thread context stack
	if( ! kernel_page_alloc( (uint64_t *) thread -> cr3, KERNEL_STACK_address, KERNEL_STACK_LIMIT_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | (thread -> type << KERNEL_PAGE_TYPE_offset) ) ) { kernel_page_deconstruct( thread -> cr3, thread -> type ); thread -> flags = EMPTY; return EMPTY; }

	// set initial startup configuration for new process
	struct KERNEL_STRUCTURE_IDT_RETURN *context = (struct KERNEL_STRUCTURE_IDT_RETURN *) (kernel_page_address( (uint64_t *) thread -> cr3, KERNEL_STACK_pointer - STD_PAGE_byte ) + KERNEL_MEMORY_mirror + (STD_PAGE_byte - sizeof( struct KERNEL_STRUCTURE_IDT_RETURN )));

	// set thread entry address
	context -> rip = function;

	// code descriptor
	context -> cs = offsetof( struct KERNEL_STRUCTURE_GDT, cs_ring3 ) | 0x03;

	// basic processor state flags
	context -> eflags = KERNEL_TASK_EFLAGS_default;

	// stack pointer of process
	context -> rsp = KERNEL_TASK_STACK_pointer - 0x18;	// no args

	// stack descriptor
	context -> ss = offsetof( struct KERNEL_STRUCTURE_GDT, ds_ring3 ) | 0x03;

	//----------------------------------------------------------------------

	// map stack space to thread paging array
	if( ! kernel_page_alloc( (uint64_t *) thread -> cr3, KERNEL_TASK_STACK_pointer - KERNEL_TASK_STACK_limit, KERNEL_TASK_STACK_limit >> STD_SHIFT_PAGE, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | (thread -> type << KERNEL_PAGE_TYPE_offset) ) ) { kernel_page_deconstruct( thread -> cr3, thread -> type ); thread -> flags = EMPTY; return EMPTY; }

	// pages used for stack
	thread -> stack_page++;

	// context stack top pointer
	thread -> rsp = KERNEL_STACK_pointer - sizeof( struct KERNEL_STRUCTURE_IDT_RETURN );

	//----------------------------------------------------------------------

	// aquire parent task properties
	struct KERNEL_STRUCTURE_TASK *current = kernel_task_current();

	// threads use same memory map as parent
	thread -> memory = current -> memory;

	//----------------------------------------------------------------------

	// map parent space to thread
	kernel_page_merge( (uint64_t *) current -> cr3, (uint64_t *) thread -> cr3 );

	// thread ready to run
	thread -> flags |= STD_TASK_FLAG_active | STD_TASK_FLAG_thread | STD_TASK_FLAG_init;

	// return process ID of new thread
	return thread -> pid;
}

uint64_t kernel_syscall_time( void ) {
	// current date and time
	return driver_rtc_time();
}

uint64_t kernel_syscall_uptime( void ) {
	// units of time
	return kernel -> time_units;
}
