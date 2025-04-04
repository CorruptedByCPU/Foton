/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

// round robin queue type
void kernel_task_switch( void ) {
}

struct KERNEL_STRUCTURE_TASK *kernel_task_current( void ) {
	// from list of active tasks of individual application processors
	// select currently processed position relative to current application processor
	return (struct KERNEL_STRUCTURE_TASK *) &kernel -> task_base_address[ kernel -> task_ap_address[ kernel_apic_id() ] ];
}
