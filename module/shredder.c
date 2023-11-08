/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions of standard library
	//----------------------------------------------------------------------
	#include	"../library/std.h"
	//----------------------------------------------------------------------
	// variables, structures, definitions of kernel
	//----------------------------------------------------------------------
	#include	"../kernel/config.h"
	#include	"../kernel/task.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"./trash/data.c"

void sheet( int64_t pid ) {
	// find all childs of parent
	for( uint64_t i = 0; i < kernel -> task_limit; i++ ) {
		// am i parent of that?
		if( kernel -> task_base_address[ i ].pid_parent ) {
			// close all childs of that parent :)
			sheet( kernel -> task_base_address[ i ].pid );

			// mark child as not active anymore
			kernel -> task_base_address[ i ].flags &= ~KERNEL_TASK_FLAG_active;

			// wait for release by any AP
			while( kernel -> task_base_address[ i ].flags & KERNEL_TASK_FLAG_exec );

			// mark child as closed
			kernel -> task_base_address[ i ].flags |= KERNEL_TASK_FLAG_close;
		}
	}
}

void _entry( uintptr_t kernel_ptr ) {
	// preserve kernel structure pointer
	kernel = (struct KERNEL *) kernel_ptr;

	// infinite loop :)
	while( TRUE ) {
		// parse tasks table
		for( uint64_t i = 0; i < kernel -> task_limit; i++ ) {
			// task closed?
			if( kernel -> task_base_address[ i ].flags & KERNEL_TASK_FLAG_close ) {	// yes
				// close all childs of this task
				sheet( kernel -> task_base_address[ i ].pid );
			}
		}
	}
}
