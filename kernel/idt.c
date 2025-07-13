/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

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
	kernel -> log( (uint8_t *) "Simple debuffer by Blackdev.org\n" );

	// hold the door
	MACRO_DEBUF();
	MACRO_DEBUF();
	while( TRUE );
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
