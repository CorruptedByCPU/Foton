/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions of kernel
	//----------------------------------------------------------------------
	#include	"../kernel/idt.h"
	#include	"../kernel/task.h"
	#include	"../kernel/config.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"./shredder/data.c"

void close( int64_t pid ) {
	MACRO_DEBUF();

	// properties of task to close
	struct KERNEL_STRUCTURE_TASK *task = kernel -> task_by_id( pid );

	// // find all childs of parent
	// for( uint64_t i = 0; i < kernel -> task_limit; i++ ) {
	// 	// ignore empty entries
	// 	if( ! kernel -> task_base_address[ i ].flags ) continue;

	// 	// am i parent of that child?
	// 	if( kernel -> task_base_address[ i ].pid_parent == pid )
	// 		// bastard :)
	// 		close( kernel -> task_base_address[ i ].pid );
	// }

	// close all network sockets belonged to process
	// kernel -> network_socket_close_by_pid( task -> pid );

	// prepare thread for decomission?
	// if( task -> flags & KERNEL_TASK_FLAG_thread ) {	// yes
	// 	// Paging Table properties
	// 	uint64_t *pml4 = (uint64_t *) (task -> cr3 | KERNEL_MEMORY_mirror);

	// 	// remove entries of PML4 paging table not belonging to thread
	// 	for( uint16_t i = 0; i < 512; i++ )
	// 		// propertylessness?
	// 		if( i != 223 && i != 511 ) pml4[ i ] = EMPTY;	// remove
	// } else {
		// release both streams
		// kernel -> stream_release( task -> stream_in );
		// kernel -> stream_release( task -> stream_out );

		// release binary memory map of task
		kernel -> memory_release( (uintptr_t) task -> memory, MACRO_PAGE_ALIGN_UP( (kernel -> page_limit >> STD_SHIFT_8) + TRUE ) >> STD_SHIFT_PAGE );
	// }

	// release paging structure of process
	kernel -> page_deconstruct( (uint64_t *) task -> cr3, task -> type );

	// clean up
	// task -> sleep	= EMPTY;
	task -> page	= EMPTY;
	task -> stack_page	= EMPTY;

	// task released
	task -> flags = EMPTY;
}

void _entry( uintptr_t kernel_ptr ) {
	// preserve kernel structure pointer
	kernel = (struct KERNEL *) kernel_ptr;

	// infinite loop :)
	while( TRUE ) {
		// check every entry
		for( uint64_t i = TRUE; i < kernel -> task_limit; i++ )
			// task ready for close?
			if( kernel -> task_base_address[ i ].flags & STD_TASK_FLAG_close )
				// close process tree
				close( kernel -> task_base_address[ i ].pid );

		// release CPU time
		kernel -> time_sleep( TRUE );
	}
}
