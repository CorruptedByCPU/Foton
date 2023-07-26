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

	// lock access to below routine, only 1 CPU at a time
	while( __sync_val_compare_and_swap( &kernel -> task_cpu_semaphore, UNLOCK, LOCK ) );

	// convert task pointer to entry id
	uint64_t entry_id = (uint64_t) kernel -> task_cpu_address[ current_cpu_id ] - (uint64_t) kernel -> task_base_address;

	// search until found
	while( TRUE ) {
		// end of task queue?
		if( ++entry_id >= kernel -> task_limit ) {
			// start from begining of task queue
			entry_id = 0; continue;
		}
			
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
	}

	// unlock access
	kernel -> task_cpu_semaphore = UNLOCK;

	//----------------------------------------------------------------------

	// reload paging tables for new task area
	__asm__ volatile( "mov %0, %%cr3" ::"r" (kernel -> task_cpu_address[ current_cpu_id ] -> cr3 & ~STD_PAGE_mask) );

	// restore previous  stack pointer of new task
	__asm__ volatile( "movq %0, %%rsp" : "=rm" (kernel -> task_cpu_address[ current_cpu_id ] -> rsp) );

	// reload CPU cycle counter inside APIC controller
	kernel_lapic_reload();

	// accept current interrupt call
	kernel_lapic_accept();
}