/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_task( void ) {
	// default task limit (extendable)
	kernel -> task_limit = KERNEL_TASK_limit;

	// prepare area for Task entries
	kernel -> task_base_address = (struct KERNEL_TASK_STRUCTURE *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( kernel -> task_limit * sizeof( struct KERNEL_TASK_STRUCTURE ) ) >> STD_SHIFT_PAGE );

	// prepare area for APs
	kernel -> task_cpu_address = (struct KERNEL_TASK_STRUCTURE **) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( limine_smp_request.response -> cpu_count << STD_SHIFT_PTR ) >> STD_SHIFT_PAGE );

	// attach task switch routine to APIC timer interrupt handler
	kernel_idt_mount( KERNEL_IDT_IRQ_offset, KERNEL_IDT_TYPE_gate_interrupt, (uintptr_t) kernel_task_entry );

	// attach task switch routine to software interrupt handler
	kernel_idt_mount( 0x40, KERNEL_IDT_TYPE_isr, (uintptr_t) kernel_task_entry );

	// mark IRQ line as used
	kernel -> io_apic_irq_lines &= ~(1 << 0);

	//----------------------------------------------------------------------
	// we need to create first entry inside task queue for kernel itself
	// that entry will never be *active*, none of BSP/AP CPUs will ever run it
	// so why we still need it? each CPU exiting initialization state (file: ap.c)
	// will go stright into task selection procedure, and as we know (later)
	// all registers/flags needs to be stored somewhere before choosing next task
	// thats the purpose of kernel entry :)
	//----------------------------------------------------------------------

	// mark first entry of task queue as secured (in use)
	kernel -> task_base_address -> flags = STD_TASK_FLAG_secured;

	// kernel paging structure
	kernel -> task_base_address -> cr3 = (uintptr_t) kernel -> page_base_address;

	// set memory map of kernel
	kernel -> task_base_address -> memory_map = kernel -> memory_base_address;

	// prepare stream[s] for kernel
	struct KERNEL_STREAM_STRUCTURE *local_stream = kernel_stream();

	// as a kernel, both stream[s] are of type null
	local_stream -> flags = KERNEL_STREAM_FLAG_null;

	// assign stream[s] to kernel entry
	kernel -> task_base_address -> stream_out = local_stream;
	kernel -> task_base_address -> stream_in = local_stream;

	// each CPU needs to know which task he is currently executing
	// that information is stored on CPU list
	kernel -> task_cpu_address[ kernel_lapic_id() ] = kernel -> task_base_address;
}
