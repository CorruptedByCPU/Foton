/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_syscall_exit( void ) {
	// current task properties
	struct KERNEL_STRUCTURE_TASK *task = kernel_task_active();

	// mark task as not active and ready to close
	task -> flags &= ~STD_TASK_FLAG_active;
	task -> flags |= STD_TASK_FLAG_close;

	// release left BS/A time
	__asm__ volatile( "int $0x20" );
}

void kernel_syscall_framebuffer( struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER *framebuffer ) {
	// return information about existing framebuffer
	framebuffer -> base_address	= kernel -> framebuffer_base_address;
	framebuffer -> width_pixel	= kernel -> framebuffer_width_pixel;
	framebuffer -> height_pixel	= kernel -> framebuffer_height_pixel;
	framebuffer -> pitch_byte	= kernel -> framebuffer_pitch_byte;

	// change framebuffer owner if possible
	if( ! __sync_val_compare_and_swap( &kernel -> framebuffer_pid, EMPTY, kernel_task_pid() ) ) {
		// current task properties
		struct KERNEL_STRUCTURE_TASK *task = (struct KERNEL_STRUCTURE_TASK *) kernel_task_active();

		// acquire N continuous pages
		uintptr_t allocated = EMPTY;
		if( (allocated = kernel_memory_acquire( task -> memory_map, MACRO_PAGE_ALIGN_UP( kernel -> framebuffer_pitch_byte * kernel -> framebuffer_height_pixel ) >> STD_SHIFT_PAGE, KERNEL_MEMORY_HIGH, kernel -> page_limit )) ) {
			// map memory area to process
			kernel_page_map( (uint64_t *) task -> cr3, (uintptr_t) kernel -> framebuffer_base_address & ~KERNEL_PAGE_mirror, (uintptr_t) (allocated << STD_SHIFT_PAGE), MACRO_PAGE_ALIGN_UP( kernel -> framebuffer_pitch_byte * kernel -> framebuffer_height_pixel ) >> STD_SHIFT_PAGE, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | (KERNEL_PAGE_TYPE_SHARED << KERNEL_PAGE_TYPE_offset) );

			// shared pages
			kernel -> page_shared += MACRO_PAGE_ALIGN_UP( kernel -> framebuffer_pitch_byte * kernel -> framebuffer_height_pixel ) >> STD_SHIFT_PAGE;

			// return the address of the first page in the collection
			framebuffer -> base_address = (uint32_t *) (allocated << STD_SHIFT_PAGE);
		}
	}

	// return information about framebuffer owner
	framebuffer -> pid = kernel -> framebuffer_pid;
}

uintptr_t kernel_syscall_memory_alloc( uint64_t page ) {
	// current task properties
	struct KERNEL_STRUCTURE_TASK *task = kernel_task_active();

	// acquire N continuous pages
	uintptr_t allocated = EMPTY;
	if( (allocated = kernel_memory_acquire( task -> memory_map, page, KERNEL_MEMORY_HIGH, kernel -> page_limit )) ) {
		// allocate space inside process paging area
		if( ! kernel_page_alloc( (uint64_t *) task -> cr3, allocated << STD_SHIFT_PAGE, page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | (task -> page_type << KERNEL_PAGE_TYPE_offset) ) ) {
			// debug
			kernel_log( (uint8_t *) "%s: memory allocation conflict!\n" );

			// no asssignment
			return EMPTY;
		}

		// reload paging structure
		kernel_page_flush();

		// process memory usage
		task -> page += page;

		// return the address of the first page in the collection
		return allocated << STD_SHIFT_PAGE;
	}

	// debug
	kernel_log( (uint8_t *) "%s: low memory.\n", task -> name );

	// no free space
	return EMPTY;
}

void kernel_syscall_memory_release( uintptr_t target, uint64_t page ) {
	// current task properties
	struct KERNEL_STRUCTURE_TASK *task = kernel_task_active();

	// remove page from paging structure
	if( ! kernel_page_release( (uint64_t *) task -> cr3, target, page ) ) {
		// debug
		kernel_log( (uint8_t *) "%s: memory release voidness!\n" );

		// no asssignment
		return;
	}

	// reload paging structure
	kernel_page_flush();

	// release page in binary memory map of process
	kernel_memory_dispose( task -> memory_map, target >> STD_SHIFT_PAGE, page );

	// process memory usage
	task -> page -= page;
}

uint64_t kernel_syscall_uptime( void ) {
	// return uptime
	return kernel -> time_rtc;
}

void kernel_syscall_log( uint8_t *string, uint64_t length ) {
	// if string pointer is above software environment memory area
	if( (uintptr_t) string > KERNEL_PAGE_mirror || ((uintptr_t) string + length) > KERNEL_PAGE_mirror ) return;	// do not allow it

	// show content of string
	for( uint64_t i = 0; i < length; i++ ) kernel_log( (uint8_t *) "%c", (uint64_t) string[ i ] );
}

int64_t kernel_syscall_thread( uintptr_t function, uint8_t *name, uint64_t length ) {
	// create a new thread in task queue
	struct KERNEL_STRUCTURE_TASK *thread = kernel_task_add( name, length );

	//----------------------------------------------------------------------

	// prepare Paging table for new process
	thread -> cr3 = kernel_memory_alloc_page() | KERNEL_PAGE_mirror;

	// all allocated pages, mark as type of THREAD
	thread -> page_type = KERNEL_PAGE_TYPE_THREAD;

	//----------------------------------------------------------------------

	// describe area under thread context stack
	kernel_page_alloc( (uint64_t *) thread -> cr3, KERNEL_STACK_address, KERNEL_STACK_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | (thread -> page_type << KERNEL_PAGE_TYPE_offset) );

	// set initial startup configuration for new process
	struct KERNEL_STRUCTURE_IDT_RETURN *context = (struct KERNEL_STRUCTURE_IDT_RETURN *) (kernel_page_address( (uint64_t *) thread -> cr3, KERNEL_STACK_pointer - STD_PAGE_byte ) + KERNEL_PAGE_mirror + (STD_PAGE_byte - sizeof( struct KERNEL_STRUCTURE_IDT_RETURN )));

	// code descriptor
	context -> cs = offsetof( struct KERNEL_STRUCTURE_GDT, cs_ring3 ) | 0x03;

	// basic processor state flags
	context -> eflags = KERNEL_TASK_EFLAGS_default;

	// stack descriptor
	context -> ss = offsetof( struct KERNEL_STRUCTURE_GDT, ds_ring3 ) | 0x03;

	// stack pointer of process
	context -> rsp = KERNEL_TASK_STACK_pointer - 0x18;	// no args

	// set thread entry address
	context -> rip = function;

	//----------------------------------------------------------------------

	// map stack space to thread paging array
	kernel_page_alloc( (uint64_t *) thread -> cr3, KERNEL_TASK_STACK_pointer - KERNEL_TASK_STACK_limit, KERNEL_TASK_STACK_limit >> STD_SHIFT_PAGE, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | (thread -> page_type << KERNEL_PAGE_TYPE_offset) );

	// page used
	thread -> stack++;

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
	thread -> flags |= STD_TASK_FLAG_active | STD_TASK_FLAG_thread | STD_TASK_FLAG_init;

	// return process ID of new thread
	return thread -> pid;
}

int64_t kernel_syscall_pid( void ) {
	// task properties
	struct KERNEL_STRUCTURE_TASK *task = kernel_task_active();

	// return task ID
	return task -> pid;
}

int64_t	kernel_syscall_exec( uint8_t *name, uint64_t length, uint8_t stream_flow, uint8_t detach ) {
	// return new process ID
	return kernel_exec( name, length, stream_flow, detach );
}

uint8_t kernel_syscall_pid_check( int64_t pid ) {
	// find an entry with selected ID
	for( uint64_t i = 0; i < KERNEL_TASK_limit; i++ ) {
		// ignore kernels ID
		if( ! pid ) return FALSE;

		// entry occupied?
		if( ! kernel -> task_base_address[ i ].flags ) continue;	// no

		// found?
		if( kernel -> task_base_address[ i ].pid == pid ) return TRUE;
	}

	// process not found
	return FALSE;
}

void kernel_syscall_ipc_send( int64_t pid, uint8_t *data ) {
	// deny access, only one logical processor at a time
	MACRO_LOCK( kernel -> ipc_semaphore );

	// wait for free stack area
	while( TRUE ) {
		// scan whole IPC area
		for( uint64_t i = 0; i < KERNEL_IPC_limit; i++ ) {
			// free entry found?
			if( kernel -> ipc_base_address[ i ].ttl > kernel -> time_rtc ) continue;	// no

			// send to
			kernel -> ipc_base_address[ i ].target = pid;

			// sent from
			kernel -> ipc_base_address[ i ].source = kernel_task_pid();

			// load data into message
			for( uint8_t j = 0; j < STD_IPC_SIZE_byte; j++ )
				kernel -> ipc_base_address[ i ].data[ j ] = data[ j ];

			// set message timeout
			kernel -> ipc_base_address[ i ].ttl = kernel -> time_rtc + KERNEL_IPC_ttl;

			// unlock access to IPC area
			MACRO_UNLOCK( kernel -> ipc_semaphore );

			// message sent
			return;
		}
	}
}

int64_t kernel_syscall_ipc_receive( uint8_t *data ) {
	// from who we received message
	int64_t source = EMPTY;	// by default no one

	// scan whole IPC area
	for( uint64_t i = 0; i < KERNEL_IPC_limit; i++ ) {
		// message alive?
		if( kernel -> time_rtc > kernel -> ipc_base_address[ i ].ttl ) continue;	// no
	
		// message for us?
		if( kernel -> ipc_base_address[ i ].target != kernel_task_pid() ) continue;	// no

		// load data into message
		for( uint8_t j = 0; j < STD_IPC_SIZE_byte; j++ )
			data[ j ] = kernel -> ipc_base_address[ i ].data[ j ];

		// retrieve message source
		source = kernel -> ipc_base_address[ i ].source;

		// mark entry as free
		kernel -> ipc_base_address[ i ].ttl = EMPTY;

		// message received
		break;
	}

	// return message source
	return source;
}

uintptr_t kernel_syscall_memory_share( int64_t pid, uintptr_t source, uint64_t pages ) {
	// properties of target task
	struct KERNEL_STRUCTURE_TASK *target = (struct KERNEL_STRUCTURE_TASK *) kernel_task_by_id( pid );

	// acquire space from target task
	uintptr_t target_pointer = kernel_memory_acquire( target -> memory_map, pages, KERNEL_MEMORY_HIGH, kernel -> page_limit ) << STD_SHIFT_PAGE;

	// connect memory space of parent process with child
	kernel_page_clang( (uint64_t *) target -> cr3, source, target_pointer, pages, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | (KERNEL_PAGE_TYPE_SHARED << KERNEL_PAGE_TYPE_offset) );

	// return the address of the first page in the collection
	return target_pointer;
}

void kernel_syscall_mouse( struct STD_STRUCTURE_MOUSE_SYSCALL *mouse ) {
	// return information about mouse device
	mouse -> x	= kernel -> device_mouse_x;
	mouse -> y	= kernel -> device_mouse_y;
	mouse -> z	= kernel -> device_mouse_z;
	mouse -> status	= kernel -> device_mouse_status;
}

void kernel_syscall_framebuffer_change( struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER *framebuffer ) {
	// process is allowed for modifications?
	if( kernel_task_pid() != kernel -> framebuffer_pid ) return;	// no

	// change new framebuffer owner
	kernel -> framebuffer_pid = framebuffer -> pid;
}

uint8_t kernel_syscall_ipc_receive_by_pid( uint8_t *data, int64_t pid ) {
	// scan whole IPC area
	for( uint64_t i = 0; i < KERNEL_IPC_limit; i++ ) {
		// message alive?
		if( kernel -> time_rtc > kernel -> ipc_base_address[ i ].ttl ) continue;	// no
	
		// message for us?
		if( kernel -> ipc_base_address[ i ].target != kernel_task_pid() ) continue;	// no

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

uint8_t kernel_syscall_stream_out( uint8_t *string, uint64_t length ) {
	// current task properties
	struct KERNEL_STRUCTURE_TASK *task = kernel_task_active();

	// block access to stream modification
	MACRO_LOCK( task -> stream_out -> semaphore );

	// stream closed or full?
	if( task -> stream_out -> flags & KERNEL_STREAM_FLAG_closed || task -> stream_out -> length_byte == STD_STREAM_SIZE_page << STD_SHIFT_PAGE ) {
		// unlock stream access
		MACRO_UNLOCK( task -> stream_out -> semaphore );

		// yes
		return FALSE;
	}

	// amount of data inside stream after operation
	task -> stream_out -> length_byte += length;

	// can we fit it?
	if( length > (STD_STREAM_SIZE_page << STD_SHIFT_PAGE) - task -> stream_out -> length_byte ) {	// no
		// unlock stream access
		MACRO_UNLOCK( task -> stream_out -> semaphore );

		// unable to fit string
		return FALSE;
	}

	// insert data from end marker to end of stream
	if( task -> stream_out -> end >= task -> stream_out -> start ) {
		while( length && task -> stream_out -> end != (STD_STREAM_SIZE_page << STD_SHIFT_PAGE) ) {
			task -> stream_out -> base_address[ task -> stream_out -> end++ ] = *(string++);

			// character inserted
			length--;
		}

		// move end marker at beginning of stream content
		if( task -> stream_out -> end == (STD_STREAM_SIZE_page << STD_SHIFT_PAGE) || length ) task -> stream_out -> end = EMPTY;
	}

	// insert data from end marker to start marker
	while( length && task -> stream_out -> end != task -> stream_out -> start ) {
		task -> stream_out -> base_address[ task -> stream_out -> end++ ] = *(string++);

		// character inserted
		length--;
	}

	// string content is modified
	task -> stream_out -> flags |= KERNEL_STREAM_FLAG_modified;

	// unlock stream access
	MACRO_UNLOCK( task -> stream_out -> semaphore );

	// string sended
	return TRUE;
}

uint64_t kernel_syscall_stream_in( uint8_t *target ) {
	// get the process output stream id
	struct KERNEL_STRUCTURE_TASK *task = (struct KERNEL_STRUCTURE_TASK *) kernel_task_active();

	// block access to stream modification
	MACRO_LOCK( task -> stream_in -> semaphore );

	// stream closed or empty?
	if( task -> stream_in -> flags & KERNEL_STREAM_FLAG_closed || ! task -> stream_in -> length_byte ) {
		// unlock stream modification
		MACRO_UNLOCK( task -> stream_in -> semaphore );

		// yes
		return EMPTY;
	}

	// passed data from stream
	uint64_t length = EMPTY;

	// send data from start marker to end of stream
	if( task -> stream_in -> start >= task -> stream_in -> end ) {
		while( task -> stream_in -> start < (STD_STREAM_SIZE_page << STD_SHIFT_PAGE) )
			target[ length++ ] = task -> stream_in -> base_address[ task -> stream_in -> start++ ];

		// move start marker at beginning of stream content
		if( task -> stream_in -> start == (STD_STREAM_SIZE_page << STD_SHIFT_PAGE) || length ) task -> stream_in -> start = EMPTY;
	}

	// send data from start marker to end marker
	while( task -> stream_in -> start != task -> stream_in -> end )
		target[ length++ ] = task -> stream_in -> base_address[ task -> stream_in -> start++ ];

	// stream empty
	task -> stream_in -> length_byte = EMPTY;

	// unlock stream modification
	MACRO_UNLOCK( task -> stream_in -> semaphore );

	// return amount of transferred data in Bytes
	return length;
}

uint16_t kernel_syscall_keyboard( void ) {
	// current task properties
	struct KERNEL_STRUCTURE_TASK *task = kernel_task_active();

	// am I framebuffer manager?
	if( task -> pid != kernel -> framebuffer_pid ) return EMPTY;	// no, return no key

	// get first key code from buffer
	uint16_t key = kernel -> device_keyboard[ 0 ];

	// move all characters inside buffer, forward one position
	for( uint8_t i = 0; i < 7; i++ ) kernel -> device_keyboard[ i ] = kernel -> device_keyboard[ i + 1 ];

	// drain last key
	kernel -> device_keyboard[ 7 ] = EMPTY;

	// return key code
	return key;
}

void kernel_syscall_stream_set( uint8_t *meta, uint8_t stream_type ) {
	// current task properties
	struct KERNEL_STRUCTURE_TASK *task = kernel_task_active();

	// stream properties
	struct KERNEL_STRUCTURE_STREAM *stream;

	// choose stream type
	if( stream_type == STD_STREAM_IN ) stream = task -> stream_in;
	else stream = task -> stream_out;

	// block access to stream modification
	MACRO_LOCK( stream -> semaphore );

	// update stream meta only if is empty
	if( ! stream -> length_byte ) {
		// update meta data
		for( uint8_t i = 0; i < STD_STREAM_META_limit; i++ ) stream -> meta[ i ] = meta[ i ];

		// meta data is consistent
		stream -> flags &= ~KERNEL_STREAM_FLAG_modified;
	}

	// unlock stream modification
	MACRO_UNLOCK( stream -> semaphore );
}

uint8_t kernel_syscall_stream_get( uint8_t *target, uint8_t stream_type ) {
	// current task properties
	struct KERNEL_STRUCTURE_TASK *task = kernel_task_active();

	// stream properties
	struct KERNEL_STRUCTURE_STREAM *stream;

	// choose stream type
	if( stream_type == STD_STREAM_IN ) stream = task -> stream_in;
	else stream = task -> stream_out;

	// block access to stream modification
	MACRO_LOCK( stream -> semaphore );

	// retrieve meta data
	for( uint8_t i = 0; i < STD_STREAM_META_limit; i++ ) target[ i ] = stream -> meta[ i ];

	// meta data was up to date?
	uint8_t status = TRUE;
	if( stream -> flags & KERNEL_STREAM_FLAG_modified ) status = FALSE;	// nope

	// unlock stream modification
	MACRO_UNLOCK( stream -> semaphore );

	// return meta data status
	return status;
}

void kernel_syscall_memory( struct STD_STRUCTURE_SYSCALL_MEMORY *memory ) {
	// all available Bytes
	memory -> total = kernel -> page_total << STD_SHIFT_PAGE;

	// and currently free
	memory -> available = kernel -> page_available << STD_SHIFT_PAGE;

	// used by paging
	memory -> paging = kernel -> page_structure << STD_SHIFT_PAGE;

	// shared between processes (only parent process will have it counted in memory use)
	memory -> shared = kernel -> page_shared << STD_SHIFT_PAGE;
}

uint8_t kernel_syscall_cd( uint8_t *path, uint64_t path_length ) {
	// current task properties
	struct KERNEL_STRUCTURE_TASK *task = kernel_task_active();
	
	// properties of storage
	struct KERNEL_STRUCTURE_STORAGE *storage = (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ task -> storage ];

	// try to open provided path
	struct KERNEL_STRUCTURE_VFS *socket = (struct KERNEL_STRUCTURE_VFS *) storage -> fs.open( storage, path, path_length, EMPTY );

	// if file doesn't exist
	if( ! socket ) return FALSE;

	// directory change semaphore
	uint8_t flag = FALSE;

	// properties of opened file
	struct LIB_VFS_STRUCTURE vfs = storage -> fs.file( socket );

	// it is a directory?
	if( vfs.type & STD_FILE_TYPE_directory ) {
		// set new root directory of current process
		task -> directory = (uint64_t) socket -> knot;

		// directory changed
		flag = TRUE;
	}

	// close file
	storage -> fs.close( socket );

	// done
	return flag;
}

int64_t kernel_syscall_ipc_receive_by_type( uint8_t *data, uint8_t type ) {
	// scan whole IPC area
	for( uint64_t i = 0; i < KERNEL_IPC_limit; i++ ) {
		// message alive?
		if( kernel -> time_rtc > kernel -> ipc_base_address[ i ].ttl ) continue;	// no
	
		// message for us?
		if( kernel -> ipc_base_address[ i ].target != kernel_task_pid() ) continue;	// no

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

uint64_t kernel_syscall_microtime( void ) {
	// return microtime
	return kernel -> time_rtc;
}

uint64_t kernel_syscall_time( void ) {
	// return current date and time
	return driver_rtc_time();
}

int64_t kernel_syscall_file_open( uint8_t *path, uint64_t path_length, uint8_t mode ) {
	// current task properties
	struct KERNEL_STRUCTURE_TASK *task = kernel_task_active();

	// storage selected by default
	uint64_t storage_id = task -> storage;

	// find colon inside path
	uint64_t i = 0;
	for( ; i < path_length; i++ ) if( path[ i ] == STD_ASCII_COLON ) {
		// storage by ID?
		if( lib_string_length_scope_digit( path ) ) {
			// retrieve storage ID
			uint64_t id = lib_string_to_integer( path, STD_NUMBER_SYSTEM_decimal );

			// invalid storage?
			if( id >= KERNEL_STORAGE_limit || ! (kernel -> storage_base_address[ id ].flags & KERNEL_STORAGE_FLAGS_active)) return STD_ERROR_file_not_found;	// yep

			// select storage
			storage_id = id;
		} else {
			// storage by name

			// find storage with exact name
			for( uint64_t j = 0; j < KERNEL_STORAGE_limit; j++ ) {
				// storage active?
				if( ! (kernel -> storage_base_address[ j ].flags & KERNEL_STORAGE_FLAGS_active) ) continue;	// no

				// storage found?
				if( lib_string_compare( path, kernel -> storage_base_address[ j ].device_name, i ) ) { storage_id = j; break; }	// yes
			}
		}

		// remove storage from path
		path += i + 1;
		path_length -= i + 1;

		// ready
		break;
	}

	// retrieve information about module file
	struct KERNEL_STRUCTURE_VFS *socket = kernel -> storage_base_address[ storage_id ].fs.open( (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ storage_id ], path, path_length, mode );

	// if file doesn't exist
	if( ! socket ) return STD_ERROR_file_not_found;

	// return socket ID
	return ((uintptr_t) socket - (uintptr_t) kernel -> vfs_base_address) / sizeof( struct KERNEL_STRUCTURE_VFS );
}

void kernel_syscall_file_close( int64_t socket ) {
	// invalid socket value?
	if( socket > KERNEL_VFS_limit ) return;	// yep, ignore

	// current task properties
	struct KERNEL_STRUCTURE_TASK *task = kernel_task_active();

	// close connection to file
	kernel -> storage_base_address[ task -> storage ].fs.close( (struct KERNEL_STRUCTURE_VFS *) &kernel -> vfs_base_address[ socket ] );
}

void kernel_syscall_file( struct STD_STRUCTURE_FILE *file ) {
	// invalid socket value?
	if( file -> socket > KERNEL_VFS_limit ) return;	// yep, ignore

	// retrieve information about file
	struct LIB_VFS_STRUCTURE vfs = kernel -> storage_base_address[ ((struct KERNEL_STRUCTURE_VFS *) &kernel -> vfs_base_address[ file -> socket ]) -> storage ].fs.file( (struct KERNEL_STRUCTURE_VFS *) &kernel -> vfs_base_address[ file -> socket ] );

	// copy important values

	// file size in Bytes
	file -> byte = vfs.limit;

	// file name
	for( uint64_t i = 0; i < vfs.name_limit; i++ ) file -> name[ file -> name_length++ ] = vfs.name[ i ];
}

void kernel_syscall_file_read( struct STD_STRUCTURE_FILE *file, uint8_t *target, uint64_t byte ) {
	// invalid socket value?
	if( file -> socket > KERNEL_VFS_limit ) return;	// yep, ignore

	// pass file content to process memory
	kernel -> storage_base_address[ ((struct KERNEL_STRUCTURE_VFS *) &kernel -> vfs_base_address[ file -> socket ]) -> storage ].fs.read( (struct KERNEL_STRUCTURE_VFS *) &kernel -> vfs_base_address[ file -> socket ], target, file -> seek, byte );

	// next file Bytes content
	file -> seek += byte;
}

void kernel_syscall_file_write( struct STD_STRUCTURE_FILE *file, uint8_t *source, uint64_t byte ) {
	// invalid socket value?
	if( file -> socket > KERNEL_VFS_limit ) return;	// yep, ignore

	// pass file content to storage
	kernel -> storage_base_address[ ((struct KERNEL_STRUCTURE_VFS *) &kernel -> vfs_base_address[ file -> socket ]) -> storage ].fs.write( (struct KERNEL_STRUCTURE_VFS *) &kernel -> vfs_base_address[ file -> socket ], source, file -> seek, byte );

	// next file Bytes content
	file -> seek += byte;
}

int64_t kernel_syscall_file_touch( uint8_t *path, uint8_t type ) {
	// current task properties
	struct KERNEL_STRUCTURE_TASK *task = kernel_task_active();

	// storage selected by default
	uint64_t storage_id = task -> storage;

	// find colon inside path
	uint64_t i = 0;
	for( ; i < lib_string_length( path ); i++ ) if( path[ i ] == STD_ASCII_COLON ) {
		// storage by ID?
		if( lib_string_length_scope_digit( path ) ) {
			// retrieve storage ID
			uint64_t id = lib_string_to_integer( path, STD_NUMBER_SYSTEM_decimal );

			// invalid storage?
			if( id >= KERNEL_STORAGE_limit || ! (kernel -> storage_base_address[ id ].flags & KERNEL_STORAGE_FLAGS_active)) return STD_ERROR_file_not_found;	// yep

			// select storage
			storage_id = id;
		} else {
			// storage by name

			// find storage with exact name
			for( uint64_t j = 0; j < KERNEL_STORAGE_limit; j++ ) {
				// storage active?
				if( ! (kernel -> storage_base_address[ j ].flags & KERNEL_STORAGE_FLAGS_active) ) continue;	// no

				// storage found?
				if( lib_string_compare( path, kernel -> storage_base_address[ j ].device_name, i ) ) { storage_id = j; break; }	// yes
			}
		}

		// remove storage from path
		path += i + 1;

		// ready
		break;
	}

	// retrieve information about module file
	struct KERNEL_STRUCTURE_VFS *socket = (struct KERNEL_STRUCTURE_VFS *) kernel -> storage_base_address[ storage_id ].fs.touch( (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ storage_id ], path, lib_string_length( path ), type );

	// if file doesn't exist
	if( ! socket ) return STD_ERROR_file_not_found;

	// return socket ID
	return ((uintptr_t) socket - (uintptr_t) kernel -> vfs_base_address) / sizeof( struct KERNEL_STRUCTURE_VFS );
}

uintptr_t kernel_syscall_task( void ) {
	// amount of entries to pass
	uint64_t count = 1;

	// count entries
	for( uint64_t i = 1; i < KERNEL_TASK_limit; i++ ) if( kernel -> task_base_address[ i ].flags ) count++;

	// alloc enough memory for all entries
	struct STD_STRUCTURE_SYSCALL_TASK *task = (struct STD_STRUCTURE_SYSCALL_TASK *) kernel_syscall_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct STD_STRUCTURE_SYSCALL_TASK ) * count ) >> STD_SHIFT_PAGE );

	// copy essential information about every task
	uint64_t entry = 0;
	for( uint64_t i = 1; i < KERNEL_TASK_limit; i++ ) {
		// process PID
		task[ entry ].pid = kernel -> task_base_address[ i ].pid;

		// amount of used memory (in Pages)
		task[ entry ].page = kernel -> task_base_address[ i ].page;

		// amount of user memory for stack (in Pages)
		task[ entry ].stack = kernel -> task_base_address[ i ].stack;
	
		// current status of task
		task[ entry ].flags = kernel -> task_base_address[ i ].flags;

		// measured time
		task[ entry ].time = kernel -> task_base_address[ i ].time;

		// name of task with length
		for( uint64_t j = 0; j < kernel -> task_base_address[ i ].name_length; j++ ) task[ entry ].name[ task[ entry ].name_length++ ] = kernel -> task_base_address[ i ].name[ j ]; task[ entry ].name[ task[ entry ].name_length ] = STD_ASCII_TERMINATOR;

		// required amount of entries passed?
		if( entry++ == count ) break;	// yes
	}

	// share structure with process
	return (uintptr_t) task;
}

void kernel_syscall_kill( int64_t pid ) {
	// properties of current task
	struct KERNEL_STRUCTURE_TASK *task = kernel_task_by_id( pid );

	// mark task as ready to close
	task -> flags |= STD_TASK_FLAG_close;
}

void kernel_syscall_network_interface( struct STD_STRUCTURE_NETWORK_INTERFACE *interface ) {
	// share set interface properties

	// IPv4 address
	interface -> ipv4_address = kernel -> network_interface.ipv4_address;

	// Mask address
	interface -> ipv4_mask = kernel -> network_interface.ipv4_mask;

	// Broadcast address
	interface -> ipv4_broadcast = kernel -> network_interface.ipv4_broadcast;

	// Gateway address
	interface -> ipv4_gateway = kernel -> network_interface.ipv4_gateway;

	// MAC address
	for( uint8_t i = 0; i < 6; i++ ) interface -> ethernet_address[ i ] = kernel -> network_interface.ethernet_address[ i ];

	// statistics

	// received frames and Bytes
	interface -> rx_frame = kernel -> network_interface.rx_frame;
	interface -> tx_frame = kernel -> network_interface.tx_frame;

	// transferred frames and Bytes
	interface -> rx_byte = kernel -> network_interface.rx_byte;
	interface -> tx_byte = kernel -> network_interface.tx_byte;
}

int64_t kernel_syscall_network_open( uint8_t protocol, uint32_t ipv4_target, uint16_t port_target, uint16_t port_local ) {
	// try to acquire socket
	struct KERNEL_STRUCTURE_NETWORK_SOCKET *socket = kernel_network_socket();

	// connection limit reached?
	if( ! socket ) return EMPTY;

	// bind selected port
	if( ! kernel_network_socket_port( socket, port_local ) ) {
		// release socket
		socket -> pid = EMPTY;

		// port already in use
		return STD_ERROR_locked;
	}

	// set socket properties
	socket -> ethernet_lease_time	= EMPTY;
	socket -> protocol		= protocol;
	socket -> ipv4_target		= ipv4_target;
	socket -> port_target		= port_target;
	socket -> ipv4_id		= kernel_syscall_microtime();

	// set sockets ipv4 protocol
	switch( protocol ) {
		// ICMP
		case STD_NETWORK_PROTOCOL_icmp: { socket -> ipv4_protocol = KERNEL_NETWORK_HEADER_IPV4_PROTOCOL_icmp; break; }

		// UDP
		case STD_NETWORK_PROTOCOL_udp: { socket -> ipv4_protocol = KERNEL_NETWORK_HEADER_IPV4_PROTOCOL_udp; break; }
		
		// TCP
		case STD_NETWORK_PROTOCOL_tcp: { socket -> ipv4_protocol = KERNEL_NETWORK_HEADER_IPV4_PROTOCOL_tcp; break; }
	}

	// wait for socket resolution
	if( ! kernel_network_ethernet_resolve( socket ) ) {
		// close socket
		kernel_network_socket_close( socket );

		// cannot create connection
		return STD_ERROR_unavailable;
	}

	// socket configures, initialize
	socket -> flags = KERNEL_NETWORK_SOCKET_FLAG_init;	// if socket is of type TCP, network module will try to establish connection with target

	// return socket ID
	return (int64_t) ((uintptr_t) socket - (uintptr_t) kernel -> network_socket_list) / sizeof( struct KERNEL_STRUCTURE_NETWORK_SOCKET );
}

int64_t kernel_syscall_network_send( int64_t socket, uint8_t *data, uint64_t length ) {
	// socket can exist?
	if( socket > KERNEL_NETWORK_SOCKET_limit ) return STD_ERROR_unavailable;	// no

	// socket exist and belongs to process?
	if( kernel -> network_socket_list[ socket ].pid != kernel_task_pid() ) return STD_ERROR_unavailable;	// no

	// pass execution to Network module
	return kernel_network_send( socket, data, length );
}

void kernel_syscall_network_interface_set( struct STD_STRUCTURE_NETWORK_INTERFACE *interface ) {
	// update IPv4 address
	kernel -> network_interface.ipv4_address = interface -> ipv4_address;

	// update Mask address
	kernel -> network_interface.ipv4_mask = interface -> ipv4_mask;

	// update Broadcast address
	kernel -> network_interface.ipv4_broadcast = interface -> ipv4_broadcast;

	// update Gateway address
	kernel -> network_interface.ipv4_gateway = interface -> ipv4_gateway;
}

void kernel_syscall_network_receive( int64_t socket, struct STD_STRUCTURE_NETWORK_DATA *packet ) {
	// socket can exist?
	if( socket > KERNEL_NETWORK_SOCKET_limit ) return;	// no

	// socket exist and belongs to process?
	if( kernel -> network_socket_list[ socket ].pid != kernel_task_pid() ) return;	// no

	// packet properties
	uint8_t *data = (uint8_t *) (kernel -> network_socket_list[ socket ].data_in[ 0 ] & STD_PAGE_mask);
	uint64_t length = kernel -> network_socket_list[ socket ].data_in[ 0 ] & ~STD_PAGE_mask;

	// nothing to transfer?
	if( ! length ) return;	// yep

	// block access to stack modification
	MACRO_LOCK( kernel -> network_socket_list[ socket ].data_in_semaphore );

	// remove packet from queue
	for( uint64_t i = 0; i < KERNEL_NETWORK_SOCKET_DATA_limit; i++ ) kernel -> network_socket_list[ socket ].data_in[ i ] = kernel -> network_socket_list[ socket ].data_in[ i + 1 ];

	// unlock
	MACRO_UNLOCK( kernel -> network_socket_list[ socket ].data_in_semaphore );

	// alloc memory inside process area
	packet -> data = (uint8_t *) kernel_syscall_memory_alloc( MACRO_PAGE_ALIGN_UP( length ) >> STD_SHIFT_PAGE );

	// move data content
	for( uint64_t i = 0; i < length; i++ ) packet -> data[ i ] = data[ i ];

	// inform about length of transffered data
	packet -> length = length;

	// release packet area from kernel memory
	kernel_memory_release( (uintptr_t) data, MACRO_PAGE_ALIGN_UP( length ) >> STD_SHIFT_PAGE );
}

uintptr_t kernel_syscall_storage( void ) {
	// amount of entries to pass
	uint64_t limit = 1;	// last entry, always empty

	// count entries
	for( uint64_t i = 0; i < KERNEL_STORAGE_limit; i++ ) if( kernel -> storage_base_address[ i ].flags & KERNEL_STORAGE_FLAGS_active ) limit++;

	// alloc enough memory for all entries
	struct STD_STRUCTURE_STORAGE *storage = (struct STD_STRUCTURE_STORAGE *) kernel_syscall_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct STD_STRUCTURE_STORAGE ) * limit ) >> STD_SHIFT_PAGE );

	// copy essential information about every storage
	uint64_t entry = 0; for( uint64_t i = 0; i < KERNEL_STORAGE_limit && limit != entry; i++ ) {
		// ignore RamFS entry
		// if( i == TRUE ) continue;

		// identificator of storage
		storage[ entry ].id = i;

		// type of storage
		storage[ entry ].type = kernel -> storage_base_address[ i ].device_type;

		// size in Bytes
		storage[ entry ].limit = kernel -> storage_base_address[ i ].device_limit;

		// available in Bytes
		storage[ entry ].available = kernel -> storage_base_address[ i ].device_free;

		// name
		storage[ entry ].name_limit = kernel -> storage_base_address[ i ].device_name_limit;
		for( uint8_t c = 0; c < storage[ entry ].name_limit && c < KERNEL_STORAGE_NAME_limit; c++ ) storage[ entry ].name[ c ] = kernel -> storage_base_address[ i ].device_name[ c ];

		// next antry position
		entry++;
	}

	// share structure with process
	return (uintptr_t) storage;
}

uint8_t kernel_syscall_storage_select( uint8_t *name ) {
	// current task properties
	struct KERNEL_STRUCTURE_TASK *task = kernel_task_active();

	// storage selected by default
	uint64_t storage_id = task -> storage;

	// storage by ID?
	if( lib_string_length_scope_digit( name ) ) storage_id = lib_string_to_integer( name, STD_NUMBER_SYSTEM_decimal );
	else {
		// storage by name

		// find storage with exact name
		for( uint64_t j = 0; j < KERNEL_STORAGE_limit; j++ ) {
			// storage active?
			if( ! (kernel -> storage_base_address[ j ].flags & KERNEL_STORAGE_FLAGS_active) ) continue;	// no

			// storage found?
			if( lib_string_compare( name, kernel -> storage_base_address[ j ].device_name, lib_string_length( name ) ) ) { storage_id = j; break; }	// yes
		}
	}

	// storage available?
	if( storage_id >= KERNEL_STORAGE_limit || ! (kernel -> storage_base_address[ storage_id ].flags & KERNEL_STORAGE_FLAGS_active) ) return FALSE;	// no

	// set task default storage
	task -> storage = storage_id;

	// set current directory to root
	task -> directory = kernel -> storage_base_address[ storage_id ].fs.root_directory_id;

	// storage set
	return TRUE;
}

uintptr_t kernel_syscall_dir( uint8_t *path ) {
	// current task properties
	struct KERNEL_STRUCTURE_TASK *task = kernel_task_active();

	// return content of directory from current storage
	return kernel -> storage_base_address[ task -> storage ].fs.dir( (struct KERNEL_STRUCTURE_STORAGE *) &kernel -> storage_base_address[ task -> storage ], path, lib_string_length( path ) );
}