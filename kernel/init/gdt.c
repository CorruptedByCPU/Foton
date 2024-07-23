/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_gdt( void ) {
	// prepare area for Global Descriptor Table
	kernel -> gdt_header.base_address = (struct KERNEL_GDT_STRUCTURE_ENTRY *) kernel_memory_alloc( TRUE );

	// create code descriptor ring0 (CS)
	kernel -> gdt_header.base_address[ 1 ].access = 0x98;
	kernel -> gdt_header.base_address[ 1 ].flags_and_limit_high = 0x20;

	// create data descriptor ring0 (DS)
	kernel -> gdt_header.base_address[ 2 ].access = 0x92;
	kernel -> gdt_header.base_address[ 2 ].flags_and_limit_high = 0x20;

	// create data descriptor ring3 (DS)
	kernel -> gdt_header.base_address[ 4 ].access = 0xF2;
	kernel -> gdt_header.base_address[ 4 ].flags_and_limit_high = 0x20;

	// create code descriptor ring3 (CS)
	kernel -> gdt_header.base_address[ 5 ].access = 0xF8;
	kernel -> gdt_header.base_address[ 5 ].flags_and_limit_high = 0x20;

	// configure header of Global Descriptor Table
	kernel -> gdt_header.limit = STD_PAGE_byte;

	// load a new Global Descriptor Table
	__asm__ volatile( "lgdt (%0)" :: "r" (&kernel -> gdt_header) );

	// set valid descriptor values
	kernel_gdt_reload();

	// initialize stack pointer inside TSS table
	kernel -> tss_table.rsp0 = KERNEL_STACK_pointer;

	// show GDT properties
	// kernel_log( (uint8_t *) "GDT base address 0x%X\n", kernel -> gdt_header.base_address );
}
