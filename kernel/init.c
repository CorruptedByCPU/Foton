/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// library
	//----------------------------------------------------------------------
	#include	"../library/elf.c"
	#include	"../library/string.c"
	#include	"../library/vfs.c"
	//----------------------------------------------------------------------
	// drivers
	//----------------------------------------------------------------------
	#include	"driver/port.h"
	#include	"driver/port.c"
	#include	"driver/pci.h"
	#include	"driver/pci.c"
	#include	"driver/serial.h"
	#include	"driver/serial.c"
	//----------------------------------------------------------------------
	// variables, structures, definitions of limine
	//----------------------------------------------------------------------
	#include	"../limine/limine.h"
	//----------------------------------------------------------------------
	// variables, structures, definitions of kernel
	//----------------------------------------------------------------------
	#include	"time.h"
	#include	"rtc.h"
	#include	"idt.h"
	#include	"gdt.h"
	#include	"tss.h"
	#include	"lapic.h"
	// #include	"hpet.h"
	#include	"io_apic.h"
	#include	"config.h"
	#include	"lapic.h"
	#include	"memory.h"
	#include	"page.h"
	#include	"task.h"
	#include	"exec.h"
	#include	"tss.h"
	#include	"storage.h"
	#include	"library.h"
	#include	"module.h"
	#include	"ipc.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"data.c"
	//----------------------------------------------------------------------
	// kernel routines, procedures
	//----------------------------------------------------------------------
	#include	"log.c"
	#include	"lapic.c"
	// #include	"hpet.c"
	#include	"idt.c"
	#include	"io_apic.c"
	#include	"memory.c"
	#include	"page.c"
	#include	"task.c"
	#include	"storage.c"
	#include	"syscall.c"
	#include	"library.c"
	#include	"exec.c"
	#include	"module.c"
	#include	"time.c"
	#include	"rtc.c"
	//----------------------------------------------------------------------
	// variables, structures, definitions of kernel environment initialization
	//----------------------------------------------------------------------
	#include	"init/acpi.h"
	#include	"init/lapic.h"
	#include	"init/ap.h"
	//----------------------------------------------------------------------
	// kernel environment initialization routines, procedures
	//----------------------------------------------------------------------
	#include	"init/acpi.c"
	#include	"init/env.c"
	#include	"init/gdt.c"
	// #include	"init/hpet.c"
	#include	"init/idt.c"
	#include	"init/lapic.c"
	#include	"init/memory.c"
	#include	"init/page.c"
	#include	"init/task.c"
	#include	"init/ap.c"
	#include	"init/smp.c"
	#include	"init/vfs.c"
	#include	"init/storage.c"
	#include	"init/library.c"
	#include	"init/module.c"
	#include	"init/rtc.c"
	#include	"init/ipc.c"

// struct KERNEL_IDT_STRUCTURE_ENTRY interrupt[ 32 ] = { EMPTY };

// void tmp_interrupt( void ) {
// 	__asm__ volatile( "addq $0x08, %rsp" );
// }

// void tmp_mount( uint8_t i, uint16_t type, uintptr_t address ) {
// 	// interrupt type
// 	interrupt[ i ].type = type;

// 	// address of code descriptor that runs procedure
// 	interrupt[ i ].gdt_descriptor = offsetof( struct KERNEL_GDT_STRUCTURE, cs_ring3 );

// 	// address of exception handler
// 	interrupt[ i ].base_low = (uint16_t) address;
// 	interrupt[ i ].base_middle = (uint16_t) (address >> 16);
// 	interrupt[ i ].base_high = (uint32_t) (address >> 32);
// }

// our mighty init
void _entry( void ) {
	MACRO_DEBUF();
	while( TRUE );

	// // attach CPU exception handlers
	// tmp_mount( 0, KERNEL_IDT_TYPE_gate_interrupt, (uintptr_t) &kernel_idt_exception_divide_by_zero );
	// tmp_mount( 1, KERNEL_IDT_TYPE_gate_interrupt, (uintptr_t) &kernel_idt_exception_debug );
	// tmp_mount( 3, KERNEL_IDT_TYPE_gate_interrupt, (uintptr_t) &kernel_idt_exception_breakpoint );
	// tmp_mount( 4, KERNEL_IDT_TYPE_gate_interrupt, (uintptr_t) &kernel_idt_exception_overflow );
	// tmp_mount( 5, KERNEL_IDT_TYPE_gate_interrupt, (uintptr_t) &kernel_idt_exception_boud_range_exceeded );
	// tmp_mount( 6, KERNEL_IDT_TYPE_gate_interrupt, (uintptr_t) &kernel_idt_exception_invalid_opcode );
	// tmp_mount( 7, KERNEL_IDT_TYPE_gate_interrupt, (uintptr_t) &kernel_idt_exception_device_not_available );
	// tmp_mount( 8, KERNEL_IDT_TYPE_gate_interrupt, (uintptr_t) &kernel_idt_exception_double_fault );
	// tmp_mount( 9, KERNEL_IDT_TYPE_gate_interrupt, (uintptr_t) &kernel_idt_exception_coprocessor_segment_overrun );
	// tmp_mount( 10, KERNEL_IDT_TYPE_gate_interrupt, (uintptr_t) &kernel_idt_exception_invalid_tss );
	// tmp_mount( 11, KERNEL_IDT_TYPE_gate_interrupt, (uintptr_t) &kernel_idt_exception_segment_not_present );
	// tmp_mount( 12, KERNEL_IDT_TYPE_gate_interrupt, (uintptr_t) &kernel_idt_exception_stack_segment_fault );
	// tmp_mount( 13, KERNEL_IDT_TYPE_gate_interrupt, (uintptr_t) &kernel_idt_exception_general_protection_fault );
	// tmp_mount( 14, KERNEL_IDT_TYPE_gate_interrupt, (uintptr_t) &kernel_idt_exception_page_fault );
	// tmp_mount( 16, KERNEL_IDT_TYPE_gate_interrupt, (uintptr_t) &kernel_idt_exception_x87_floating_point );
	// tmp_mount( 17, KERNEL_IDT_TYPE_gate_interrupt, (uintptr_t) &kernel_idt_exception_alignment_check );
	// tmp_mount( 18, KERNEL_IDT_TYPE_gate_interrupt, (uintptr_t) &kernel_idt_exception_machine_check );
	// tmp_mount( 19, KERNEL_IDT_TYPE_gate_interrupt, (uintptr_t) &kernel_idt_exception_simd_floating_point );
	// tmp_mount( 20, KERNEL_IDT_TYPE_gate_interrupt, (uintptr_t) &kernel_idt_exception_virtualization );
	// tmp_mount( 21, KERNEL_IDT_TYPE_gate_interrupt, (uintptr_t) &kernel_idt_exception_control_protection );
	// tmp_mount( 28, KERNEL_IDT_TYPE_gate_interrupt, (uintptr_t) &kernel_idt_exception_hypervisor_injection );
	// tmp_mount( 29, KERNEL_IDT_TYPE_gate_interrupt, (uintptr_t) &kernel_idt_exception_vmm_communication );
	// tmp_mount( 30, KERNEL_IDT_TYPE_gate_interrupt, (uintptr_t) &kernel_idt_exception_security );

	// struct KERNEL_IDT_STRUCTURE_HEADER tmp_idt_header = { sizeof( uintptr_t ) * 32, (struct	KERNEL_IDT_STRUCTURE_ENTRY *) &interrupt };
	// __asm__ volatile( "lidt (%0)" :: "r" (&tmp_idt_header) );

	// initialize default debug output
	// driver_serial_init();

	// linear framebuffer is available (with 32 bits per pixel)?
	if( limine_framebuffer_request.response == NULL || ! limine_framebuffer_request.response -> framebuffer_count || limine_framebuffer_request.response -> framebuffers[ 0 ] -> bpp != STD_VIDEO_DEPTH_bit )
		// no, hold the door (screen will be black)
		while( TRUE );

	// create binary memory map
	kernel_init_memory();

	// fill in remaining necessary variables / functions
	kernel_init_env();

	// parse ACPI tables
	kernel_init_acpi();

	// recreate kernel's paging structures
	kernel_init_page();

	// reload new kernel environment paging array
	__asm__ volatile( "movq %0, %%cr3\nmovq %1, %%rsp" :: "r" ((uintptr_t) kernel -> page_base_address & ~KERNEL_PAGE_logical), "r" ((uintptr_t) KERNEL_STACK_pointer) );

	// create Global Descriptor Table
	kernel_init_gdt();

	// create Interrupt Descriptor Table
	kernel_init_idt();

	// create Task queue and insert kernel into it
	kernel_init_task();

	// configure RTC
	kernel_init_rtc();

	// configure HPET
	// kernel_init_hpet();

	// initialize other CPUs
	kernel_init_smp();

	// register all available storage devices
	kernel_init_storage();

	// create library management space
	kernel_init_library();

	// prepare Inter Process communication
	kernel_init_ipc();

	// load basic list of modules
	kernel_init_module();

	// execute first process
	kernel_exec( (uint8_t *) "init", 4 );

	// reload BSP configuration
	kernel_init_ap();
}
