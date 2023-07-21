/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_idt() {
	// prepare area for Interrupt Descriptor Table
	kernel -> idt_header.base_address = (struct KERNEL_IDT_STRUCTURE_ENTRY *) kernel_memory_alloc( 1 );

	// attach CPU exception handlers
	kernel_idt_mount( 0, KERNEL_IDT_TYPE_exception, (uintptr_t) &kernel_idt_exception_divide_by_zero );
	kernel_idt_mount( 1, KERNEL_IDT_TYPE_exception, (uintptr_t) &kernel_idt_exception_debug );
	kernel_idt_mount( 3, KERNEL_IDT_TYPE_exception, (uintptr_t) &kernel_idt_exception_breakpoint );
	kernel_idt_mount( 4, KERNEL_IDT_TYPE_exception, (uintptr_t) &kernel_idt_exception_overflow );
	kernel_idt_mount( 5, KERNEL_IDT_TYPE_exception, (uintptr_t) &kernel_idt_exception_boud_range_exceeded );
	kernel_idt_mount( 6, KERNEL_IDT_TYPE_exception, (uintptr_t) &kernel_idt_exception_invalid_opcode );
	kernel_idt_mount( 7, KERNEL_IDT_TYPE_exception, (uintptr_t) &kernel_idt_exception_device_not_available );
	kernel_idt_mount( 8, KERNEL_IDT_TYPE_exception, (uintptr_t) &kernel_idt_exception_double_fault );
	kernel_idt_mount( 9, KERNEL_IDT_TYPE_exception, (uintptr_t) &kernel_idt_exception_coprocessor_segment_overrun );
	kernel_idt_mount( 10, KERNEL_IDT_TYPE_exception, (uintptr_t) &kernel_idt_exception_invalid_tss );
	kernel_idt_mount( 11, KERNEL_IDT_TYPE_exception, (uintptr_t) &kernel_idt_exception_segment_not_present );
	kernel_idt_mount( 12, KERNEL_IDT_TYPE_exception, (uintptr_t) &kernel_idt_exception_stack_segment_fault );
	kernel_idt_mount( 13, KERNEL_IDT_TYPE_exception, (uintptr_t) &kernel_idt_exception_general_protection_fault );
	kernel_idt_mount( 14, KERNEL_IDT_TYPE_exception, (uintptr_t) &kernel_idt_exception_page_fault );
	kernel_idt_mount( 16, KERNEL_IDT_TYPE_exception, (uintptr_t) &kernel_idt_exception_x87_floating_point );
	kernel_idt_mount( 17, KERNEL_IDT_TYPE_exception, (uintptr_t) &kernel_idt_exception_alignment_check );
	kernel_idt_mount( 18, KERNEL_IDT_TYPE_exception, (uintptr_t) &kernel_idt_exception_machine_check );
	kernel_idt_mount( 19, KERNEL_IDT_TYPE_exception, (uintptr_t) &kernel_idt_exception_simd_floating_point );
	kernel_idt_mount( 20, KERNEL_IDT_TYPE_exception, (uintptr_t) &kernel_idt_exception_virtualization );
	kernel_idt_mount( 21, KERNEL_IDT_TYPE_exception, (uintptr_t) &kernel_idt_exception_control_protection );
	kernel_idt_mount( 28, KERNEL_IDT_TYPE_exception, (uintptr_t) &kernel_idt_exception_hypervisor_injection );
	kernel_idt_mount( 29, KERNEL_IDT_TYPE_exception, (uintptr_t) &kernel_idt_exception_vmm_communication );
	kernel_idt_mount( 30, KERNEL_IDT_TYPE_exception, (uintptr_t) &kernel_idt_exception_security );

	// attach default hardware interrupt handler
	for( uint8_t i = 32; i < 48; i++ )
		kernel_idt_mount( i, KERNEL_IDT_TYPE_irq, (uintptr_t) &kernel_idt_interrupt_default );

	// attach interrupt handler for "spurious interrupt"
	kernel_idt_mount( 255, KERNEL_IDT_TYPE_irq, (uintptr_t) &kernel_idt_spurious_interrupt );

	// configure header of Interrupt Descriptor Table
	kernel -> idt_header.limit = STD_PAGE_byte;

	// load new Interrupt Descriptor Table
	__asm__ volatile( "lidt (%0)" :: "r" (&kernel -> idt_header) );

	// show IDT properties
	lib_terminal_printf( &kernel_terminal, "IDT base address 0x%X\n\n", kernel -> idt_header.base_address );

	// enable interrupt handling
	__asm__ volatile( "sti" );
}
