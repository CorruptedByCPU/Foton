/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions of Limine Bootloader
	//----------------------------------------------------------------------
	#include	"../limine/limine.h"
	//======================================================================

	//----------------------------------------------------------------------
	// Build-in libraries
	//----------------------------------------------------------------------
	// #include	"../library/color.c"
	#include	"../library/elf.c"
	// #include	"../library/font.c"
	#include	"../library/string.c"
	// #include	"../library/terminal.c"
	//======================================================================

	//----------------------------------------------------------------------
	// Build-in driver variables, structures, definitions
	//----------------------------------------------------------------------
	#include	"driver/port.h"
	#include	"driver/rtc.h"
	//======================================================================

	//----------------------------------------------------------------------
	// variables, structures, definitions of Kernel
	//----------------------------------------------------------------------
	#include	"apic.h"
	#include	"exec.h"
	#include	"gdt.h"
	#include	"idt.h"
	#include	"io_apic.h"
	#include	"library.h"
	#include	"memory.h"
	#include	"page.h"
	#include	"storage.h"
	#include	"syscall.h"
	#include	"task.h"
	// #include	"time.h"
	#include	"tss.h"
	#include	"vfs.h"
	// --- always at end
	#include	"config.h"
	//======================================================================

	//----------------------------------------------------------------------
	// variables, structures of Kernel
	//----------------------------------------------------------------------
	#include	"data.c"
	//======================================================================

	//----------------------------------------------------------------------
	// Build-in drivers
	//----------------------------------------------------------------------
	#include	"driver/port.c"
	#include	"driver/rtc.c"
	//======================================================================

	//----------------------------------------------------------------------
	// kernel routines, procedures
	//----------------------------------------------------------------------
	#include	"apic.c"
	#include	"exec.c"
	#include	"idt.c"
	#include	"io_apic.c"
	#include	"library.c"
	// #include	"log.c"
	#include	"memory.c"
	#include	"module.c"
	#include	"page.c"
	#include	"storage.c"
	#include	"syscall.c"
	#include	"task.c"
	#include	"time.c"
	#include	"vfs.c"
	//======================================================================

	//----------------------------------------------------------------------
	// variables, structures, definitions of Kernel initialization routines
	//----------------------------------------------------------------------
	#include	"init/acpi.h"
	#include	"init/ap.h"
	#include	"init/apic.h"
	//======================================================================

	//----------------------------------------------------------------------
	// kernel environment initialization routines, procedures
	//----------------------------------------------------------------------
	#include	"init/acpi.c"
	#include	"init/ap.c"
	#include	"init/apic.c"
	#include	"init/env.c"
	#include	"init/exec.c"
	#include	"init/gdt.c"
	#include	"init/idt.c"
	#include	"init/library.c"
	#include	"init/memory.c"
	#include	"init/module.c"
	#include	"init/page.c"
	#include	"init/storage.c"
	#include	"init/task.c"
	#include	"init/vfs.c"
	//======================================================================

// start of kernel initialization
void _entry( void ) {
	// BASE ----------------------------------------------------------------

	// initialize kernel environment pointer and variables/functions/rountines
	kernel_init_env();

	// binary memory map as source of everything
	kernel_init_memory();

	// parse ACPI tables
	kernel_init_acpi();

	// create own Kernel paging structures
	kernel_init_page();

	// reload new kernel environment paging array
	__asm__ volatile( "movq %0, %%cr3\nmovq %1, %%rsp" :: "r" ((uintptr_t) kernel -> page_base_address & ~KERNEL_MEMORY_mirror), "r" ((uintptr_t) KERNEL_STACK_pointer) );

	// create Global Descriptor Table
	kernel_init_gdt();

	// create Interrupt Descriptor Table
	kernel_init_idt();

	// ESSENTIAL -----------------------------------------------------------

	// configure RTC as our only source of passing time, for now
	driver_rtc_init();

	// assign area for streams
	// kernel -> stream_base_address = (struct KERNEL_STRUCTURE_STREAM *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( KERNEL_STREAM_limit * sizeof( struct KERNEL_STRUCTURE_STREAM ) ) >> STD_SHIFT_PAGE );

	// create storage container
	kernel_init_storage();

	// initialize VFS directory
	kernel_init_vfs();

	// create Task queue
	kernel_init_task();

	// load essential modules
	kernel_init_module();

	// prepare library management area
	kernel_init_library();

	// execute initial software
	kernel_init_exec();

	// FINISH --------------------------------------------------------------

	// reload BSP configuration
	kernel_init_ap();
}
