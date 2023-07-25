/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// library
	//----------------------------------------------------------------------
	#include	"../library/color.h"
	#include	"../library/color.c"
	#include	"../library/elf.h"
	#include	"../library/elf.c"
	#include	"../library/font.h"
	#include	"../library/font.c"
	#include	"../library/string.h"
	#include	"../library/string.c"
	#include	"../library/terminal.h"
	#include	"../library/terminal.c"
	//----------------------------------------------------------------------
	// variables, structures, definitions of limine
	//----------------------------------------------------------------------
	#include	"../limine/limine.h"
	//----------------------------------------------------------------------
	// variables, structures, definitions of kernel
	//----------------------------------------------------------------------
	#include	"idt.h"
	#include	"gdt.h"
	#include	"tss.h"
	#include	"lapic.h"
	#include	"hpet.h"
	#include	"io_apic.h"
	#include	"config.h"
	#include	"lapic.h"
	#include	"memory.h"
	#include	"page.h"
	#include	"task.h"
	#include	"tss.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"data.c"
	//----------------------------------------------------------------------
	// kernel routines, procedures
	//----------------------------------------------------------------------
	#include	"lapic.c"
	#include	"hpet.c"
	#include	"idt.c"
	#include	"io_apic.c"
	#include	"memory.c"
	#include	"page.c"
	#include	"task.c"
	//----------------------------------------------------------------------
	// variables, structures, definitions of kernel environment initialization
	//----------------------------------------------------------------------
	#include	"init/acpi.h"
	#include	"init/lapic.h"
	#include	"init/reload.h"
	//----------------------------------------------------------------------
	// kernel environment initialization routines, procedures
	//----------------------------------------------------------------------
	#include	"init/acpi.c"
	#include	"init/gdt.c"
	#include	"init/hpet.c"
	#include	"init/idt.c"
	#include	"init/lapic.c"
	#include	"init/memory.c"
	#include	"init/page.c"
	#include	"init/task.c"
	#include	"init/reload.c"

// our mighty init
void kernel_init( void ) {
	// linear framebuffer is available (with 32 bits per pixel)?
	if( limine_framebuffer_request.response == NULL || ! limine_framebuffer_request.response -> framebuffer_count || limine_framebuffer_request.response -> framebuffers[ 0 ] -> bpp != STD_VIDEO_DEPTH_bit )
		// no, hold the door (screen will be black)
		while( TRUE );

	// update terminal properties
	kernel_terminal.width			= limine_framebuffer_request.response -> framebuffers[ 0 ] -> width;
	kernel_terminal.height			= limine_framebuffer_request.response -> framebuffers[ 0 ] -> height;
	kernel_terminal.base_address		= (uint32_t *) ((uintptr_t) limine_framebuffer_request.response -> framebuffers[ 0 ] -> address | KERNEL_PAGE_logical);
	kernel_terminal.scanline_pixel		= limine_framebuffer_request.response -> framebuffers[ 0 ] -> pitch >> STD_VIDEO_DEPTH_shift;
	kernel_terminal.color_foreground	= STD_COLOR_WHITE;
	kernel_terminal.color_background	= STD_COLOR_BLACK;

	// initialize terminal
	lib_terminal( &kernel_terminal );

	// show welcome message
	lib_terminal_printf( &kernel_terminal, "/ Foton, environment initialization. /\n\n" );

	// create binary memory map
	kernel_init_memory();

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

	// configure HPET
	kernel_init_hpet();

	// reload BSP configuration
	kernel_init_reload();
}
