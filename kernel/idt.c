/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_idt_exception( struct KERNEL_STRUCTURE_IDT_EXCEPTION *exception ) {
	// action based on exception id
	switch( exception -> id ) {
		case 14: {
			// // exception for process stack space?
			// if( MACRO_PAGE_ALIGN_UP( exception -> cr2 ) == KERNEL_TASK_STACK_pointer - (task -> stack << STD_SHIFT_PAGE) ) {
			// 	// describe additional space under process stack
			// 	kernel_page_alloc( (uint64_t *) task -> cr3, KERNEL_TASK_STACK_pointer - (++task -> stack << STD_SHIFT_PAGE), 1, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | (task -> page_type << KERNEL_PAGE_TYPE_offset) );

			// 	// done
			// 	return;
			// }

			// done
			break;
		}

		default: { kernel_log( (uint8_t *) "ERROR!" ); }
	}

	// time to hunt some BUGs
	MACRO_DEBUF();
	volatile uint8_t ok = FALSE;
	while( ! ok ) { MACRO_DEBUF(); }
	MACRO_DEBUF();
}

__attribute__ (( preserve_most ))
void kernel_idt_interrupt_default( struct KERNEL_STRUCTURE_IDT_RETURN *interrupt ) {
	// tell APIC of current logical processor that the hardware interrupt is being handled properly
	kernel_apic_accept();
}

void kernel_idt_attach( uint8_t id, uint16_t type, uintptr_t address ) {
	// interrupt type
	kernel -> idt_header.base_address[ id ].type = type;

	// address of code descriptor that runs procedure
	kernel -> idt_header.base_address[ id ].gdt_descriptor = sizeof( ( ( struct KERNEL_STRUCTURE_GDT *) 0 ) -> cs_ring0 );

	// address of exception handler
	kernel -> idt_header.base_address[ id ].base_low = (uint16_t) address;
	kernel -> idt_header.base_address[ id ].base_middle = (uint16_t) (address >> 16);
	kernel -> idt_header.base_address[ id ].base_high = (uint32_t) (address >> 32);
}
