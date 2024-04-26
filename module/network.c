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
	#include	"../kernel/network.h"
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

	// debug
	kernel -> network_interface.ipv4_address = 0x4000000A;

	// never ending story
	while( TRUE ) {
		// frame for translation?
		if( ! module_network_rx_limit ) continue;	// nope

		// properties of first header
		struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet = (struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET *) (*module_network_rx_base_address & STD_PAGE_mask);

		// release frame area?
		uint8_t release = TRUE;

		// choose action
		switch( ethernet -> type ) {
			case MODULE_NETWORK_HEADER_ETHERNET_TYPE_arp: {
				// parse as ARP frame
				release = module_network_arp( ethernet, *module_network_rx_base_address & ~STD_PAGE_mask );

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

uint8_t module_network_arp( struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length ) {
	// properties of ARP header
	struct MODULE_NETWORK_STRUCTURE_HEADER_ARP *arp = (struct MODULE_NETWORK_STRUCTURE_HEADER_ARP *) ((uintptr_t) ethernet + sizeof( struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET ));

	// inquiry about our IPv4 address?
	if( arp -> operation == MODULE_NETWORK_HEADER_ARP_OPERATION_request && arp -> target_ipv4 != kernel -> network_interface.ipv4_address ) return TRUE;	// no, ignore message and release frame area

	//----------------------------------------------------------------------

	// open new socket for this task
	struct MODULE_NETWORK_STRUCTURE_SOCKET *socket = module_network_socket_open();

	// set socket properties

	// protocol
	socket -> protocol = MODULE_NETWORK_SOCKET_PROTOCOL_arp;

	// target IPv4 address
	socket -> ipv4_address = arp -> source_ipv4;

	// target MAC address
	for( uint8_t i = 0; i < 6; i++ ) socket -> ethernet_mac[ i ] = arp -> source_mac[ i ];

	// socket configured, activate
	socket -> flags = MODULE_NETWORK_SOCKET_FLAG_active;

	//----------------------------------------------------------------------

	// change ARP content to answer
	arp -> operation = MODULE_NETWORK_HEADER_ARP_OPERATION_answer;

	// set target MAC
	for( uint8_t i = 0; i < 6; i++ ) arp -> target_mac[ i ] = socket -> ethernet_mac[ i ];

	// set source MAC
	for( uint8_t i = 0; i < 6; i++ ) arp -> source_mac[ i ] = kernel -> network_interface.ethernet_mac[ i ];
	
	// set target IPv4
	arp -> target_ipv4 = socket -> ipv4_address;

	// set source IPv4
	arp -> source_ipv4 = kernel -> network_interface.ipv4_address;

	// encapsulate ARP frame and send
	module_network_ethernet_encapsulate( socket, ethernet, length - sizeof( struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET ) );

	// frame transferred to driver
	return FALSE;
}

void module_network_ethernet_encapsulate( struct MODULE_NETWORK_STRUCTURE_SOCKET *socket, struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length ) {
	// set target and host MAC addresses
	for( uint8_t i = 0; i < 6; i++ ) ethernet -> target[ i ] = socket -> ethernet_mac[ i ];
	for( uint8_t i = 0; i < 6; i++ ) ethernet -> source[ i ] = kernel -> network_interface.ethernet_mac[ i ];

	// set type of Ethernet header
	switch( socket -> protocol ) {
		case MODULE_NETWORK_SOCKET_PROTOCOL_arp: { ethernet -> type = MODULE_NETWORK_HEADER_ETHERNET_TYPE_arp; break; }
		default: { ethernet -> type = MODULE_NETWORK_HEADER_ETHERNET_TYPE_ipv4; break; }
	}

	// block access to stack modification
	MACRO_LOCK( module_network_tx_semaphore );

	// TODO, make sure that frame was placed inside transfer queue

	// free entry available?
	if( module_network_tx_limit < MODULE_NETWORK_YX_limit )
		// insert frame properties
		module_network_tx_base_address[ module_network_tx_limit++ ] = ((uintptr_t) ethernet & ~KERNEL_PAGE_logical) | length + sizeof( struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET );

	// unlock
	MACRO_UNLOCK( module_network_tx_semaphore );

	// if socket was type of ARP
	if( socket -> protocol == MODULE_NETWORK_SOCKET_PROTOCOL_arp ) socket -> pid = EMPTY;	// release it
}

void module_network_init( void ) {
	// assign area for incomming/outgoing frames
	module_network_rx_base_address = (uint64_t *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( MODULE_NETWORK_YX_limit * sizeof( uintptr_t ) ) >> STD_SHIFT_PAGE );
	module_network_tx_base_address = (uint64_t *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( MODULE_NETWORK_YX_limit * sizeof( uintptr_t ) ) >> STD_SHIFT_PAGE );

	// share incomming function
	kernel -> network_rx = (void *) module_network_rx;
	kernel -> network_tx = (void *) module_network_tx;

	// create port table
	module_network_port_table = (int64_t *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( MODULE_NETWORK_PORT_limit * sizeof( int64_t ) ) >> STD_SHIFT_PAGE );

	// assign area for connection sockets
	module_network_socket = (struct MODULE_NETWORK_STRUCTURE_SOCKET *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( MODULE_NETWORK_SOCKET_limit * sizeof( struct MODULE_NETWORK_STRUCTURE_SOCKET ) ) >> STD_SHIFT_PAGE );
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

struct MODULE_NETWORK_STRUCTURE_SOCKET *module_network_socket_open( void ) {
	// block access to socket list
	MACRO_LOCK( module_network_socket_semaphore );

	// search for closed socket
	for( uint64_t i = 0; i < MODULE_NETWORK_SOCKET_limit; i++ ) {
		// socket already in use?
		if( module_network_socket[ i ].pid ) continue;	// yes, leave it

		// register socket for current task
		module_network_socket[ i ].pid = kernel -> task_pid();

		// unlock
		MACRO_UNLOCK( module_network_socket_semaphore );

		// return socket pointer
		return (struct MODULE_NETWORK_STRUCTURE_SOCKET *) &module_network_socket[ i ];
	}

	// unlock
	MACRO_UNLOCK( module_network_socket_semaphore );

	// no available sockets for use
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