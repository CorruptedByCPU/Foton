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
	#include	"../kernel/page.h"
	#include	"../kernel/task.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"./shredder/data.c"

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

				// release both streams
				kernel -> stream_release( kernel -> task_base_address[ i ].stream_in );
				kernel -> stream_release( kernel -> task_base_address[ i ].stream_out );

				// prepare thread for decomission?
				if( kernel -> task_base_address[ i ].flags & KERNEL_TASK_FLAG_thread ) {	// yes
					// thread is owning only stacks areas

					// Paging Table properties
					uint64_t *pml4 = (uint64_t *) (kernel -> task_base_address[ i ].cr3 | KERNEL_PAGE_logical);

					// remove entries of PML4 paging table
					for( uint16_t i = 0; i < 512; i++ )
						// propertylessness?
						if( i != 223 && i != 511 ) pml4[ i ] = EMPTY;	// remove
				} else
					// release binary memory map of task
					kernel -> memory_release( (uintptr_t) kernel -> task_base_address[ i ].memory_map, MACRO_PAGE_ALIGN_UP( kernel -> page_limit >> STD_SHIFT_8 ) >> STD_SHIFT_PAGE );

				// release paging structure of task
				kernel -> page_deconstruct( (uintptr_t *) kernel -> task_base_address[ i ].cr3 );

				// task released
				kernel -> task_base_address[ i ].flags = EMPTY;

				// number of tasks in queue
				kernel -> task_count--;
			}
		}
	}
}
