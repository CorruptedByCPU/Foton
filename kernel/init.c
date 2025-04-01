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
	#include	"gdt.h"
	#include	"idt.h"
	#include	"tss.h"
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
	#include	"apic.c"
	#include	"idt.c"
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
	#include	"init/gdt.c"
	#include	"init/idt.c"
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
	__asm__ volatile( "movq %0, %%cr3\nmovq %1, %%rsp" :: "r" ((uintptr_t) kernel -> page_base_address & ~KERNEL_MEMORY_mirror), "r" ((uintptr_t) KERNEL_STACK_pointer) );

	// create Global Descriptor Table
	kernel_init_gdt();

	// create Interrupt Descriptor Table
	kernel_init_idt();

	// ESSENTIAL -----------------------------------------------------------

	// debug
	uint64_t offset_x = EMPTY;
	for( uint64_t y = (limine_framebuffer_request.response -> framebuffers[ 0 ] -> height >> STD_SHIFT_2) - 16; y < (limine_framebuffer_request.response -> framebuffers[ 0 ] -> height >> STD_SHIFT_2) + 16; y++ ) { for( uint64_t x = (limine_framebuffer_request.response -> framebuffers[ 0 ] -> width >> STD_SHIFT_2) - 16; x < (limine_framebuffer_request.response -> framebuffers[ 0 ] -> width >> STD_SHIFT_2) + 16; x++ ) { kernel -> framebuffer_base_address[ (y * limine_framebuffer_request.response -> framebuffers[ 0 ] -> width) + x + offset_x ] = 0x0000FF00; } offset_x++; }
	for( uint64_t y = (limine_framebuffer_request.response -> framebuffers[ 0 ] -> height >> STD_SHIFT_2) + 16; y > (limine_framebuffer_request.response -> framebuffers[ 0 ] -> height >> STD_SHIFT_2) - 64; y-- ) { for( uint64_t x = (limine_framebuffer_request.response -> framebuffers[ 0 ] -> width >> STD_SHIFT_2) - 16; x < (limine_framebuffer_request.response -> framebuffers[ 0 ] -> width >> STD_SHIFT_2) + 16; x++ ) { kernel -> framebuffer_base_address[ (y * limine_framebuffer_request.response -> framebuffers[ 0 ] -> width) + x + offset_x ] = 0x0000FF00; } offset_x += (y % 2); }

	// hodor, that should not happen!
	while( TRUE );
}