/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_network_ethernet_encapsulate( struct KERNEL_NETWORK_STRUCTURE_SOCKET *socket, struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length ) {
	// set target and host MAC addresses
	for( uint8_t i = 0; i < 6; i++ ) ethernet -> target[ i ] = socket -> ethernet_mac[ i ];
	for( uint8_t i = 0; i < 6; i++ ) ethernet -> source[ i ] = kernel -> network_interface.ethernet_mac[ i ];

	// set type of Ethernet header
	switch( socket -> protocol ) {
		case STD_NETWORK_PROTOCOL_arp: { ethernet -> type = KERNEL_NETWORK_HEADER_ETHERNET_TYPE_arp; break; }
		default: { ethernet -> type = KERNEL_NETWORK_HEADER_ETHERNET_TYPE_ipv4; break; }
	}

	// block access to stack modification
	MACRO_LOCK( kernel -> network_tx_semaphore );

	// TODO, make sure that frame was placed inside transfer queue

	// free entry available?
	if( kernel -> network_tx_limit < KERNEL_NETWORK_YX_limit )
		// insert frame properties
		kernel -> network_tx_base_address[ kernel -> network_tx_limit++ ] = ((uintptr_t) ethernet & ~KERNEL_PAGE_logical) | length + sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET );

	// unlock
	MACRO_UNLOCK( kernel -> network_tx_semaphore );
}