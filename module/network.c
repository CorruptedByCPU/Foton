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
	#include	"../kernel/page.h"
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

	// initialize network module
	module_network_init();

	// never ending story
	while( TRUE ) {
		// packet for translation?
		if( ! module_network_rx_limit ) continue;	// nope

		// properties of first packet
		struct MODULE_NETWORK_STRUCTURE_FRAME_ETHERNET *ethernet = (struct MODULE_NETWORK_STRUCTURE_FRAME_ETHERNET *) (*module_network_rx_base_address & STD_PAGE_mask);

		// release packet area?
		uint8_t release = FALSE;

		// choose action
		switch( ethernet -> type ) {
			case MODULE_NETWORK_FRAME_ETHERNET_TYPE_arp: {
				// parse ARP frame
				release = module_network_arp( ethernet, *module_network_rx_base_address & ~STD_PAGE_mask );

				// done
				break;
			}
		}

		// release packet area
		if( release ) kernel -> memory_release( (uintptr_t) ethernet, TRUE );

		// block access to stack modification
		MACRO_LOCK( module_network_rx_semaphore );

		// remove packet from stack
		for( uint64_t i = 0; i < module_network_rx_limit; i++ ) module_network_rx_base_address[ i ] = module_network_rx_base_address[ i + 1 ];

		// one packet less on stack
		module_network_rx_limit--;

		// unlock
		MACRO_UNLOCK( module_network_rx_semaphore );
	}
}

uint8_t module_network_arp( struct MODULE_NETWORK_STRUCTURE_FRAME_ETHERNET *ethernet, uint16_t length ) {
	// properties of ARP frame
	struct MODULE_NETWORK_STRUCTURE_FRAME_ARP *arp = (struct MODULE_NETWORK_STRUCTURE_FRAME_ARP *) ((uintptr_t) ethernet + sizeof( struct MODULE_NETWORK_STRUCTURE_FRAME_ETHERNET ));

	// release packet area
	return TRUE;
}

void module_network_init( void ) {
	// assign area for incomming packets
	module_network_rx_base_address = (uint64_t *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( MODULE_NETWORK_RX_limit * sizeof( uintptr_t ) ) >> STD_SHIFT_PAGE );

	// share incomming function
	kernel -> network_rx = (void *) module_network_rx;
}

void module_network_rx( uintptr_t packet ) {
	// block access to stack modification
	MACRO_LOCK( module_network_rx_semaphore );

	// rx stack is full?
	if( module_network_rx_limit < MODULE_NETWORK_RX_limit )	// no
		// add packet to stack
		module_network_rx_base_address[ module_network_rx_limit++ ] = packet;

	// unlock
	MACRO_UNLOCK( module_network_rx_semaphore );
}