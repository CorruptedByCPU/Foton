/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_ap( void ) {
	// reload kernel environment paging array
	__asm__ volatile( "movq %0, %%cr3" :: "r" ((uintptr_t) kernel -> page_base_address & ~KERNEL_PAGE_logical) );

	// reload the Global Descriptor Table
	__asm__ volatile( "lgdt (%0)" :: "r" (&kernel -> gdt_header) );

	// reset to valid descriptor values
	kernel_gdt_reload();

	// reload the Interrupt Descriptor Table
	__asm__ volatile( "lidt (%0)" :: "r" (&kernel -> idt_header) );

	// create a TSS descriptor for current BS/A processor
	uint8_t current_cpu_id = kernel_lapic_id();

	// Task State Segment descriptor properties
	struct KERNEL_TSS_STRUCTURE_ENTRY *tss = (struct KERNEL_TSS_STRUCTURE_ENTRY *) &kernel -> gdt_header.base_address[ KERNEL_TSS_OFFSET ];

	// insert descriptor data for BSP/logical processor
	tss[ current_cpu_id ].limit_low = sizeof( struct KERNEL_TSS_STRUCTURE );	// size of Task State Segment array in Bytes
	tss[ current_cpu_id ].base_low = (uint64_t) &kernel -> tss_table;		// TSS table address (bits 15..0)
	tss[ current_cpu_id ].base_middle = (uint64_t) &kernel -> tss_table >> 16;	// TSS table address (bits 23..16)
	tss[ current_cpu_id ].access = 0b10001001;					// TSS descriptor access attributes
	tss[ current_cpu_id ].base_high = (uint64_t) &kernel -> tss_table >> 24;	// TSS table address (bits 31..24)
	tss[ current_cpu_id ].base_64bit = (uint64_t) &kernel -> tss_table >> 32;	// TSS table address (bits 63..32)

	// set TSS descriptor for BS/A processor
	__asm__ volatile( "ltr %%ax" :: "a" ((uintptr_t) &tss[ current_cpu_id ] & ~STD_PAGE_mask) );

	// select task from queue which CPU is now processing
	kernel -> task_cpu_address[ current_cpu_id ] = &kernel -> task_base_address[ 0 ];

	// disable Emulation, enable Monitoring
	__asm__ volatile( "movq %cr0, %rax\nand $0xFFFB, %ax\nor $0x02, %ax\nmovq %rax, %cr0" );

	// reset FPU state
	__asm__ volatile( "fninit" );

	// allow all BS/A processors to write on read-only pages
	__asm__ volatile( "movq %cr0, %rax\nandq $~(1 << 16), %rax\nmovq %rax, %cr0" );

	// enable OSXSAVE, OSFXSR support
	__asm__ volatile( "movq %cr4, %rax\norq $0b1000000011000000000, %rax\nmovq %rax, %cr4" );

	// enable X87, SSE, AVX support
	__asm__ volatile( "xorq %rcx, %rcx\nxgetbv\nor $0x3, %eax\nxsetbv" );

	//--------------------------------------------------------------------------
	// enable syscall/sysret support
	__asm__ volatile( "movl $0xC0000080, %ecx\nrdmsr\nor $1, %eax\nwrmsr" );	// enable SCE flag
	// set code/stack segments for kernel and process
	__asm__ volatile( "wrmsr" :: "a" (EMPTY), "d" (0x00180008), "c" (KERNEL_INIT_AP_MSR_STAR) );
	// set the kernel entry routin
	__asm__ volatile( "wrmsr" :: "a" ((uintptr_t) kernel_syscall), "d" ((uintptr_t) kernel_syscall >> 32), "c" (KERNEL_INIT_AP_MSR_LSTAR) );
	// disable IF flag and DF after calling syscall
	__asm__ volatile( "wrmsr" :: "a" ((uint32_t) KERNEL_TASK_EFLAGS_if | KERNEL_TASK_EFLAGS_df), "d" (EMPTY ), "c" (KERNEL_INIT_AP_MSR_EFLAGS) );
	//--------------------------------------------------------------------------

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

	// don't wait for miracle, speed is our motto :]
	__asm__ volatile( "int $0x20" );
}
