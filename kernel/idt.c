/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint64_t disasm( uint8_t *rip ) {
	uint8_t *parse = rip;
	uint8_t opcode;

next:
	opcode = *(parse++);

	switch( opcode ) {
		default:
			break;
	}

	if( opcode == 0x90 ) {
		log( "nop" );
		return 1;
	}

	return EMPTY;
}

void kernel_idt_exception( struct KERNEL_STRUCTURE_IDT_EXCEPTION *exception ) {
	// properties of current stack
	struct KERNEL_STRUCTURE_TASK *current = kernel_task_current();

	// action based on exception id
	switch( exception -> id ) {
		case 14: {
			// exception for process stack space?
			if( MACRO_PAGE_ALIGN_UP( exception -> cr2 ) == KERNEL_TASK_STACK_pointer - (current -> stack_page << STD_SHIFT_PAGE) ) {
				// describe additional space under process stack
				kernel_page_alloc( (uint64_t *) current -> cr3, KERNEL_TASK_STACK_pointer - (++current -> stack_page << STD_SHIFT_PAGE), TRUE, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | (current -> type << KERNEL_PAGE_TYPE_offset) );

				// done
				return;
			}

			// done
			break;
		}
	}

	// time to hunt some BUGs

	// say, hello!
	log( "\n Simple debuffer by https://blackdev.org/\n\n" );

	log( "--REGISTERS------------------FLAGS--------------" );
	uint64_t x = kernel -> terminal.cursor_x - 1;
	uint64_t y = kernel -> terminal.cursor_y;
	log( "\n                                               |\r" );
	log( "| rip:    %16X | ", exception -> rip ); if( exception -> eflags & KERNEL_IDT_EFLAGS_CF ) log( "CF " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_PF ) log( "PF " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_AF ) log( "AF " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_ZF ) log( "ZF " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_SF ) log( "SF " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_TF ) log( "TF " );
	log( "\n                                               |\r" );
	log( "| rsp:    %16X | ", exception -> rsp ); if( exception -> eflags & KERNEL_IDT_EFLAGS_IF ) log( "IF " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_DF ) log( "DF " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_OF ) log( "OF " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_NT ) log( "NT " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_RF ) log( "RF " );
	log( "\n                                               |\r" );
	log( "| eflags:         %8X | ", exception -> eflags ); if( exception -> eflags & KERNEL_IDT_EFLAGS_VM ) log( "VM " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_AC ) log( "AC " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_VIF ) log( "VIF " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_VIP ) log( "VIP " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_ID ) log( "ID " );
	log( "\n------------------------------------------------\n" );
	log( "| rax: %8X %8X  %20u |\n", exception -> rax >> STD_MOVE_DWORD, (uint32_t) exception -> rax, exception -> rax );
	log( "| rbx: %8X %8X  %20u |\n", exception -> rbx >> STD_MOVE_DWORD, (uint32_t) exception -> rbx, exception -> rbx );
	log( "| rcx: %8X %8X  %20u |\n", exception -> rcx >> STD_MOVE_DWORD, (uint32_t) exception -> rcx, exception -> rcx );
	log( "| rdx: %8X %8X  %20u |\n", exception -> rdx >> STD_MOVE_DWORD, (uint32_t) exception -> rdx, exception -> rdx );
	log( "| rsi: %8X %8X  %20u |\n", exception -> rsi >> STD_MOVE_DWORD, (uint32_t) exception -> rsi, exception -> rsi );
	log( "| rdi: %8X %8X  %20u |\n", exception -> rdi >> STD_MOVE_DWORD, (uint32_t) exception -> rdi, exception -> rdi );
	log( "|  r8: %8X %8X  %20u |\n", exception -> r8 >> STD_MOVE_DWORD, (uint32_t) exception -> r8, exception -> r8 );
	log( "|  r9: %8X %8X  %20u |\n", exception -> r9 >> STD_MOVE_DWORD, (uint32_t) exception -> r9, exception -> r9 );
	log( "| r10: %8X %8X  %20u |\n", exception -> r10 >> STD_MOVE_DWORD, (uint32_t) exception -> r10, exception -> r10 );
	log( "| r11: %8X %8X  %20u |\n", exception -> r11 >> STD_MOVE_DWORD, (uint32_t) exception -> r11, exception -> r11 );
	log( "| r12: %8X %8X  %20u |\n", exception -> r12 >> STD_MOVE_DWORD, (uint32_t) exception -> r12, exception -> r12 );
	log( "| r13: %8X %8X  %20u |\n", exception -> r13 >> STD_MOVE_DWORD, (uint32_t) exception -> r13, exception -> r13 );
	log( "| r14: %8X %8X  %20u |\n", exception -> r14 >> STD_MOVE_DWORD, (uint32_t) exception -> r14, exception -> r14 );
	log( "| r15: %8X %8X  %20u |\n", exception -> r15 >> STD_MOVE_DWORD, (uint32_t) exception -> r15, exception -> r15 );
	log( "------------------------------------------------\n" );
	kernel -> terminal.cursor_x = x;
	kernel -> terminal.cursor_y = y;
	kernel_terminal_cursor();
	log( "--STACK---------------------------------\n" );
	kernel -> terminal.cursor_x = x;
	kernel_terminal_cursor();
	uint64_t *stack = (uint64_t *) exception -> rsp;
	for( uint64_t i = 0; i < kernel -> terminal.height_char - (y + 3); i++ ) {
		if( (uintptr_t) &stack[ i ] == KERNEL_LIBRARY_base_address || ! stack ) break;
		log( "| 0x%16X %8X %8X |\n", (uint64_t) &stack[ i ], stack[ i ] >> STD_MOVE_DWORD, stack[ i ] & STD_MASK_DWORD );
		kernel -> terminal.cursor_x = x;
		kernel_terminal_cursor();
	}
	log( "----------------------------------------" );
	x = kernel -> terminal.cursor_x - 1;
	kernel -> terminal.cursor_x = x;
	kernel -> terminal.cursor_y = y;
	kernel_terminal_cursor();
	log( "--DISASSEMBLY---------------------------\n" );
	kernel -> terminal.cursor_x = x;
	kernel_terminal_cursor();
	uint8_t *rip = (uint8_t *) exception -> rip;
	for( uint64_t i = 0; i < kernel -> terminal.height_char - (y + 3); i++ ) {
		log( "| 0x%16X ", (uint64_t) &rip[ i ] );

		uint64_t l = disasm( (uint8_t *) &rip[ i ] );
		// if( ! l ) log( "%2X", rip[ i ] );

		kernel -> terminal.cursor_y++;
		kernel -> terminal.cursor_x = x;
		kernel_terminal_cursor();
	}
	log( "----------------------------------------" );

	// hold the door
	MACRO_DEBUF();
	MACRO_DEBUF();
	volatile uint8_t a = TRUE;
	while( a );
}

__attribute__ (( preserve_most ))
void kernel_idt_interrupt_default( struct KERNEL_STRUCTURE_IDT_RETURN *interrupt ) {
	// tell APIC of current logical processor that the hardware interrupt is being handled properly
	kernel_apic_accept();
}

void kernel_idt_attach( uint8_t irq, uint16_t type, uintptr_t address ) {
	// interrupt type
	kernel -> idt_header.base_address[ irq ].type = type;

	// address of code descriptor that runs procedure
	kernel -> idt_header.base_address[ irq ].gdt_descriptor = sizeof( ( ( struct KERNEL_STRUCTURE_GDT *) 0 ) -> cs_ring0 );

	// address of exception handler
	kernel -> idt_header.base_address[ irq ].base_low = (uint16_t) address;
	kernel -> idt_header.base_address[ irq ].base_middle = (uint16_t) (address >> 16);
	kernel -> idt_header.base_address[ irq ].base_high = (uint32_t) (address >> 32);
}
