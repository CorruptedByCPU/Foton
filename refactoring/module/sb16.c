/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions of standard library
	//----------------------------------------------------------------------
	#include	"../library/std.h"
	//----------------------------------------------------------------------
	// variables, structures, definitions of kernel
	//----------------------------------------------------------------------
	#include	"../kernel/config.h"
	#include	"../kernel/idt.h"
	#include	"../kernel/io_apic.h"
	#include	"../kernel/lapic.h"
	#include	"../kernel/page.h"
	//----------------------------------------------------------------------
	// drivers
	//----------------------------------------------------------------------
	#include	"../kernel/driver/port.h"
	#include	"../kernel/driver/port.c"
	#include	"../kernel/driver/pci.h"
	#include	"../kernel/driver/pci.c"
	//----------------------------------------------------------------------
	// variables, structures, definitions of module
	//----------------------------------------------------------------------
	#include	"./sb16/config.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"./sb16/data.c"

void _entry( uintptr_t kernel_ptr ) {
	// preserve kernel structure pointer
	kernel = (struct KERNEL *) kernel_ptr;

	// try to reset audio controller
	driver_port_out_byte( MODULE_SB16_PORT_RESET, TRUE );
	kernel -> time_sleep( 3 );	// wait about ~3ms
	driver_port_out_byte( MODULE_SB16_PORT_RESET, FALSE );
	kernel -> time_sleep( 1 );	// wait about ~1ms

	// doesn't exist?
	if( driver_port_in_byte( MODULE_SB16_PORT_READ ) != 0xAA ) while( TRUE );

	// debug
	// kernel -> log( (uint8_t *) "[SB16] Controller found.\n" );

	// speaker on
	driver_port_out_byte( MODULE_SB16_PORT_WRITE, 0xD1 );

	// driver_port_out_byte( MODULE_SB16_PORT_MIXER, 0x80 );
	// driver_port_in_byte( MODULE_SB16_PORT_DATA );

	// hold the door (release CPU time)
	while( TRUE ) kernel -> time_sleep( TRUE );
}