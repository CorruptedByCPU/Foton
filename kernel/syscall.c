/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_syscall_exit( void ) {
	// properties of current task
	struct KERNEL_TASK_STRUCTURE *task = kernel -> task_cpu_address[ kernel_lapic_id() ];

	// mark task as not active and ready to close
	task -> flags &= ~KERNEL_TASK_FLAG_active;
	task -> flags |= KERNEL_TASK_FLAG_close;

	// release left BS/A time
	__asm__ volatile( "int $0x20" );
}

void kernel_syscall_framebuffer( struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER *framebuffer ) {
	// return information about existing framebuffer
	framebuffer -> base_address	= kernel -> framebuffer_base_address;
	framebuffer -> width_pixel	= kernel -> framebuffer_width_pixel;
	framebuffer -> height_pixel	= kernel -> framebuffer_height_pixel;
	framebuffer -> pitch_byte	= kernel -> framebuffer_pitch_byte;

	// change framebuffer owner if possible
	if( ! __sync_val_compare_and_swap( &kernel -> framebuffer_pid, EMPTY, kernel_task_pid() ) )
		// approved
		framebuffer -> base_address = (uint32_t *) kernel_memory_share( (uintptr_t) kernel -> framebuffer_base_address & ~KERNEL_PAGE_PML5_mask, MACRO_PAGE_ALIGN_UP( kernel -> framebuffer_pitch_byte * kernel -> framebuffer_height_pixel) >> STD_SHIFT_PAGE );

	// return information about framebuffer owner
	framebuffer -> pid = kernel -> framebuffer_pid;
}

uintptr_t kernel_syscall_memory_alloc( uint64_t page ) {
	// task properties
	struct KERNEL_TASK_STRUCTURE *task = (struct KERNEL_TASK_STRUCTURE *) kernel_task_active();

	// acquire N continuous pages
	uintptr_t allocated = EMPTY;
	if( (allocated = kernel_memory_acquire_secured( task, page )) ) {
		// allocate space inside process paging area
		kernel_page_alloc( (uint64_t *) task -> cr3, KERNEL_EXEC_base_address + (allocated << STD_SHIFT_PAGE), page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | KERNEL_PAGE_FLAG_process );

		// process memory usage
		task -> page += page;

		// return the address of the first page in the collection
		return KERNEL_EXEC_base_address + (allocated << STD_SHIFT_PAGE);
	}

	// debug
	kernel -> log( (uint8_t *) "KERNEL: low memory.\n" );

	// no free space
	return EMPTY;
}

void kernel_syscall_memory_release( uintptr_t source, uint64_t page) {
	// task properties
	struct KERNEL_TASK_STRUCTURE *task = (struct KERNEL_TASK_STRUCTURE *) kernel_task_active();

	// release occupied pages
	for( uint64_t i = source >> STD_SHIFT_PAGE; i < (source >> STD_SHIFT_PAGE) + page; i++ ) {
		// remove page from paging structure
		uintptr_t page = kernel_page_remove( (uint64_t *) task -> cr3, i << STD_SHIFT_PAGE );

		// if released
		if( page ) {
			// return page back to stack
			kernel_memory_release_page( page );

			// process memory usage
			task -> page--;
		}

		// release page in binary memory map of process
		kernel_memory_dispose( task -> memory_map, i, 1 );
	}
}

uint64_t kernel_syscall_uptime( void ) {
	// return uptime
	return kernel -> time_unit / KERNEL_RTC_Hz;
}

void kernel_syscall_log( uint8_t *string, uint64_t length ) {
	// if string pointer is above software environment memory area
	if( (uint64_t) string > KERNEL_PAGE_logical ) return;	// do not allow it

	// show content of string
	for( uint64_t i = 0; i < length; i++ ) kernel -> log( (uint8_t *) "%c", (uint64_t) string[ i ] );
}

int64_t kernel_syscall_thread( uintptr_t function, uint8_t *name, uint64_t length ) {
	// create a new thread in task queue
	struct KERNEL_TASK_STRUCTURE *thread = kernel_task_add( name, length );

	//----------------------------------------------------------------------

	// prepare Paging table for new process
	thread -> cr3 = kernel_memory_alloc_page() | KERNEL_PAGE_logical;

	//----------------------------------------------------------------------

	// describe space under thread context stack
	kernel_page_alloc( (uintptr_t *) thread -> cr3, KERNEL_STACK_address, 2, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_process );

	// set initial startup configuration for new process
	struct KERNEL_IDT_STRUCTURE_RETURN *context = (struct KERNEL_IDT_STRUCTURE_RETURN *) (kernel_page_address( (uintptr_t *) thread -> cr3, KERNEL_STACK_pointer - STD_PAGE_byte ) + KERNEL_PAGE_logical + (STD_PAGE_byte - sizeof( struct KERNEL_IDT_STRUCTURE_RETURN )));

	// code descriptor
	context -> cs = offsetof( struct KERNEL_GDT_STRUCTURE, cs_ring3 ) | 0x03;

	// basic processor state flags
	context -> eflags = KERNEL_TASK_EFLAGS_default;

	// stack descriptor
	context -> ss = offsetof( struct KERNEL_GDT_STRUCTURE, ds_ring3 ) | 0x03;

	// stack pointer of process
	context -> rsp = KERNEL_TASK_STACK_pointer - 0x18;	// no args

	// set thread entry address
	context -> rip = function;

	//----------------------------------------------------------------------

	// prepare space for stack of thread
	uint8_t *process_stack = (uint8_t *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( 0x20 ) >> STD_SHIFT_PAGE );

	// context stack top pointer
	thread -> rsp = KERNEL_STACK_pointer - sizeof( struct KERNEL_IDT_STRUCTURE_RETURN );

	// map stack space to thread paging array
	kernel_page_map( (uintptr_t *) thread -> cr3, (uintptr_t) process_stack, context -> rsp & STD_PAGE_mask, MACRO_PAGE_ALIGN_UP( 0x20 ) >> STD_SHIFT_PAGE, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | KERNEL_PAGE_FLAG_process );

	//----------------------------------------------------------------------

	// aquire parent task properties
	struct KERNEL_TASK_STRUCTURE *task = kernel_task_active();

	// threads use same memory map as parent
	thread -> memory_map = task -> memory_map;

	//----------------------------------------------------------------------

	// map parent space to thread
	kernel_page_merge( (uint64_t *) task -> cr3, (uint64_t *) thread -> cr3 );

	// thread ready to run
	thread -> flags |= KERNEL_TASK_FLAG_active | KERNEL_TASK_FLAG_thread | KERNEL_TASK_FLAG_init;

	// return process ID of new thread
	return thread -> pid;
}

int64_t kernel_syscall_pid( void ) {
	// task properties
	struct KERNEL_TASK_STRUCTURE *task = kernel_task_active();

	// return task ID
	return task -> pid;
}

int64_t	kernel_syscall_exec( uint8_t *name, uint64_t length, uint8_t stream_flow ) {
	// return new process ID
	return kernel_exec( name, length, stream_flow );
}

uint8_t kernel_syscall_pid_check( int64_t pid ) {
	// find an entry with selected ID
	for( uint64_t i = 0; i < kernel -> task_limit; i++ ) {
		// entry occupied?
		if( ! kernel -> task_base_address[ i ].flags ) continue;	// no

		// found?
		if( kernel -> task_base_address[ i ].pid == pid ) {
			// task closed?
			if( kernel -> task_base_address[ i ].flags & KERNEL_TASK_FLAG_close ) return FALSE;	// yes
			else return TRUE;	// no, running
		}
	}

	// process not found
	return FALSE;
}

void kernel_syscall_ipc_send( int64_t pid, uint8_t *data ) {
	// deny access, only one logical processor at a time
	while( __sync_val_compare_and_swap( &kernel -> ipc_semaphore, UNLOCK, LOCK ) );

	// wait for free stack area
	while( TRUE ) {
		// scan whole IPC area
		for( uint64_t i = 0; i < KERNEL_IPC_limit; i++ ) {
			// free entry found?
			if( kernel -> ipc_base_address[ i ].ttl > kernel -> time_unit ) continue;	// no

			// send to
			kernel -> ipc_base_address[ i ].target = pid;

			// sent from
			kernel -> ipc_base_address[ i ].source = kernel -> task_pid();

			// load data into message
			for( uint8_t j = 0; j < STD_IPC_SIZE_byte; j++ )
				kernel -> ipc_base_address[ i ].data[ j ] = data[ j ];

			// set message timeout
			kernel -> ipc_base_address[ i ].ttl = kernel -> time_unit + KERNEL_IPC_ttl;

			// unlock access to IPC area
			kernel -> ipc_semaphore = UNLOCK;

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
		if( kernel -> time_unit > kernel -> ipc_base_address[ i ].ttl ) continue;	// no
	
		// message for us?
		if( kernel -> ipc_base_address[ i ].target != kernel -> task_pid() ) continue;	// no

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
	struct KERNEL_TASK_STRUCTURE *target = (struct KERNEL_TASK_STRUCTURE *) kernel_task_by_id( pid );

	// acquire space from target task
	uintptr_t target_pointer = KERNEL_EXEC_base_address + (kernel_memory_acquire_secured( target, pages ) << STD_SHIFT_PAGE);

	// connect memory space of parent process with child
	kernel_page_clang( (uintptr_t *) target -> cr3, source, target_pointer, pages, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | KERNEL_PAGE_FLAG_process | KERNEL_PAGE_FLAG_shared );

	// return the address of the first page in the collection
	return target_pointer;
}

void kernel_syscall_mouse( struct STD_SYSCALL_STRUCTURE_MOUSE *mouse ) {
	// return information about existing framebuffer
	mouse -> x	= kernel -> device_mouse_x;
	mouse -> y	= kernel -> device_mouse_y;
	mouse -> status	= kernel -> device_mouse_status;
}

void kernel_syscall_framebuffer_change( struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER *framebuffer ) {
	// process is allowed for modifications?
	if( kernel_task_pid() != kernel -> framebuffer_pid ) return;	// no

	// change new framebuffer owner
	kernel -> framebuffer_pid = framebuffer -> pid;
}

uint8_t kernel_syscall_ipc_receive_by_pid( uint8_t *data, int64_t pid ) {
	// scan whole IPC area
	for( uint64_t i = 0; i < KERNEL_IPC_limit; i++ ) {
		// message alive?
		if( kernel -> time_unit > kernel -> ipc_base_address[ i ].ttl ) continue;	// no
	
		// message for us?
		if( kernel -> ipc_base_address[ i ].target != kernel -> task_pid() ) continue;	// no

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
	// retrieve stream output of current task
	struct KERNEL_TASK_STRUCTURE *task = (struct KERNEL_TASK_STRUCTURE *) kernel_task_active();

	// stream closed or full?
	if( task -> stream_out -> flags & KERNEL_STREAM_FLAG_closed || task -> stream_out -> length_byte == STD_STREAM_SIZE_page << STD_SHIFT_PAGE ) return FALSE;	// yes

	// block access to stream modification
	while( __sync_val_compare_and_swap( &task -> stream_out -> semaphore, UNLOCK, LOCK ) );

	// amount of data inside stream after operation
	task -> stream_out -> length_byte += length;

	// can we fit it?
	if( length > (STD_STREAM_SIZE_page << STD_SHIFT_PAGE) - task -> stream_out -> length_byte ) {	// no
		// unlock stream access
		task -> stream_out -> semaphore = UNLOCK;

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
	task -> stream_out -> semaphore = UNLOCK;

	// string sended
	return TRUE;
}

uint64_t kernel_syscall_stream_in( uint8_t *target ) {
	// get the process output stream id
	struct KERNEL_TASK_STRUCTURE *task = (struct KERNEL_TASK_STRUCTURE *) kernel_task_active();

	// stream closed or empty?
	if( task -> stream_in -> flags & KERNEL_STREAM_FLAG_closed || ! task -> stream_in -> length_byte ) return EMPTY;	// yes

	// block access to stream modification
	while( __sync_val_compare_and_swap( &task -> stream_in -> semaphore, UNLOCK, LOCK ) );

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
	task -> stream_in -> semaphore = UNLOCK;

	// return amount of transferred data in Bytes
	return length;
}

uint16_t kernel_syscall_keyboard( void ) {
	// current task properties
	struct KERNEL_TASK_STRUCTURE *task = kernel_task_active();

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
	struct KERNEL_TASK_STRUCTURE *task = kernel_task_active();

	// stream properties
	struct KERNEL_STREAM_STRUCTURE *stream;

	// choose stream type
	if( stream_type == STD_STREAM_IN ) stream = task -> stream_in;
	else stream = task -> stream_out;

	// block access to stream modification
	while( __sync_val_compare_and_swap( &stream -> semaphore, UNLOCK, LOCK ) );

	// update stream meta only if is empty
	if( ! stream -> length_byte ) {
		// update meta data
		for( uint8_t i = 0; i < STD_STREAM_META_limit; i++ ) stream -> meta[ i ] = meta[ i ];

		// meta data is consistent
		stream -> flags &= ~KERNEL_STREAM_FLAG_modified;
	}

	// unlock stream modification
	stream -> semaphore = UNLOCK;
}

uint8_t kernel_syscall_stream_get( uint8_t *target, uint8_t stream_type ) {
	// current task properties
	struct KERNEL_TASK_STRUCTURE *task = kernel_task_active();

	// stream properties
	struct KERNEL_STREAM_STRUCTURE *stream;

	// choose stream type
	if( stream_type == STD_STREAM_IN ) stream = task -> stream_in;
	else stream = task -> stream_out;

	// block access to stream modification
	while( __sync_val_compare_and_swap( &stream -> semaphore, UNLOCK, LOCK ) );

	// retrieve meta data
	for( uint8_t i = 0; i < STD_STREAM_META_limit; i++ ) target[ i ] = stream -> meta[ i ];

	// meta data was up to date?
	uint8_t status = TRUE;
	if( stream -> flags & KERNEL_STREAM_FLAG_modified ) status = FALSE;	// nope

	// unlock stream modification
	stream -> semaphore = UNLOCK;

	// return meta data status
	return status;
}