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
	log( "\n \033[38;5;118m|deBuffer| \033[38;5;245mby https://\033[38;5;250mblackdev.org\033[38;5;245m/        \033[0mMenu: (\033[38;5;11ms\033[0m)tep (\033[38;5;11mc\033[0m)ontinue\n\n" );
	// if( kernel -> framebuffer_width_pixel < 1280 || kernel -> framebuffer_height_pixel < 720 ) { log( "\n\tAt least 1280x720 resolution is required." ); while( TRUE ); }

	log( "\033[0m CPU State             EFLAGS (literals)    Stack                             Disassembly\n" );
	uint64_t x = kernel -> terminal.cursor_x + 1;
	uint64_t y = kernel -> terminal.cursor_y;
	kernel -> terminal.cursor_x = x;
	kernel -> terminal.cursor_y = y;
	kernel_terminal_cursor();
	log( "\033[38;5;33mRIP: \033[38;5;245m%8X\033[38;5;250m%8X ", exception -> rip >> STD_MOVE_DWORD, exception -> rip & STD_MASK_DWORD ); log( "\033[38;5;226m\0" ); if( exception -> eflags & KERNEL_IDT_EFLAGS_CF ) log( "CF " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_PF ) log( "PF " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_AF ) log( "AF " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_ZF ) log( "ZF " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_SF ) log( "SF " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_TF ) log( "TF " ); log( "\n" );
	kernel -> terminal.cursor_x++; kernel_terminal_cursor();
	log( "\033[38;5;33mRSP: \033[38;5;245m%8X\033[38;5;250m%8X ", exception -> rsp >> STD_MOVE_DWORD, exception -> rsp & STD_MASK_DWORD ); log( "\033[38;5;226m\0" ); if( exception -> eflags & KERNEL_IDT_EFLAGS_IF ) log( "IF " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_DF ) log( "DF " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_OF ) log( "OF " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_NT ) log( "NT " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_RF ) log( "RF " ); log( "\n" );
	kernel -> terminal.cursor_x++; kernel_terminal_cursor();
	log( "\033[38;5;33mEFLAGS:      \033[38;5;250m%8X ", exception -> eflags ); log( "\033[38;5;226m\0" ); if( exception -> eflags & KERNEL_IDT_EFLAGS_VM ) log( "VM " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_AC ) log( "AC " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_VIF ) log( "VIF " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_VIP ) log( "VIP " ); if( exception -> eflags & KERNEL_IDT_EFLAGS_ID ) log( "ID " ); log( "\n\n" );
	log( "\033[0m Registers\n" ); kernel -> terminal.cursor_x++; kernel_terminal_cursor();
	log( "\033[38;5;33mRAX: \033[38;5;245m%8X\033[38;5;250m%8X \033[38;5;49m%20u\n", exception -> rax >> STD_MOVE_DWORD, exception -> rax & STD_MASK_DWORD, exception -> rax ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "\033[38;5;33mRBX: \033[38;5;245m%8X\033[38;5;250m%8X \033[38;5;49m%20u\n", exception -> rbx >> STD_MOVE_DWORD, exception -> rbx & STD_MASK_DWORD, exception -> rbx ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "\033[38;5;33mRCX: \033[38;5;245m%8X\033[38;5;250m%8X \033[38;5;49m%20u\n", exception -> rcx >> STD_MOVE_DWORD, exception -> rcx & STD_MASK_DWORD, exception -> rcx ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "\033[38;5;33mRDX: \033[38;5;245m%8X\033[38;5;250m%8X \033[38;5;49m%20u\n", exception -> rdx >> STD_MOVE_DWORD, exception -> rdx & STD_MASK_DWORD, exception -> rdx ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "\033[38;5;33mRSI: \033[38;5;245m%8X\033[38;5;250m%8X \033[38;5;49m%20u\n", exception -> rsi >> STD_MOVE_DWORD, exception -> rsi & STD_MASK_DWORD, exception -> rsi ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "\033[38;5;33mRDI: \033[38;5;245m%8X\033[38;5;250m%8X \033[38;5;49m%20u\n", exception -> rdi >> STD_MOVE_DWORD, exception -> rdi & STD_MASK_DWORD, exception -> rdi ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "\033[38;5;33mRBP: \033[38;5;245m%8X\033[38;5;250m%8X \033[38;5;49m%20u\n", exception -> rbp >> STD_MOVE_DWORD, exception -> rbp & STD_MASK_DWORD, exception -> rbp ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "\033[38;5;33m R8: \033[38;5;245m%8X\033[38;5;250m%8X \033[38;5;49m%20u\n", exception -> r8 >> STD_MOVE_DWORD, exception -> r8 & STD_MASK_DWORD, exception -> r8 ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "\033[38;5;33m R9: \033[38;5;245m%8X\033[38;5;250m%8X \033[38;5;49m%20u\n", exception -> r9 >> STD_MOVE_DWORD, exception -> r9 & STD_MASK_DWORD, exception -> r9 ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "\033[38;5;33mR10: \033[38;5;245m%8X\033[38;5;250m%8X \033[38;5;49m%20u\n", exception -> r10 >> STD_MOVE_DWORD, exception -> r10 & STD_MASK_DWORD, exception -> r10 ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "\033[38;5;33mR11: \033[38;5;245m%8X\033[38;5;250m%8X \033[38;5;49m%20u\n", exception -> r11 >> STD_MOVE_DWORD, exception -> r11 & STD_MASK_DWORD, exception -> r11 ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "\033[38;5;33mR12: \033[38;5;245m%8X\033[38;5;250m%8X \033[38;5;49m%20u\n", exception -> r12 >> STD_MOVE_DWORD, exception -> r12 & STD_MASK_DWORD, exception -> r12 ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "\033[38;5;33mR13: \033[38;5;245m%8X\033[38;5;250m%8X \033[38;5;49m%20u\n", exception -> r13 >> STD_MOVE_DWORD, exception -> r13 & STD_MASK_DWORD, exception -> r13 ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "\033[38;5;33mR14: \033[38;5;245m%8X\033[38;5;250m%8X \033[38;5;49m%20u\n", exception -> r14 >> STD_MOVE_DWORD, exception -> r14 & STD_MASK_DWORD, exception -> r14 ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "\033[38;5;33mR15: \033[38;5;245m%8X\033[38;5;250m%8X \033[38;5;49m%20u\n", exception -> r15 >> STD_MOVE_DWORD, exception -> r15 & STD_MASK_DWORD, exception -> r15 ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	log( "\n" ); kernel -> terminal.cursor_x++; kernel_terminal_cursor();

	uint64_t cr;

	log( "\033[0mControl Registers\n" ); kernel -> terminal.cursor_x++; kernel_terminal_cursor();
	__asm__ volatile( "mov %%cr0, %0" : "=r" (cr) );
	log( "\033[38;5;124mCR0: \033[38;5;245m%8X\033[38;5;250m%8X \033[38;5;49m%20u\n", cr >> STD_MOVE_DWORD, cr & STD_MASK_DWORD, cr ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	__asm__ volatile( "mov %%cr2, %0" : "=r" (cr) );
	log( "\033[38;5;124mCR2: \033[38;5;245m%8X\033[38;5;250m%8X \033[38;5;49m%20u\n", cr >> STD_MOVE_DWORD, cr & STD_MASK_DWORD, cr ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	__asm__ volatile( "mov %%cr3, %0" : "=r" (cr) );
	log( "\033[38;5;124mCR3: \033[38;5;245m%8X\033[38;5;250m%8X \033[38;5;49m%20u\n", cr >> STD_MOVE_DWORD, cr & STD_MASK_DWORD, cr ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
	__asm__ volatile( "mov %%cr4, %0" : "=r" (cr) );
	log( "\033[38;5;124mCR4: \033[38;5;245m%8X\033[38;5;250m%8X \033[38;5;49m%20u\n", cr >> STD_MOVE_DWORD, cr & STD_MASK_DWORD, cr ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();

	log( "\n" ); kernel -> terminal.cursor_x++; kernel_terminal_cursor();

	uint64_t dr;

	log( "\033[0mDebug Registers\n" ); kernel -> terminal.cursor_x++; kernel_terminal_cursor();
    	__asm__ volatile("mov %%dr0, %0" : "=r" (dr));
	log( "\033[38;5;202mDR0: \033[38;5;245m%8X\033[38;5;250m%8X \033[38;5;49m%20u\n", dr >> STD_MOVE_DWORD, dr & STD_MASK_DWORD, dr ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
    	__asm__ volatile("mov %%dr1, %0" : "=r" (dr));
	log( "\033[38;5;202mDR1: \033[38;5;245m%8X\033[38;5;250m%8X \033[38;5;49m%20u\n", dr >> STD_MOVE_DWORD, dr & STD_MASK_DWORD, dr ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
    	__asm__ volatile("mov %%dr2, %0" : "=r" (dr));
	log( "\033[38;5;202mDR2: \033[38;5;245m%8X\033[38;5;250m%8X \033[38;5;49m%20u\n", dr >> STD_MOVE_DWORD, dr & STD_MASK_DWORD, dr ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
    	__asm__ volatile("mov %%dr3, %0" : "=r" (dr));
	log( "\033[38;5;202mDR3: \033[38;5;245m%8X\033[38;5;250m%8X \033[38;5;49m%20u\n", dr >> STD_MOVE_DWORD, dr & STD_MASK_DWORD, dr ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
    	__asm__ volatile("mov %%dr6, %0" : "=r" (dr));
	log( "\033[38;5;202mDR6: \033[38;5;245m%8X\033[38;5;250m%8X \033[38;5;49m%20u\n", dr >> STD_MOVE_DWORD, dr & STD_MASK_DWORD, dr ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
    	__asm__ volatile("mov %%dr7, %0" : "=r" (dr));
	log( "\033[38;5;202mDR7: \033[38;5;245m%8X\033[38;5;250m%8X \033[38;5;49m%20u\n", dr >> STD_MOVE_DWORD, dr & STD_MASK_DWORD, dr ); kernel -> terminal.cursor_x = x; kernel_terminal_cursor();

	x += 43;
	kernel -> terminal.cursor_y = y;
	for( uint64_t i = 0; i < kernel -> terminal.height_char - (y + 1); i++ ) { kernel -> terminal.cursor_x = x; kernel_terminal_cursor(); log( "                                    \n" ); }
	kernel -> terminal.cursor_y = y;
	uint64_t *stack = (uint64_t *) exception -> rsp;
	for( uint64_t i = 0; i < kernel -> terminal.height_char - (y + 1); i++ ) {
		if( (uintptr_t) &stack[ i ] == KERNEL_LIBRARY_base_address || ! stack ) break;
		kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
		log( "\033[38;5;240m%16X \033[38;5;245m%8X\033[38;5;250m%8X \n", (uint64_t) &stack[ i ], stack[ i ] >> STD_MOVE_DWORD, stack[ i ] & STD_MASK_DWORD );
	}

	x += 34;
	kernel -> terminal.cursor_y = y;
	for( uint64_t i = 0; i < kernel -> terminal.height_char - (y + 1); i++ ) { kernel -> terminal.cursor_x = x; kernel_terminal_cursor(); log( "                                                                             \n" ); }
	kernel -> terminal.cursor_y = y;
	uint8_t *rip = (uint8_t *) exception -> rip;
	for( uint64_t i = 0; i < kernel -> terminal.height_char - (y + 1); i++ ) {
		kernel -> terminal.cursor_x = x; kernel_terminal_cursor();
		log( "\033[38;5;243m%16X \033[0m", (uint64_t) rip );
		rip += lib_asm( rip );

		if( *rip == 0x90 ) break;

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
