/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_task( void ) {
	// prepare area for tasks
	kernel -> task_limit = KERNEL_TASK_limit;
	kernel -> task_base_address = (struct KERNEL_STRUCTURE_TASK *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( kernel -> task_limit * sizeof( struct KERNEL_STRUCTURE_TASK ) ) >> STD_SHIFT_PAGE );

	//----------------------------------------------------------------------
	// we need to create first entry inside task queue for kernel itself
	// that entry will never be *active*, none of BS/A CPUs will ever run it
	// so why we still need it? each CPU exiting initialization state (file: ap.c)
	// will go stright into task selection procedure, and as we know (later)
	// all registers/flags needs to be stored somewhere before choosing next task
	// thats the purpose of kernel entry :)
	//----------------------------------------------------------------------

	// id of BS Processor
	uint8_t id = kernel_apic_id();

	// mark first entry of task queue as secured (in use)
	kernel -> task_base_address[ id ].flags = STD_TASK_FLAG_secured;

	// prepare area for APs
	kernel -> task_ap_address = (uint64_t *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( limine_smp_request.response -> cpu_count << STD_SHIFT_PTR ) >> STD_SHIFT_PAGE );

	// each CPU needs to know which task he was currently executing
	// that information is stored on AP list

	// BS CPU is currently executing Kernel initialization routines
	kernel -> task_ap_address[ id ] = 0;

	//----------------------------------------------------------------------
	// every child will inherit this parameters:
	//----------------------------------------------------------------------

	// storage id and directory knot
	kernel -> task_base_address[ kernel -> task_ap_address[ id ] ].storage = 0;

	// root directory of that storage
	kernel -> task_base_address[ kernel -> task_ap_address[ id ] ].directory = kernel -> storage_base_address[ kernel -> task_base_address[ kernel -> task_ap_address[ id ] ].storage ].block;

	//----------------------------------------------------------------------

	// attach task switch routine to APIC timer - interrupt handler
	kernel_idt_attach( KERNEL_IDT_IRQ_offset, KERNEL_IDT_TYPE_gate_interrupt, (uintptr_t) kernel_task );

	// attach task switch routine to Software - interrupt handler
	kernel_idt_attach( KERNEL_TASK_IRQ_software, KERNEL_IDT_TYPE_isr, (uintptr_t) kernel_task );
}
