/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

// round robin queue type
void kernel_task( void ) {
	// increment microtime of BSP
	if( kernel_lapic_id() == EMPTY ) kernel -> time_unit++;

	// task properties
	struct KERNEL_TASK_STRUCTURE *current = kernel -> task_cpu_address[ kernel_lapic_id() ];

	// keep current top of stack pointer
	__asm__ volatile( "mov %%rsp, %0" : "=rm" (current -> rsp) );

	// current task execution stopped
	current -> flags &= ~STD_TASK_FLAG_exec;

	//----------------------------------------------------------------------

	// convert task pointer to entry id
	struct KERNEL_TASK_STRUCTURE *next = kernel_task_select( (uint64_t) (((uint64_t) current - (uint64_t) kernel -> task_base_address) / sizeof( struct KERNEL_TASK_STRUCTURE )) );

	//----------------------------------------------------------------------

	// reload paging tables for next task area
	__asm__ volatile( "mov %0, %%cr3" ::"r" (next -> cr3 & ~KERNEL_PAGE_logical) );

	// restore previous  stack pointer of next task
	__asm__ volatile( "movq %0, %%rsp" : "=rm" (next -> rsp) );

	// reload CPU cycle counter inside APIC controller
	kernel_lapic_reload();

	// accept current interrupt call
	kernel_lapic_accept();

	// first run of task?
	if( next -> flags & STD_TASK_FLAG_init ) {
		// disable init flag
		next -> flags &= ~STD_TASK_FLAG_init;

		// if module, pass a pointer to kernel environment specification
		if( next -> flags & STD_TASK_FLAG_module ) __asm__ volatile( "" :: "D" (kernel), "S" (EMPTY) );
		else {
			// retrieve from stack
			uint64_t *arg = (uint64_t *) (next -> rsp + offsetof( struct KERNEL_IDT_STRUCTURE_RETURN, rsp ) );
			uint64_t *argc = (uint64_t *) *arg;

			// length of string
			__asm__ volatile( "" :: "D" (*argc) );
			
			// pointer to string
			__asm__ volatile( "" :: "S" (*arg + 0x08) );
		}

		// reset FPU state
		__asm__ volatile( "fninit" );

		// kernel guarantee clean registers at first run
		__asm__ volatile( "xor %r15, %r15\nxor %r14, %r14\nxor %r13, %r13\nxor %r12, %r12\nxor %r11, %r11\nxor %r10, %r10\nxor %r9, %r9\nxor %r8, %r8\nxor %ebp, %ebp\nxor %edx, %edx\nxor %ecx, %ecx\nxor %ebx, %ebx\nxor %eax, %eax\n" );

		// run task in exception mode
		__asm__ volatile( "iretq" );
	}
}

struct KERNEL_TASK_STRUCTURE *kernel_task_active( void ) {
	// from list of active tasks on individual logical processors
	// select currently processed position relative to current logical processor
	if( ! kernel -> task_cpu_address ) return EMPTY;
	return (struct KERNEL_TASK_STRUCTURE *) kernel -> task_cpu_address[ kernel_lapic_id() ];
}

struct KERNEL_TASK_STRUCTURE *kernel_task_add( uint8_t *name, uint8_t length ) {
	// deny modification of job queue
	MACRO_LOCK( kernel -> task_add_semaphore );

	// find an free entry
	for( uint64_t i = 0; i < kernel -> task_limit; i++ ) {
		// free entry?
		if( kernel -> task_base_address[ i ].flags ) continue;	// no

		// mark entry as "in use""
		kernel -> task_base_address[ i ].flags = STD_TASK_FLAG_secured;

		// ID of new job
		kernel -> task_base_address[ i ].pid = ++kernel -> task_id;

		// ID of its parent
		kernel -> task_base_address[ i ].pid_parent = kernel_task_pid();

		// reset number of characters representing process name
		kernel -> task_base_address[ i ].name_length = length;

		// set process name
		if( kernel -> task_base_address[ i ].name_length > KERNEL_TASK_NAME_limit ) kernel -> task_base_address[ i ].name_length = KERNEL_TASK_NAME_limit;
		for( uint16_t n = 0; n < kernel -> task_base_address[ i ].name_length; n++ ) kernel -> task_base_address[ i ].name[ n ] = name[ n ]; kernel -> task_base_address[ i ].name[ kernel -> task_base_address[ i ].name_length ] = STD_ASCII_TERMINATOR;


		// number of jobs in queue
		kernel -> task_count++;

		// free access to job queue
		MACRO_UNLOCK( kernel -> task_add_semaphore );

		// new task initiated
		return (struct KERNEL_TASK_STRUCTURE *) &kernel -> task_base_address[ i ];
	}

	// free access to job queue
	MACRO_UNLOCK( kernel -> task_add_semaphore );

	// no free entry
	return EMPTY;
}

int64_t kernel_task_pid( void ) {
	// based on ID of active BS/A processor
	// get from list of active jobs, number of current record in job queue
	struct KERNEL_TASK_STRUCTURE *task = (struct KERNEL_TASK_STRUCTURE *) kernel -> task_cpu_address[ kernel_lapic_id() ];

	// get ID of process
	return task -> pid;
}

struct KERNEL_TASK_STRUCTURE *kernel_task_select( uint64_t i ) {
	// block possibility of modifying tasks, only 1 CPU at a time
	MACRO_LOCK( kernel -> task_cpu_semaphore );

	// search until found
	while( TRUE ) {
		// search in task queue for a ready-to-do task
		for( ++i ; i < kernel -> task_limit; i++ ) {
			// task available for processing?
			if( kernel -> task_base_address[ i ].flags & STD_TASK_FLAG_active && ! (kernel -> task_base_address[ i ].flags & STD_TASK_FLAG_exec) ) {	// yes
				// mark task as performed by current logical processor
				kernel -> task_base_address[ i ].flags |= STD_TASK_FLAG_exec;

				// inform BS/A about task to execute as next
				kernel -> task_cpu_address[ kernel_lapic_id() ] = &kernel -> task_base_address[ i ];

				// unlock access to modification of tasks
				MACRO_UNLOCK( kernel -> task_cpu_semaphore );

				// return address of selected task from queue
				return &kernel -> task_base_address[ i ];
			}
		}

		// start from begining
		i = 0;
	}
}

struct KERNEL_TASK_STRUCTURE *kernel_task_by_id( int64_t pid ) {
	// entry ID
	for( uint64_t i = 0; i < kernel -> task_limit; i++ )
		// found?
		if( pid == kernel -> task_base_address[ i ].pid ) return (struct KERNEL_TASK_STRUCTURE *) &kernel -> task_base_address[ i ];	// yes

	// ID not found
	return EMPTY;
}