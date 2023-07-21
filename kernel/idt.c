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
	lib_terminal_printf( &kernel_terminal, "Exception: " );
	switch( exception -> id ) {
		case 0: { lib_terminal_printf( &kernel_terminal, "Divide-by-zero Error" ); break; }
		case 1: { lib_terminal_printf( &kernel_terminal, "Debug" ); break; }
		case 3: { lib_terminal_printf( &kernel_terminal, "Breakpoint" ); break; }
		case 4: { lib_terminal_printf( &kernel_terminal, "Overflow" ); break; }
		case 5: { lib_terminal_printf( &kernel_terminal, "Bound Range Exceeded" ); break; }
		case 6: { lib_terminal_printf( &kernel_terminal, "Invalid Opcode" ); break; }
		case 7: { lib_terminal_printf( &kernel_terminal, "Device Not Available" ); break; }
		case 8: { lib_terminal_printf( &kernel_terminal, "Double Fault" ); break; }
		case 9: { lib_terminal_printf( &kernel_terminal, "Coprocessor Segment Overrun" ); break; }
		case 10: { lib_terminal_printf( &kernel_terminal, "Invalid TSS" ); break; }
		case 11: { lib_terminal_printf( &kernel_terminal, "Segment Not Present" ); break; }
		case 12: { lib_terminal_printf( &kernel_terminal, "Stack-Segment Fault" ); break; }
		case 13: { lib_terminal_printf( &kernel_terminal, "General Protection Fault" ); break; }
		case 14: { lib_terminal_printf( &kernel_terminal, "Page Fault" ); break; }
		case 16: { lib_terminal_printf( &kernel_terminal, "x87 Floating-Point" ); break; }
		case 17: { lib_terminal_printf( &kernel_terminal, "Alignment Check" ); break; }
		case 18: { lib_terminal_printf( &kernel_terminal, "Machine Check" ); break; }
		case 19: { lib_terminal_printf( &kernel_terminal, "SIMD Floating-Point" ); break; }
		case 20: { lib_terminal_printf( &kernel_terminal, "Virtualization" ); break; }
		case 21: { lib_terminal_printf( &kernel_terminal, "Control Protection" ); break; }
		case 28: { lib_terminal_printf( &kernel_terminal, "Hypervisor Injection" ); break; }
		case 29: { lib_terminal_printf( &kernel_terminal, "VMM Communication" ); break; }
		case 30: { lib_terminal_printf( &kernel_terminal, "Security" ); break; }
		default: { lib_terminal_printf( &kernel_terminal, "{unknown}" ); break; }
	}

	// hold the door
	while( TRUE ) {}
}

// why it doesn't preserve registers?
__attribute__ (( preserve_all ))
void kernel_idt_interrupt_default( struct KERNEL_IDT_STRUCTURE_RETURN *interrupt ) {
	// tell APIC of current logical processor that the hardware interrupt is being handled properly
	kernel_lapic_accept();
}
