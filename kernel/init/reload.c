/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_reload( void ) {
	MACRO_DEBUF();

	// reload kernel environment paging array
	__asm__ volatile( "movq %0, %%cr3\nmovq %1, %%rsp" :: "r" ((uintptr_t) kernel -> page_base_address & ~KERNEL_PAGE_logical), "r" ((uintptr_t) KERNEL_STACK_pointer) );

	// reload the Global Descriptor Table
	__asm__ volatile( "lgdt (%0)" :: "r" (&kernel -> gdt_header) );

	// reset to valid descriptor values
	kernel_gdt_reload();

	// reload the Interrupt Descriptor Table
	__asm__ volatile( "lidt (%0)" :: "r" (&kernel -> idt_header) );

	// create a TSS descriptor for current BS/A processor
	uint8_t id = kernel_lapic_id();

	// Task State Segment descriptor properties
	struct KERNEL_TSS_STRUCTURE_ENTRY *tss = (struct KERNEL_TSS_STRUCTURE_ENTRY *) &kernel -> gdt_header.base_address[ KERNEL_TSS_OFFSET ];

	// insert descriptor data for BSP/logical processor
	tss[ id ].limit_low = sizeof( struct KERNEL_TSS_STRUCTURE );	// size of Task State Segment array in Bytes
	tss[ id ].base_low = (uint64_t) &kernel -> tss_table;		// TSS table address (bits 15..0)
	tss[ id ].base_middle = (uint64_t) &kernel -> tss_table >> 16;	// TSS table address (bits 23..16)
	tss[ id ].access = 0b10001001;					// TSS descriptor access attributes
	tss[ id ].base_high = (uint64_t) &kernel -> tss_table >> 24;	// TSS table address (bits 31..24)
	tss[ id ].base_64bit = (uint64_t) &kernel -> tss_table >> 32;	// TSS table address (bits 63..32)

	// set TSS descriptor for BS/A processor
	__asm__ volatile( "ltr %%ax" :: "a" ((uintptr_t) &tss[ id ] & ~STD_PAGE_mask) );

	// reset FPU state
	__asm__ volatile( "fninit" );

	// enable OSXSAVE, OSFXSR support
	__asm__ volatile( "movq %cr4, %rax\norq $0b1000000011000000000, %rax\nmovq %rax, %cr4" );

	// enable X87, SSE, AVX support
	__asm__ volatile( "xorq %rcx, %rcx\nxgetbv\nor $0x3, %eax\nxsetbv" );

	// initialize LAPIC of BS/A processor
	kernel_lapic_init();

	// reload cpu cycle counter in LAPIC controller
	kernel_lapic_reload();

	// accept current interrupt call (if exist)
	kernel_lapic_accept();

	// BS/A initialized
	kernel -> cpu_count++;

	// enable interrupt handling
	__asm__ volatile( "sti" );

	// wait for miracle :]
	while( TRUE );
}
