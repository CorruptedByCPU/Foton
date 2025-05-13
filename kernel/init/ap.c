/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_ap( struct limine_smp_info *info ) {
	// set Write-Combining on framebuffer memory area
	kernel_init_mtrr();

	// reload kernel environment paging array
	__asm__ volatile( "movq %0, %%cr3" :: "r" ((uintptr_t) kernel -> page_base_address & ~KERNEL_MEMORY_mirror) );

	// reload Global Descriptor Table
	__asm__ volatile( "lgdt (%0)" :: "r" (&kernel -> gdt_header) );

	// reset to valid descriptor values
	kernel_gdt_reload();

	// reload Interrupt Descriptor Table
	__asm__ volatile( "lidt (%0)" :: "r" (&kernel -> idt_header) );

	// create a TSS descriptor for current BS/A processor
	uint8_t id = kernel_apic_id();

	// Task State Segment descriptor properties
	struct KERNEL_STRUCTURE_TSS_ENTRY *tss = (struct KERNEL_STRUCTURE_TSS_ENTRY *) &kernel -> gdt_header.base_address[ KERNEL_TSS_OFFSET ];

	// insert descriptor data for BSP/logical processor
	tss[ id ].limit_low	= sizeof( struct KERNEL_STRUCTURE_TSS );	// size of Task State Segment array in Bytes
	tss[ id ].base_low	= (uint64_t) &kernel -> tss;			// TSS table address (bits 15..0)
	tss[ id ].base_middle	= (uint64_t) &kernel -> tss >> 16;		// TSS table address (bits 23..16)
	tss[ id ].access	= 0x89;						// TSS descriptor access attributes
	tss[ id ].base_high	= (uint64_t) &kernel -> tss >> 24;		// TSS table address (bits 31..24)
	tss[ id ].base_64bit	= (uint64_t) &kernel -> tss >> 32;		// TSS table address (bits 63..32)

	// set TSS descriptor for BS/A processor
	__asm__ volatile( "ltr %%ax" :: "a" ((uintptr_t) &tss[ id ] & ~STD_PAGE_mask) );

	// select task from queue which CPU is now processing
	kernel -> task_ap_address[ id ] = 0;

	// disable x87 FPU Emulation, enable co-processor Monitor
	__asm__ volatile( "movq %cr0, %rax\nand $0xFFFB, %ax\nor $0x02, %ax\nmovq %rax, %cr0" );

	// reset FPU state
	__asm__ volatile( "fninit" );

	// allow all BS/A processors to write on read-only pages
	__asm__ volatile( "movq %cr0, %rax\nandq $~(1 << 16), %rax\nmovq %rax, %cr0" );

	// enable FXSAVE/FXRSTOR (bit 9), OSXMMEXCPT (bit 10) and OSXSAVE (bit 18)
	__asm__ volatile( "movq %cr4, %rax\norq $0b000001000000011000000000, %rax\nmovq %rax, %cr4" );

	// enable X87, SSE, AVX support
	__asm__ volatile( "xor %ecx, %ecx\nxgetbv\nor $0x3, %eax\nxsetbv" );

	//--------------------------------------------------------------------------
	// enable syscall/sysret support
	__asm__ volatile( "movl $0xC0000080, %ecx\nrdmsr\nor $1, %eax\nwrmsr" );	// enable SCE flag
	// set code/stack segments for kernel and process
	__asm__ volatile( "wrmsr" :: "a" (EMPTY), "c" (KERNEL_INIT_AP_MSR_STAR), "d" (0x00180008) );	// edx - GDT descriptors
	// set the kernel entry routin
	__asm__ volatile( "wrmsr" :: "a" ((uintptr_t) kernel_syscall), "c" (KERNEL_INIT_AP_MSR_LSTAR), "d" ((uintptr_t) kernel_syscall >> 32) );
	// disable IF flag and DF after calling syscall
	__asm__ volatile( "wrmsr" :: "a" ((uint32_t) KERNEL_TASK_EFLAGS_if | KERNEL_TASK_EFLAGS_df), "c" (KERNEL_INIT_AP_MSR_EFLAGS), "d" (EMPTY) );
	//--------------------------------------------------------------------------

	// re/initialize APIC of BS/A processor
	kernel_init_apic();

	// reload cpu cycle counter in APIC controller
	kernel_apic_reload();

	// accept current interrupt call (if exist)
	kernel_apic_accept();

	// BS/A initialized
	kernel -> cpu_limit++;

	// enable interrupt handling
	__asm__ volatile( "sti" );

	// don't wait for miracle, speed is our motto :]
	__asm__ volatile( "int $0x20" );
}
