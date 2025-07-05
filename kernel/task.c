/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct KERNEL_STRUCTURE_TASK *kernel_task_add( uint8_t *name, uint16_t limit ) {
	// deny modification of task queue
	MACRO_LOCK( kernel -> task_lock );

	// find an available entry
	for( uint64_t i = 0; i < kernel -> task_limit; i++ ) {
		// available?
		if( kernel -> task_base_address[ i ].flags ) continue;	// no

		// reserve
		kernel -> task_base_address[ i ].flags = STD_TASK_FLAG_secured;

		// ID of new task
		kernel -> task_base_address[ i ].pid = ++kernel -> task_id;

		// reset number of characters representing process name
		kernel -> task_base_address[ i ].name_limit = limit;

		// set process name
		if( limit > KERNEL_TASK_NAME_limit ) kernel -> task_base_address[ i ].name_limit = KERNEL_TASK_NAME_limit; kernel -> task_base_address[ i ].name = (uint8_t *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( KERNEL_TASK_NAME_limit ) >> STD_SHIFT_PAGE );
		for( uint16_t n = 0; n < kernel -> task_base_address[ i ].name_limit; n++ ) kernel -> task_base_address[ i ].name[ n ] = name[ n ]; kernel -> task_base_address[ i ].name[ kernel -> task_base_address[ i ].name_limit ] = STD_ASCII_TERMINATOR;

		// parent processs properties
		struct KERNEL_STRUCTURE_TASK *parent = kernel_task_current();

		// ID of its parent
		kernel -> task_base_address[ i ].parent = parent -> pid;

		// inherit storage and directory
		kernel -> task_base_address[ i ].storage = parent -> storage;
		kernel -> task_base_address[ i ].directory = parent -> directory;

		// unlock
		MACRO_UNLOCK( kernel -> task_lock );

		// new task created
		return (struct KERNEL_STRUCTURE_TASK *) &kernel -> task_base_address[ i ];
	}

	// unlock
	MACRO_UNLOCK( kernel -> task_lock );

	// no available entry
	return EMPTY;
}

struct KERNEL_STRUCTURE_TASK *kernel_task_by_id( uint64_t pid ) {
	// entry ID
	for( uint64_t i = TRUE; i < kernel -> task_limit; i++ )
		// found?
		if( pid == kernel -> task_base_address[ i ].pid ) return (struct KERNEL_STRUCTURE_TASK *) &kernel -> task_base_address[ i ];	// yes

	// ID not found
	return EMPTY;
}

struct KERNEL_STRUCTURE_TASK *kernel_task_current( void ) {
	// from list of active tasks of individual application processors
	// select currently processed position relative to current application processor
	return (struct KERNEL_STRUCTURE_TASK *) &kernel -> task_base_address[ kernel -> task_ap_address[ kernel_apic_id() ] ];
}

struct KERNEL_STRUCTURE_TASK *kernel_task_select( uint64_t i ) {
	// search until found
	while( TRUE ) {
		// search in task queue for a ready-to-do task
		for( ++i ; i < kernel -> task_limit; i++ ) {
			// task available for processing?
			if( ! (kernel -> task_base_address[ i ].flags & STD_TASK_FLAG_active) ) continue;	// no
			
			// task already taken by different AP?
			if( kernel -> task_base_address[ i ].flags & STD_TASK_FLAG_exec ) continue;	// yes
	
			// mark task as performed by current logical processor
			kernel -> task_base_address[ i ].flags |= STD_TASK_FLAG_exec;

			// inform BS/A about task to execute as next
			kernel -> task_ap_address[ kernel_apic_id() ] = i;

			// return address of selected task from queue
			return (struct KERNEL_STRUCTURE_TASK *) &kernel -> task_base_address[ i ];
		}

		// start from begining
		i = 0;
	}
}

// round robin queue type
void kernel_task_switch( void ) {
	// lock function
	MACRO_LOCK( kernel -> task_lock_ap );

	// current task properties
	struct KERNEL_STRUCTURE_TASK *current = kernel_task_current();

	// keep current top of stack pointer
	__asm__ volatile( "mov %%rsp, %0" : "=rm" (current -> rsp) );

	// current task execution stopped
	current -> flags &= ~STD_TASK_FLAG_exec;

	//----------------------------------------------------------------------

	// select another process
	struct KERNEL_STRUCTURE_TASK *next = kernel_task_select( (uint64_t) (((uint64_t) current - (uint64_t) kernel -> task_base_address) / sizeof( struct KERNEL_STRUCTURE_TASK )) );

	//----------------------------------------------------------------------

	// reload environment paging array
	__asm__ volatile( "mov %0, %%cr3" ::"r" ((uintptr_t) next -> cr3 & ~KERNEL_MEMORY_mirror) );

	// restore previous stack pointer of next task
	__asm__ volatile( "movq %0, %%rsp" : "=rm" (next -> rsp) );

	// unlock function
	MACRO_UNLOCK( kernel -> task_lock_ap );

	// reload CPU cycle counter inside APIC controller
	kernel_apic_reload();

	// accept current interrupt call
	kernel_apic_accept();

	// first run of task?
	if( next -> flags & STD_TASK_FLAG_init ) {
		// disable init flag
		next -> flags &= ~STD_TASK_FLAG_init;

		// if module, pass a pointer to kernel environment specification
		if( next -> flags & STD_TASK_FLAG_module ) __asm__ volatile( "" :: "D" (kernel), "S" (EMPTY) );
		else {
			// retrieve from stack
			uint64_t *argv = (uint64_t *) (next -> rsp + offsetof( struct KERNEL_STRUCTURE_IDT_RETURN, rsp ) );
			uint64_t *argc = (uint64_t *) *argv;

			// length of string
			__asm__ volatile( "" :: "D" (*argc) );
			
			// pointer to string
			__asm__ volatile( "" :: "S" (*argv + 0x08) );
		}

		// reset FPU state
		__asm__ volatile( "fninit" );

		// kernel guarantee clean registers at first run
		__asm__ volatile( "xor %r15, %r15\nxor %r14, %r14\nxor %r13, %r13\nxor %r12, %r12\nxor %r11, %r11\nxor %r10, %r10\nxor %r9, %r9\nxor %r8, %r8\nxor %ebp, %ebp\nxor %edx, %edx\nxor %ecx, %ecx\nxor %ebx, %ebx\nxor %eax, %eax\n" );

		// run task in exception mode
		__asm__ volatile( "iretq" );
	}
}
