/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

// round robin queue type
void kernel_task_switch( void ) {
}

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

		// ID of its parent
		kernel -> task_base_address[ i ].parent = kernel_task_current() -> pid;

		// reset number of characters representing process name
		kernel -> task_base_address[ i ].name_limit = limit;

		// set process name
		if( limit > KERNEL_TASK_NAME_limit ) kernel -> task_base_address[ i ].name_limit = KERNEL_TASK_NAME_limit; kernel -> task_base_address[ i ].name = (uint8_t *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( KERNEL_TASK_NAME_limit ) >> STD_SHIFT_PAGE );
		for( uint16_t n = INIT; n < kernel -> task_base_address[ i ].name_limit; n++ ) kernel -> task_base_address[ i ].name[ n ] = name[ n ]; kernel -> task_base_address[ i ].name[ kernel -> task_base_address[ i ].name_limit ] = STD_ASCII_TERMINATOR;

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

struct KERNEL_STRUCTURE_TASK *kernel_task_current( void ) {
	// from list of active tasks of individual application processors
	// select currently processed position relative to current application processor
	return (struct KERNEL_STRUCTURE_TASK *) &kernel -> task_base_address[ kernel -> task_ap_address[ kernel_apic_id() ] ];
}
