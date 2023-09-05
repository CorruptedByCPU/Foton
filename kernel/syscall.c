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
	if( ! __sync_val_compare_and_swap( &kernel -> framebuffer_owner_pid, EMPTY, kernel_task_pid() ) )
		// approved
		framebuffer -> base_address = (uint32_t *) kernel_memory_share( (uintptr_t) kernel -> framebuffer_base_address & ~KERNEL_PAGE_PML5_mask, MACRO_PAGE_ALIGN_UP( kernel -> framebuffer_pitch_byte * kernel -> framebuffer_height_pixel) >> STD_SHIFT_PAGE );

	// return information about framebuffer owner
	framebuffer -> owner_pid = kernel -> framebuffer_owner_pid;
}

uintptr_t kernel_syscall_memory_alloc( uint64_t page ) {
	// task properties
	struct KERNEL_TASK_STRUCTURE *task = (struct KERNEL_TASK_STRUCTURE *) kernel_task_active();

	// acquire N continuous pages
	uintptr_t allocated = EMPTY;
	if( (allocated = kernel_memory_acquire( task -> memory_map, page )) ) {
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

uint64_t kernel_syscall_uptime() {
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

int64_t kernel_syscall_pid() {
	// task properties
	struct KERNEL_TASK_STRUCTURE *task = kernel_task_active();

	// return task ID
	return task -> pid;
}

int64_t	kernel_syscall_exec( uint8_t *name, uint64_t length ) {
	// return new process ID
	return kernel_exec( name, length );
}

uint8_t kernel_syscall_pid_check( int64_t pid ) {
	// find an entry with selected ID
	for( uint64_t i = 0; i < kernel -> task_limit; i++ )
		// found?
		if( kernel -> task_base_address[ i ].pid == pid ) return TRUE;	// yes

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
			// entry found?
			if( kernel -> ipc_base_address[ i ].ttl < kernel -> time_unit ) {
				// set message timeout
				kernel -> ipc_base_address[ i ].ttl = kernel -> time_unit + KERNEL_IPC_ttl;

				// send to
				kernel -> ipc_base_address[ i ].target = pid;

				// sent from
				kernel -> ipc_base_address[ i ].source = kernel -> task_pid();

				// load data into message
				for( uint8_t j = 0; j < STD_IPC_SIZE_byte; j++ )
					kernel -> ipc_base_address[ i ].data[ j ] = data[ j ];

				// unlock access to IPC area
				kernel -> ipc_semaphore = UNLOCK;

				// message sent
				return;
			}
		}
	}
}

int64_t kernel_syscall_ipc_receive( uint8_t *data ) {
	// deny access, only one logical processor at a time
	while( __sync_val_compare_and_swap( &kernel -> ipc_semaphore, UNLOCK, LOCK ) );

	// scan whole IPC area
	for( uint64_t i = 0; i < KERNEL_IPC_limit; i++ ) {
		// message for us and alive?
		if( kernel -> ipc_base_address[ i ].target == kernel -> task_pid() && kernel -> ipc_base_address[ i ].ttl > kernel -> time_unit ) {
			// load data into message
			for( uint8_t j = 0; j < STD_IPC_SIZE_byte; j++ )
				data[ j ] = kernel -> ipc_base_address[ i ].data[ j ];

			// retrieve message source
			int64_t source = kernel -> ipc_base_address[ i ].source;

			// mark entry as free
			kernel -> ipc_base_address[ i ].ttl = EMPTY;

			// unlock access to IPC area
			kernel -> ipc_semaphore = UNLOCK;

			// message from
			return source;
		}
	}

	// unlock access to IPC area
	kernel -> ipc_semaphore = UNLOCK;

	// no message for process
	return EMPTY;
}