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

	// mark IRQ line as used
	kernel -> io_apic_irq_lines &= ~(1 << 0);

	// we need to create first entry inside task queue for kernel itself

	// although kernel will die a natural death on first task switch,
	// this entry is part of further initialization of system environment
	// so it MUST exist

	// kernel paging structure
	kernel -> task_base_address -> cr3 = (uintptr_t) kernel -> page_base_address;

	// mark first entry of task queue as secured (in use)
	kernel -> task_base_address -> flags = STD_TASK_FLAG_secured;

	// register memory map of kernel
	kernel -> task_base_address -> memory_map = kernel -> memory_base_address;

	// define memory semaphore location
	uint8_t *semaphore = (uint8_t *) kernel -> task_base_address -> memory_map + MACRO_PAGE_ALIGN_UP( kernel -> page_limit >> STD_SHIFT_8 ) - STD_SIZE_BYTE_byte;

	// unlock access to binary memory map
	MACRO_UNLOCK( *semaphore );

	// when BSP (Bootstrap Processor) will end with initialization of every system aspect,
	// he needs to know which is his current task entry point
	kernel -> task_cpu_address[ kernel_lapic_id() ] = kernel -> task_base_address;

	// show information about Task queue
	// kernel -> log( (uint8_t *) "Task queue base address 0x%X\n Entry[0] -> kernel environment initialization procedures.\n", (uintptr_t) kernel -> task_base_address );
}
