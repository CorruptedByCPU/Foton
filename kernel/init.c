/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// library as build-in
	//----------------------------------------------------------------------
	#include	"../library/elf.c"
	#include	"../library/string.c"
	#include	"../library/color.c"
	#include	"../library/font.c"
	#include	"../library/terminal.c"
	//----------------------------------------------------------------------
	// drivers
	//----------------------------------------------------------------------
	#include	"driver/port.h"
	#include	"driver/port.c"
	#include	"driver/rtc.h"
	#include	"driver/rtc.c"
	#include	"driver/serial.h"
	#include	"driver/serial.c"
	//----------------------------------------------------------------------
	// variables, structures, definitions of limine
	//----------------------------------------------------------------------
	#include	"../limine/limine.h"
	//----------------------------------------------------------------------
	// variables, structures, definitions of kernel
	//----------------------------------------------------------------------
	#include	"task.h"
	#include	"vfs.h"
	#include	"time.h"
	#include	"idt.h"	
	#include	"gdt.h"
	#include	"tss.h"
	#include	"io_apic.h"
	#include	"config.h"
	#include	"lapic.h"
	#include	"memory.h"
	#include	"page.h"
	#include	"exec.h"
	#include	"storage.h"
	#include	"library.h"
	#include	"module.h"
	#include	"ipc.h"
	#include	"stream.h"
	#include	"network.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"data.c"
	//----------------------------------------------------------------------
	// kernel routines, procedures
	//----------------------------------------------------------------------
	#include	"log.c"
	#include	"lapic.c"
	#include	"idt.c"
	#include	"io_apic.c"
	#include	"memory.c"
	#include	"page.c"
	#include	"task.c"
	#include	"vfs.c"
	#include	"storage.c"
	#include	"syscall.c"
	#include	"library.c"
	#include	"exec.c"
	#include	"module.c"
	#include	"time.c"
	#include	"rtc.c"
	#include	"stream.c"
	#include	"network.c"
	//----------------------------------------------------------------------
	// variables, structures, definitions of kernel environment initialization
	//----------------------------------------------------------------------
	#include	"init/acpi.h"
	#include	"init/ap.h"
	//----------------------------------------------------------------------
	// kernel environment initialization routines, procedures
	//----------------------------------------------------------------------
	#include	"init/limine.c"
	#include	"init/acpi.c"
	#include	"init/environment.c"
	#include	"init/gdt.c"
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
	#include	"init/stream.c"
	#include	"init/cmd.c"
	#include	"init/network.c"
	#include	"init/clean.c"

// our mighty init
void _entry( void ) {
	// DEBUG ---------------------------------------------------------------

	// initialize default debug output
	driver_serial_init();

	// check passed variables/structures by Limine bootloader
	kernel_init_limine();

	// BASE ----------------------------------------------------------------

	// initialize global kernel environment variables/functions/rountines
	kernel_init_environment();

	// create binary memory map
	kernel_init_memory();

	// parse ACPI tables
	kernel_init_acpi();

	// recreate kernel's paging structures
	kernel_init_page();

	// reload new kernel environment paging array
	__asm__ volatile( "movq %0, %%cr3\nmovq %1, %%rsp" :: "r" ((uintptr_t) kernel -> page_base_address & ~KERNEL_PAGE_mirror), "r" ((uintptr_t) KERNEL_STACK_pointer) );

	// create Global Descriptor Table
	kernel_init_gdt();

	// create Interrupt Descriptor Table
	kernel_init_idt();

	// ESSENTIAL -----------------------------------------------------------

	// initialize stream set
	kernel_init_stream();

	// create Task queue and insert kernel into it
	kernel_init_task();

	// prepare Inter Process communication
	kernel_init_ipc();

	// configure RTC
	kernel_init_rtc();

	// initialize network stack
	kernel_init_network();

	// register all available storage devices
	kernel_init_storage();

	// initialize VFS storages
	kernel_init_vfs();

	// create library management space
	kernel_init_library();

	// load basic list of modules
	kernel_init_module();

	// execute first process
	kernel_init_cmd();

	// EXTRA ---------------------------------------------------------------

	// initialize other CPUs
	kernel_init_smp();

	// some clean up
	kernel_init_clean();

	// FINISH --------------------------------------------------------------

	// reload BSP configuration
	kernel_init_ap();
}
