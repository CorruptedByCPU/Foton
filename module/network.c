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
	#include	"../kernel/driver/rtc.h"
	//----------------------------------------------------------------------
	// structures, definitions
	//----------------------------------------------------------------------
	#include	"./network/config.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"./network/data.c"
	//----------------------------------------------------------------------
	// functions / procedures
	//----------------------------------------------------------------------
	#include	"./network/arp.c"
	#include	"./network/ethernet.c"
	#include	"./network/icmp.c"
	#include	"./network/init.c"
	#include	"./network/ip.c"
	#include	"./network/socket.c"
	#include	"./network/udp.c"

void _entry( uintptr_t kernel_ptr ) {
	// preserve kernel structure pointer
	kernel = (struct KERNEL *) kernel_ptr;

	// initialize network module
	module_network_init();

	// debug
	kernel -> network_interface.ipv4_address = 0x0A000040;	// 10.0.0.64

	// never ending story
	while( TRUE ) {
		// frame for translation?
		if( ! module_network_rx_limit ) continue;	// nope

		// properties of first header
		struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet = (struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET *) (*module_network_rx_base_address & STD_PAGE_mask);

		// release frame area?
		uint8_t release = TRUE;	// yes, why not?

		// choose action
		switch( ethernet -> type ) {
			case MODULE_NETWORK_HEADER_ETHERNET_TYPE_arp: {
				// parse as ARP frame
				release = module_network_arp( ethernet, *module_network_rx_base_address & ~STD_PAGE_mask );

				// done
				break;
			}

			case MODULE_NETWORK_HEADER_ETHERNET_TYPE_ipv4: {
				// parse as IPv4 frame
				release = module_network_ipv4( ethernet, *module_network_rx_base_address & ~STD_PAGE_mask );

				// done
				break;
			}
		}

		// release frame area
		if( release ) kernel -> memory_release( (uintptr_t) ethernet, TRUE );

		// block access to stack modification
		MACRO_LOCK( module_network_rx_semaphore );

		// remove frame from stack
		for( uint64_t i = 0; i < module_network_rx_limit; i++ ) module_network_rx_base_address[ i ] = module_network_rx_base_address[ i + 1 ];

		// one frame less on stack
		module_network_rx_limit--;

		// unlock
		MACRO_UNLOCK( module_network_rx_semaphore );
	}
}

uint16_t module_network_checksum( uint16_t *data, uint16_t length ) {
	// initial checksum value
	uint32_t result = EMPTY;

	// add each chunk of data
	for( uint16_t i = 0; i < length >> STD_SHIFT_2; i++ ) {
		result += (uint16_t) MACRO_ENDIANNESS_WORD( data[ i ] );
	
		// if overflow
		if( result > 0xFFFF ) result = (result & STD_WORD_mask) + 1;
	}

	// if result is EMPTY
	if( ! result ) return STD_MAX_unsigned;
	else return ~MACRO_ENDIANNESS_WORD( ((result >> STD_MOVE_WORD) + (result & STD_WORD_mask)) );
}

void module_network_rx( uintptr_t frame ) {
	// block access to stack modification
	MACRO_LOCK( module_network_rx_semaphore );

	// rx stack is full?
	if( module_network_rx_limit < MODULE_NETWORK_YX_limit )	// no
		// add frame to stack
		module_network_rx_base_address[ module_network_rx_limit++ ] = frame;

	// unlock
	MACRO_UNLOCK( module_network_rx_semaphore );
}

int64_t module_network_send( int64_t socket, uint8_t *data, uint64_t length ) {
	// choose action
	switch( module_network_socket_list[ socket ].protocol ) {
		case STD_NETWORK_PROTOCOL_icmp: { module_network_ipv4_exit( (struct MODULE_NETWORK_STRUCTURE_SOCKET *) &module_network_socket_list[ socket ], data, length ); break; }
		case STD_NETWORK_PROTOCOL_udp: { module_network_udp_exit( (struct MODULE_NETWORK_STRUCTURE_SOCKET *) &module_network_socket_list[ socket ], data, length ); break; }
	}

	// sent
	return EMPTY;
}

uintptr_t module_network_tx( void ) {
	// tx stack is empty?
	if( ! module_network_tx_limit ) return EMPTY;	// nothing to send

	// retrieve frame properties
	volatile uintptr_t frame = *module_network_tx_base_address;

	// block access to stack modification
	MACRO_LOCK( module_network_tx_semaphore );

	// remove frame from stack
	for( uint64_t i = 0; i < module_network_tx_limit; i++ ) module_network_tx_base_address[ i ] = module_network_tx_base_address[ i + 1 ];

	// one frame sent
	module_network_tx_limit--;

	// unlock
	MACRO_UNLOCK( module_network_tx_semaphore );

	// return frame properties
	return frame;
}