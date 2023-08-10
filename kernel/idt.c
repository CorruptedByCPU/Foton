/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_idt_mount( uint8_t id, uint16_t type, uintptr_t address ) {
	// interrupt type
	kernel -> idt_header.base_address[ id ].type = type;

	// address of code descriptor that runs procedure
	kernel -> idt_header.base_address[ id ].gdt_descriptor = sizeof( ( ( struct KERNEL_GDT_STRUCTURE *) 0 ) -> cs_ring0 );

	// address of exception handler
	kernel -> idt_header.base_address[ id ].base_low = (uint16_t) address;
	kernel -> idt_header.base_address[ id ].base_middle = (uint16_t) (address >> 16);
	kernel -> idt_header.base_address[ id ].base_high = (uint32_t) (address >> 32);
}

void kernel_idt_exception( struct KERNEL_IDT_STRUCTURE_EXCEPTION *exception ) {
	// show type of exception
	lib_terminal_printf( &kernel_terminal, (uint8_t *) "Exception: " );
	switch( exception -> id ) {
		case 0: { lib_terminal_printf( &kernel_terminal, (uint8_t *) "Divide-by-zero Error" ); break; }
		case 1: { lib_terminal_printf( &kernel_terminal, (uint8_t *) "Debug" ); break; }
		case 3: { lib_terminal_printf( &kernel_terminal, (uint8_t *) "Breakpoint" ); break; }
		case 4: { lib_terminal_printf( &kernel_terminal, (uint8_t *) "Overflow" ); break; }
		case 5: { lib_terminal_printf( &kernel_terminal, (uint8_t *) "Bound Range Exceeded" ); break; }
		case 6: { lib_terminal_printf( &kernel_terminal, (uint8_t *) "Invalid Opcode" ); break; }
		case 7: { lib_terminal_printf( &kernel_terminal, (uint8_t *) "Device Not Available" ); break; }
		case 8: { lib_terminal_printf( &kernel_terminal, (uint8_t *) "Double Fault" ); break; }
		case 9: { lib_terminal_printf( &kernel_terminal, (uint8_t *) "Coprocessor Segment Overrun" ); break; }
		case 10: { lib_terminal_printf( &kernel_terminal, (uint8_t *) "Invalid TSS" ); break; }
		case 11: { lib_terminal_printf( &kernel_terminal, (uint8_t *) "Segment Not Present" ); break; }
		case 12: { lib_terminal_printf( &kernel_terminal, (uint8_t *) "Stack-Segment Fault" ); break; }
		case 13: { lib_terminal_printf( &kernel_terminal, (uint8_t *) "General Protection Fault" ); break; }
		case 14: {
			// show exteption type
			lib_terminal_printf( &kernel_terminal, (uint8_t *) "Page Fault\n" );
			
			// debug
			lib_terminal_printf( &kernel_terminal, (uint8_t *) "RAX 0x%16X   RBX 0x%16X\nRCX 0x%16X   RDX 0x%16X\nRBP 0x%16X\nR8  0x%16X   R9  0x%16X\nR10 0x%16X   R11 0x%16X\nR12 0x%16X   R13 0x%16X\nR14 0x%16X   R15 0x%16X\n", exception -> rax, exception -> rbx, exception -> rcx, exception -> rdx, exception -> rbp, exception -> r8, exception -> r9, exception -> r10, exception -> r11, exception -> r12, exception -> r13, exception -> r14, exception -> r15 );
			
			// done
			break;
		}
		case 16: { lib_terminal_printf( &kernel_terminal, (uint8_t *) "x87 Floating-Point" ); break; }
		case 17: { lib_terminal_printf( &kernel_terminal, (uint8_t *) "Alignment Check" ); break; }
		case 18: { lib_terminal_printf( &kernel_terminal, (uint8_t *) "Machine Check" ); break; }
		case 19: { lib_terminal_printf( &kernel_terminal, (uint8_t *) "SIMD Floating-Point" ); break; }
		case 20: { lib_terminal_printf( &kernel_terminal, (uint8_t *) "Virtualization" ); break; }
		case 21: { lib_terminal_printf( &kernel_terminal, (uint8_t *) "Control Protection" ); break; }
		case 28: { lib_terminal_printf( &kernel_terminal, (uint8_t *) "Hypervisor Injection" ); break; }
		case 29: { lib_terminal_printf( &kernel_terminal, (uint8_t *) "VMM Communication" ); break; }
		case 30: { lib_terminal_printf( &kernel_terminal, (uint8_t *) "Security" ); break; }
		default: { lib_terminal_printf( &kernel_terminal, (uint8_t *) "{unknown}" ); break; }
	}

	// show task name
	struct KERNEL_TASK_STRUCTURE *task = kernel_task_active();
	lib_terminal_printf( &kernel_terminal, (uint8_t *) "Task: '%s' near 0x%X)\n", task -> name, exception -> cr2 );

	// hold the door
	while( TRUE ) {}
}

__attribute__ (( preserve_all ))
void kernel_idt_interrupt_default( struct KERNEL_IDT_STRUCTURE_RETURN *interrupt ) {
	// tell APIC of current logical processor that the hardware interrupt is being handled properly
	kernel_lapic_accept();
}
