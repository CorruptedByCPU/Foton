/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions of standard library
	//----------------------------------------------------------------------
	#include	"../library/elf.h"
	#include	"../library/std.h"
	//----------------------------------------------------------------------
	// variables, structures, definitions of kernel
	//----------------------------------------------------------------------
	#include	"../kernel/config.h"
	#include	"../kernel/page.h"
	#include	"../kernel/task.h"
	#include	"../kernel/vfs.h"
	#include	"../kernel/library.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"./shredder/data.c"

void close( int64_t pid ) {
	// properties of task to close
	struct KERNEL_TASK_STRUCTURE *task = kernel -> task_by_id( pid );

	// mark me as not active anymore
	task -> flags &= ~KERNEL_TASK_FLAG_active;

	// find all childs of parent
	for( uint64_t i = 0; i < kernel -> task_limit; i++ ) {
		// ignore empty entries
		if( ! kernel -> task_base_address[ i ].flags ) continue;

		// am i parent of that child?
		if( kernel -> task_base_address[ i ].pid_parent == pid )
			// kill that bastard :)
			close( kernel -> task_base_address[ i ].pid );
	}

	// wait for release by any AP
	while( task -> flags & KERNEL_TASK_FLAG_exec );

	// close all network sockets belonged to process
	kernel -> network_socket_close_by_pid( task -> pid );

	// prepare thread for decomission?
	if( task -> flags & KERNEL_TASK_FLAG_thread ) {	// yes
		// Paging Table properties
		uint64_t *pml4 = (uint64_t *) (task -> cr3 | KERNEL_PAGE_logical);

		// remove entries of PML4 paging table not belonging to thread
		for( uint16_t i = 0; i < 512; i++ )
			// propertylessness?
			if( i != 223 && i != 511 ) pml4[ i ] = EMPTY;	// remove
	} else {
		// release both streams
		kernel -> stream_release( task -> stream_in );
		kernel -> stream_release( task -> stream_out );

		// release binary memory map of task
		kernel -> memory_release( (uintptr_t) task -> memory_map, MACRO_PAGE_ALIGN_UP( kernel -> page_limit >> STD_SHIFT_8 ) >> STD_SHIFT_PAGE );
	}

	// release paging structure of process
	kernel -> page_deconstruct( (uintptr_t *) task -> cr3, task -> page_type );

	// clean up
	task -> sleep	= EMPTY;
	task -> page	= EMPTY;
	task -> stack	= EMPTY;

	// task released
	task -> flags = EMPTY;

	// number of tasks in queue
	kernel -> task_count--;
}

void _entry( uintptr_t kernel_ptr ) {
	// preserve kernel structure pointer
	kernel = (struct KERNEL *) kernel_ptr;

	// infinite loop :)
	while( TRUE ) {
		// parse tasks table
		for( uint64_t i = 0; i < kernel -> task_limit; i++ ) {
			// task closed?
			if( kernel -> task_base_address[ i ].flags & STD_TASK_FLAG_close )
				// close process tree
				close( kernel -> task_base_address[ i ].pid );
		}

		// release AP time
		// kernel -> time_sleep( TRUE );
	}
}
