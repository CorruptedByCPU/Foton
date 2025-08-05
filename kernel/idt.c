/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	#include	"../library/asm.c"

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
	log( "\n > Simple debuffer by https://blackdev.org/\n\n" );
	// if( kernel -> framebuffer_width_pixel < 1280 || kernel -> framebuffer_height_pixel < 720 ) { log( "\n\tAt least 1280x720 resolution is required." ); while( TRUE ); }

	log( " CPU State              EFLAGS Literals      Stack                                Disassembly\n" );
	uint64_t x = kernel -> terminal.cursor_x;	// + 1;
	uint64_t y = kernel -> terminal.cursor_y;
	kernel -> terminal.color_background = 0x00002200;
	for( uint8_t i = 0; i < 3; i++ ) { kernel -> terminal.cursor_x = x; kernel_terminal_cursor(); log( "                                           \n" ); }
	kernel -> terminal.color_background = 0x00004000; log( "\n General Registers\n" ); kernel -> terminal.color_background = 0x00002200;
	for( uint8_t i = 0; i < 15; i++ ) { kernel -> terminal.cursor_x = x; kernel_terminal_cursor(); log( "                                           \n" ); }
	kernel -> terminal.color_background = 0x00004000; log( "\n Specific\n" ); kernel -> terminal.color_background = 0x00002200;
	for( uint8_t i = 0; i < 1; i++ ) { kernel -> terminal.cursor_x = x; kernel_terminal_cursor(); log( "                                           \n" ); }
	kernel -> terminal.cursor_x = x;
	kernel -> terminal.cursor_y = y;
	kernel_terminal_cursor();
	log( "RIP: %8X %8X ", exception -> rip >> STD_MOVE_DWORD, exception -> rip & STD_MASK_DWORD ); if( exception -> eflags & KERNEL_IDT_EFLAGS_CF ) log( "CF " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_PF ) log( "PF " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_AF ) log( "AF " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_ZF ) log( "ZF " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_SF ) log( "SF " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_TF ) log( "TF " ); log( "\n" );
	kernel -> terminal.cursor_x++; kernel_terminal_cursor();
	log( "RSP: %8X %8X ", exception -> rsp >> STD_MOVE_DWORD, exception -> rsp & STD_MASK_DWORD ); if( exception -> eflags & KERNEL_IDT_EFLAGS_IF ) log( "IF " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_DF ) log( "DF " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_OF ) log( "OF " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_NT ) log( "NT " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_RF ) log( "RF " ); log( "\n" );
	kernel -> terminal.cursor_x++; kernel_terminal_cursor();
	log( "EFLAGS:       %8X ", exception -> eflags ); if( exception -> eflags & KERNEL_IDT_EFLAGS_VM ) log( "VM " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_AC ) log( "AC " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_VIF ) log( "VIF " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_VIP ) log( "VIP " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_ID ) log( "ID " ); log( "\n\n" );
	kernel -> terminal.cursor_x++; kernel -> terminal.cursor_y++; kernel_terminal_cursor();
	log( "RAX: %8X %8X %20u\n", exception -> rax >> STD_MOVE_DWORD, exception -> rax & STD_MASK_DWORD, exception -> rax ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "RBX: %8X %8X %20u\n", exception -> rbx >> STD_MOVE_DWORD, exception -> rbx & STD_MASK_DWORD, exception -> rbx ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "RCX: %8X %8X %20u\n", exception -> rcx >> STD_MOVE_DWORD, exception -> rcx & STD_MASK_DWORD, exception -> rcx ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "RDX: %8X %8X %20u\n", exception -> rdx >> STD_MOVE_DWORD, exception -> rdx & STD_MASK_DWORD, exception -> rdx ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "RSI: %8X %8X %20u\n", exception -> rsi >> STD_MOVE_DWORD, exception -> rsi & STD_MASK_DWORD, exception -> rsi ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "RDI: %8X %8X %20u\n", exception -> rdi >> STD_MOVE_DWORD, exception -> rdi & STD_MASK_DWORD, exception -> rdi ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "RBP: %8X %8X %20u\n", exception -> rbp >> STD_MOVE_DWORD, exception -> rbp & STD_MASK_DWORD, exception -> rbp ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "R8:  %8X %8X %20u\n", exception -> r8 >> STD_MOVE_DWORD, exception -> r8 & STD_MASK_DWORD, exception -> r8 ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "R9:  %8X %8X %20u\n", exception -> r9 >> STD_MOVE_DWORD, exception -> r9 & STD_MASK_DWORD, exception -> r9 ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "R10: %8X %8X %20u\n", exception -> r10 >> STD_MOVE_DWORD, exception -> r10 & STD_MASK_DWORD, exception -> r10 ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "R11: %8X %8X %20u\n", exception -> r11 >> STD_MOVE_DWORD, exception -> r11 & STD_MASK_DWORD, exception -> r11 ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "R12: %8X %8X %20u\n", exception -> r12 >> STD_MOVE_DWORD, exception -> r12 & STD_MASK_DWORD, exception -> r12 ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "R13: %8X %8X %20u\n", exception -> r13 >> STD_MOVE_DWORD, exception -> r13 & STD_MASK_DWORD, exception -> r13 ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "R14: %8X %8X %20u\n", exception -> r14 >> STD_MOVE_DWORD, exception -> r14 & STD_MASK_DWORD, exception -> r14 ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "R15: %8X %8X %20u\n", exception -> r15 >> STD_MOVE_DWORD, exception -> r15 & STD_MASK_DWORD, exception -> r15 ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "\n" ); kernel -> terminal.cursor_x++; kernel -> terminal.cursor_y++; kernel_terminal_cursor();
	log( "CR2: %8X %8X %20u\n", exception -> cr2 >> STD_MOVE_DWORD, exception -> cr2 & STD_MASK_DWORD, exception -> cr2 ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();

	x += 44;
	kernel -> terminal.cursor_y = y;
	for( uint64_t i = 0; i < kernel -> terminal.height_char - (y + 1); i++ ) { kernel -> terminal.cursor_x = x; kernel_terminal_cursor(); log( "                                    \n" ); }
	kernel -> terminal.cursor_y = y;
	uint64_t *stack = (uint64_t *) exception -> rsp;
	for( uint64_t i = 0; i < kernel -> terminal.height_char - (y + 1); i++ ) {
		if( (uintptr_t) &stack[ i ] == KERNEL_LIBRARY_base_address || ! stack ) break;
		kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
		log( "0x%16X %8X %8X\n", (uint64_t) &stack[ i ], stack[ i ] >> STD_MOVE_DWORD, stack[ i ] & STD_MASK_DWORD );
	}

	x += 37;
	kernel -> terminal.cursor_y = y;
	for( uint64_t i = 0; i < kernel -> terminal.height_char - (y + 1); i++ ) { kernel -> terminal.cursor_x = x; kernel_terminal_cursor(); log( "                                                                             \n" ); }
	kernel -> terminal.cursor_y = y;
	uint8_t *rip = (uint8_t *) exception -> rip;
	for( uint64_t i = 0; i < kernel -> terminal.height_char - (y + 1); i++ ) {
		kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
		log( "0x%16X ", (uint64_t) &rip );

		MACRO_DEBUF();
		rip += lib_asm( rip );
		// log( "---" );

		kernel -> terminal.cursor_y++;
	}

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
