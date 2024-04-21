/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// library as build-in
	//----------------------------------------------------------------------
	#include	"../library/elf.c"
	#include	"../library/string.c"
	//----------------------------------------------------------------------
	// drivers
	//----------------------------------------------------------------------
	#include	"driver/port.h"
	#include	"driver/port.c"
	#include	"driver/pci.h"
	#include	"driver/pci.c"
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
	#include	"vfs.h"
	#include	"time.h"
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
	#include	"stream.h"
	#include	"log.h"
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
	#include	"vfs.c"
	#include	"storage.c"
	#include	"syscall.c"
	#include	"library.c"
	#include	"exec.c"
	#include	"module.c"
	#include	"time.c"
	#include	"rtc.c"
	#include	"stream.c"
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
	#include	"init/stream.c"
	#include	"init/cmd.c"

// our mighty init
void _entry( void ) {
	// initialize default debug output
	driver_serial_init();

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

	// initialize stream set
	kernel_init_stream();

	// configure RTC
	kernel_init_rtc();

	// configure HPET
	// kernel_init_hpet();

	// initialize other CPUs
	kernel_init_smp();

	// register all available storage devices
	kernel_init_storage();

	// initialize VFS storages
	kernel_init_vfs();

	// create library management space
	kernel_init_library();

	// prepare Inter Process communication
	kernel_init_ipc();

	// load basic list of modules
	kernel_init_module();

	// execute first process
	kernel_init_cmd();

	// reload BSP configuration
	kernel_init_ap();
}
