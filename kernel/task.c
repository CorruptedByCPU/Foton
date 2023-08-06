/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

// round robin queue type
void kernel_task( void ) {
	// retieve current CPU id
	uint64_t current_cpu_id = kernel_lapic_id();

	// keep current top of stack pointer
	__asm__ volatile( "mov %%rsp, %0" : "=rm" (kernel -> task_cpu_address[ current_cpu_id ] -> rsp) );

	// current task execution stopped
	kernel -> task_cpu_address[ current_cpu_id ] -> flags &= ~KERNEL_TASK_FLAG_exec;

	//----------------------------------------------------------------------

	// convert task pointer to entry id
	uint64_t entry_id = (uint64_t) kernel -> task_cpu_address[ current_cpu_id ] - (uint64_t) kernel -> task_base_address;

	// search until found
	while( TRUE ) {
		// end of task queue?
		if( ++entry_id >= kernel -> task_limit ) {
			// start from begining of task queue
			entry_id = 0; continue;
		}

		// lock access to below routine, only 1 CPU at a time
		while( __sync_val_compare_and_swap( &kernel -> task_cpu_semaphore, UNLOCK, LOCK ) );

		// if task is active, and no one is parsing it
		if( kernel -> task_base_address[ entry_id ].flags & KERNEL_TASK_FLAG_active && ! (kernel -> task_base_address[ entry_id ].flags & KERNEL_TASK_FLAG_exec) ) {
			// of course, if task is not sleeping too :)
			if( kernel -> task_base_address[ entry_id ].sleep > kernel -> hpet_miliseconds ) continue;	// sleeping

			// mark job as executed
			kernel -> task_base_address[ entry_id ].flags |= KERNEL_TASK_FLAG_exec;

			// update task assigned to cpu
			kernel -> task_cpu_address[ current_cpu_id ] = &kernel -> task_base_address[ entry_id ];

			// entry found
			break;
		}

		// unlock access
		kernel -> task_cpu_semaphore = UNLOCK;
	}

	// unlock access
	kernel -> task_cpu_semaphore = UNLOCK;

	//----------------------------------------------------------------------

	// reload paging tables for new task area
	__asm__ volatile( "mov %0, %%cr3" ::"r" (kernel -> task_cpu_address[ current_cpu_id ] -> cr3 & ~KERNEL_PAGE_logical) );

	// restore previous  stack pointer of new task
	__asm__ volatile( "movq %0, %%rsp" : "=rm" (kernel -> task_cpu_address[ current_cpu_id ] -> rsp) );

	// reload CPU cycle counter inside APIC controller
	kernel_lapic_reload();

	// accept current interrupt call
	kernel_lapic_accept();

	// first run of the task?
	if( kernel -> task_cpu_address[ current_cpu_id ] -> flags & KERNEL_TASK_FLAG_init ) {
		// disable init flag
		kernel -> task_cpu_address[ current_cpu_id ] -> flags &= ~KERNEL_TASK_FLAG_init;

		// if daemon, pass a pointer to the kernel environment specification
		if( kernel -> task_cpu_address[ current_cpu_id ] -> flags & KERNEL_TASK_FLAG_service ) __asm__ volatile( "" :: "D" (kernel), "S" (EMPTY) );
		else {
			// retrieve from stack
			uint64_t *arg = (uint64_t *) (kernel -> task_cpu_address[ current_cpu_id ] -> rsp + offsetof( struct KERNEL_IDT_STRUCTURE_RETURN, rsp ) );
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

		// run the task in exception mode
		__asm__ volatile( "iretq" );
	}
}

struct KERNEL_TASK_STRUCTURE *kernel_task_add( uint8_t *name, uint8_t length ) {
	// deny modification of job queue
	while( __sync_val_compare_and_swap( &kernel -> task_cpu_semaphore, UNLOCK, LOCK ) );

	// find an free entry
	for( uint64_t i = 0; i < KERNEL_TASK_limit; i++ ) {
		// free entry?
		if( kernel -> task_base_address[ i ].flags ) continue;	// no

		// mark entry as "in use""
		kernel -> task_base_address[ i ].flags = KERNEL_TASK_FLAG_secured;

		// ID of new job
		kernel -> task_base_address[ i ].pid = ++kernel -> task_id;

		// number of characters representing process name
		kernel -> task_base_address[ i ].length = length;

		// set process name
		if( length > KERNEL_TASK_NAME_limit ) length = KERNEL_TASK_NAME_limit;
		for( uint16_t n = 0; n < length; n++ ) kernel -> task_base_address[ i ].name[ n ] = name[ n ]; kernel -> task_base_address[ i ].name[ length ] = EMPTY;

		// number of jobs in queue
		kernel -> task_count++;

		// free access to job queue
		kernel -> task_cpu_semaphore = UNLOCK;

		// new task initiated
		return (struct KERNEL_TASK_STRUCTURE *) &kernel -> task_base_address[ i ];
	}

	// free access to job queue
	kernel -> task_cpu_semaphore = UNLOCK;

	// no free entry
	return EMPTY;
}

int64_t kernel_task_pid() {
	// based on ID of active BS/A processor
	// get from list of active jobs, number of current record in job queue
	struct KERNEL_TASK_STRUCTURE *task = (struct KERNEL_TASK_STRUCTURE *) kernel -> task_cpu_address[ kernel_lapic_id() ];

	// get ID of process
	return task -> pid;
}