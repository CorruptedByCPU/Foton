/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// Build-in libraries
	//----------------------------------------------------------------------
	#include	"../library/color.c"
	#include	"../library/elf.c"
	#include	"../library/font.c"
	#include	"../library/string.c"
	#include	"../library/terminal.c"
	//======================================================================

	//----------------------------------------------------------------------
	// variables, structures, definitions of Limine Bootloader
	//----------------------------------------------------------------------
	#include	"../limine/limine.h"
	//======================================================================

	//----------------------------------------------------------------------
	// variables, structures, definitions of Kernel
	//----------------------------------------------------------------------
	#include	"apic.h"
	#include	"config.h"
	#include	"memory.h"
	#include	"page.h"
	//======================================================================

	//----------------------------------------------------------------------
	// variables, structures of Kernel
	//----------------------------------------------------------------------
	#include	"data.c"
	//======================================================================

	//----------------------------------------------------------------------
	// kernel routines, procedures
	//----------------------------------------------------------------------
	#include	"log.c"
	#include	"memory.c"
	#include	"page.c"
	//======================================================================

	//----------------------------------------------------------------------
	// variables, structures, definitions of Kernel initialization
	//----------------------------------------------------------------------
	#include	"init/acpi.h"
	//======================================================================

	//----------------------------------------------------------------------
	// kernel environment initialization routines, procedures
	//----------------------------------------------------------------------
	#include	"init/acpi.c"
	#include	"init/env.c"
	#include	"init/memory.c"
	#include	"init/page.c"
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
	kernel_log( (uint8_t *) "\rNew CR3 and RSP," );
	__asm__ volatile( "movq %0, %%cr3\nmovq %1, %%rsp" :: "r" ((uintptr_t) kernel -> page_base_address & ~KERNEL_MEMORY_mirror), "r" ((uintptr_t) KERNEL_STACK_pointer) ); kernel_log( (uint8_t *) " set.\n" );

	// hodor, that should not happen!
	while( TRUE );
}