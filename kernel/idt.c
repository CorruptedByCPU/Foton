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
	// task properties
	struct KERNEL_TASK_STRUCTURE *task = kernel_task_active();

	// show type of exception
	kernel_log( (uint8_t *) "Exception: " );
	switch( exception -> id ) {
		case 0: { kernel_log( (uint8_t *) "Divide-by-zero Error" ); break; }
		case 1: { kernel_log( (uint8_t *) "Debug" ); break; }
		case 3: { kernel_log( (uint8_t *) "Breakpoint" ); break; }
		case 4: { kernel_log( (uint8_t *) "Overflow" ); break; }
		case 5: { kernel_log( (uint8_t *) "Bound Range Exceeded" ); break; }
		case 6: { kernel_log( (uint8_t *) "Invalid Opcode" ); break; }
		case 7: { kernel_log( (uint8_t *) "Device Not Available" ); break; }
		case 8: { kernel_log( (uint8_t *) "Double Fault" ); break; }
		case 9: { kernel_log( (uint8_t *) "Coprocessor Segment Overrun" ); break; }
		case 10: { kernel_log( (uint8_t *) "Invalid TSS" ); break; }
		case 11: { kernel_log( (uint8_t *) "Segment Not Present" ); break; }
		case 12: { kernel_log( (uint8_t *) "Stack-Segment Fault" ); break; }
		case 13: { kernel_log( (uint8_t *) "General Protection Fault" ); break; }
		case 14: {
			// exception for process stack space?
			if( MACRO_PAGE_ALIGN_UP( exception -> cr2 ) == KERNEL_TASK_STACK_pointer - (task -> stack << STD_SHIFT_PAGE) ) {
				// describe additional space under process stack
				kernel_page_alloc( (uint64_t *) task -> cr3, KERNEL_TASK_STACK_pointer - (++task -> stack << STD_SHIFT_PAGE), 1, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | (task -> page_type << KERNEL_PAGE_TYPE_offset) );

				// done
				return;
			}

			kernel_log( (uint8_t *) "Page Fault" );

			// done
			break;
		}
		case 16: { kernel_log( (uint8_t *) "x87 Floating-Point" ); break; }
		case 17: { kernel_log( (uint8_t *) "Alignment Check" ); break; }
		case 18: { kernel_log( (uint8_t *) "Machine Check" ); break; }
		case 19: { kernel_log( (uint8_t *) "SIMD Floating-Point" ); break; }
		case 20: { kernel_log( (uint8_t *) "Virtualization" ); break; }
		case 21: { kernel_log( (uint8_t *) "Control Protection" ); break; }
		case 28: { kernel_log( (uint8_t *) "Hypervisor Injection" ); break; }
		case 29: { kernel_log( (uint8_t *) "VMM Communication" ); break; }
		case 30: { kernel_log( (uint8_t *) "Security" ); break; }
		default: { kernel_log( (uint8_t *) "{unknown}" ); break; }
	}

	// debug
	// kernel_log( (uint8_t *) "\nRAX 0x%16X\nRBX 0x%16X\nRCX 0x%16X\nRDX 0x%16X\nRSI 0x%16X\nRDI 0x%16X\nRBP 0x%16X\nRSP 0x%16X\nR8  0x%16X\nR9  0x%16X\nR10 0x%16X\nR11 0x%16X\nR12 0x%16X\nR13 0x%16X\nR14 0x%16X\nR15 0x%16X", exception -> rax, exception -> rbx, exception -> rcx, exception -> rdx, exception -> rsi, exception -> rdi, exception -> rbp, exception -> rsp, exception -> r8, exception -> r9, exception -> r10, exception -> r11, exception -> r12, exception -> r13, exception -> r14, exception -> r15 );
	
	// show task name
	kernel_log( (uint8_t *) "\nTask: '%s' near CR2: 0x%16X or RIP: 0x%16X)\n", task -> name, exception -> cr2, exception -> rip );

	// // // memory dump
	// // kernel_log( (uint8_t *) "Memory:\n" );
	// // uint8_t *memory = (uint8_t *) (exception -> rip & (uint64_t) ~0x0F );
	// // for( uint8_t y = 0; y < 16; y++ )
	// // 	kernel_log( (uint8_t *) "%8X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X\n", (uintptr_t) &memory[ y * 16 ], memory[ (y * 16) + 0 ], memory[ (y * 16) + 1 ], memory[ (y * 16) + 2 ], memory[ (y * 16) + 3 ], memory[ (y * 16) + 4 ], memory[ (y * 16) + 5 ], memory[ (y * 16) + 6 ], memory[ (y * 16) + 7 ], memory[ (y * 16) + 8 ], memory[ (y * 16) + 9 ], memory[ (y * 16) + 10 ], memory[ (y * 16) + 11 ], memory[ (y * 16) + 12 ], memory[ (y * 16) + 13 ], memory[ (y * 16) + 14 ], memory[ (y * 16) + 15 ]);

	// // // paging dump
	// // kernel_log( (uint8_t *) "Page:\n" );
	// // uint64_t *pml4 = (uint64_t *) task -> cr3; uint64_t area = EMPTY; uint64_t length = EMPTY;
	// // for( uint16_t p4 = 0; p4 < KERNEL_PAGE_PML_records >> STD_SHIFT_2; p4++ ) {
	// // 	if( ! pml4[ p4 ] ) { if( length ) kernel_log( (uint8_t *) "0x%16X - 0x%16X\n", area, area + length - 1 ); area += length + KERNEL_PAGE_PML3_byte; length = EMPTY; continue; }
	// // 	uint64_t *pml3 = (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml4[ p4 ] ) | KERNEL_PAGE_mirror);
	// // 	for( uint16_t p3 = 0; p3 < KERNEL_PAGE_PML_records; p3++ ) {
	// // 		if( ! pml3[ p3 ] ) { if( length ) kernel_log( (uint8_t *) "0x%16X - 0x%16X\n", area, area + length - 1 ); area += length + KERNEL_PAGE_PML2_byte; length = EMPTY; continue; }
	// // 		uint64_t *pml2 = (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml3[ p3 ] ) | KERNEL_PAGE_mirror);
	// // 		for( uint16_t p2 = 0; p2 < KERNEL_PAGE_PML_records; p2++ ) {
	// // 			if( ! pml2[ p2 ] ) { if( length ) kernel_log( (uint8_t *) "0x%16X - 0x%16X\n", area, area + length - 1 ); area += length + KERNEL_PAGE_PML1_byte; length = EMPTY; continue; }
	// // 			uint64_t *pml1 = (uint64_t *) (MACRO_PAGE_ALIGN_DOWN( pml2[ p2 ] ) | KERNEL_PAGE_mirror);
	// // 			for( uint16_t p1 = 0; p1 < KERNEL_PAGE_PML_records; p1++ ) {
	// // 				if( ! pml1[ p1 ] ) { if( length ) kernel_log( (uint8_t *) "0x%16X - 0x%16X\n", area, area + length - 1 ); area += length + STD_PAGE_byte; length = EMPTY; continue; }
	// // 				length += STD_PAGE_byte;
	// // 			}
	// // 		}
	// // 	}
	// // }

	// TODO, disassembly?

	// time to hunt some BUGs
	MACRO_DEBUF();
	volatile uint8_t ok = FALSE;
	while( ! ok ) { MACRO_DEBUF(); }
	MACRO_DEBUF();
}

__attribute__ (( preserve_most ))
void kernel_idt_interrupt_default( struct KERNEL_IDT_STRUCTURE_RETURN *interrupt ) {
	// tell APIC of current logical processor that the hardware interrupt is being handled properly
	kernel_lapic_accept();
}
