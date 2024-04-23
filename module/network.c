/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions of standard library
	//----------------------------------------------------------------------
	#include	"../library/std.h"
	#include	"../library/macro.h"
	//----------------------------------------------------------------------
	// variables, structures, definitions of kernel
	//----------------------------------------------------------------------
	#include	"../kernel/config.h"
	//----------------------------------------------------------------------
	// variables, structures, definitions of module
	//----------------------------------------------------------------------
	#include	"./network/config.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"./network/data.c"

void _entry( uintptr_t kernel_ptr ) {
	// preserve kernel structure pointer
	kernel = (struct KERNEL *) kernel_ptr;

	// assign area for incomming packets
	module_network_rx_base_address = (uint64_t *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( MODULE_NETWORK_RX_limit * sizeof( uintptr_t ) ) >> STD_SHIFT_PAGE );

	// share incomming function
	kernel -> network_rx = (void *) module_network_rx;

	// hold the door
	while( TRUE );
}

void module_network_rx( uintptr_t packet ) {
	// debug
	kernel -> log( (uint8_t *) "--- PACKET ---\n" );

	// block access to stack modification
	MACRO_LOCK( module_network_rx_semaphore );

	// rx stack is full?
	if( module_network_rx_limit == MODULE_NETWORK_RX_limit ) return;	// yep

	// add packet to stack
	module_network_rx_base_address[ module_network_rx_limit++ ] = packet;

	// unlock
	MACRO_UNLOCK( module_network_rx_semaphore );
}