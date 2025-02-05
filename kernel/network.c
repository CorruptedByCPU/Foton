/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// functions / procedures
	//----------------------------------------------------------------------
	#include	"network/arp.c"
	#include	"network/ethernet.c"
	#include	"network/icmp.c"
	#include	"network/ip.c"
	#include	"network/socket.c"
	#include	"network/tcp.c"
	#include	"network/udp.c"

void kernel_network( void ) {
	// never ending story
	while( TRUE ) {
		// frame for translation?
		if( ! kernel -> network_rx_limit ) { kernel_time_sleep( TRUE ); continue; }	// nope 

		// properties of first header
		struct KERNEL_STRUCTURE_NETWORK_HEADER_ETHERNET *ethernet = (struct KERNEL_STRUCTURE_NETWORK_HEADER_ETHERNET *) (*kernel -> network_rx_base_address & STD_PAGE_mask);

		// release frame area?
		uint8_t release = TRUE;	// yes, why not?

		// choose action
		switch( MACRO_ENDIANNESS_WORD( ethernet -> type ) ) {
			case KERNEL_NETWORK_HEADER_ETHERNET_TYPE_arp: {
				// parse as ARP frame
				release = kernel_network_arp( ethernet, *kernel -> network_rx_base_address & ~STD_PAGE_mask );

				// done
				break;
			}

			case KERNEL_NETWORK_HEADER_ETHERNET_TYPE_ipv4: {
				// parse as IPv4 frame
				release = kernel_network_ipv4( ethernet, *kernel -> network_rx_base_address & ~STD_PAGE_mask );

				// done
				break;
			}
		}

		// release frame area
		if( release ) kernel_memory_release( (uintptr_t) ethernet, TRUE );

		// block access to stack modification
		MACRO_LOCK( kernel -> network_rx_semaphore );

		// remove frame from stack
		for( uint64_t i = 0; i < kernel -> network_rx_limit; i++ ) kernel -> network_rx_base_address[ i ] = kernel -> network_rx_base_address[ i + 1 ];

		// one frame less on stack
		kernel -> network_rx_limit--;

		// unlock
		MACRO_UNLOCK( kernel -> network_rx_semaphore );
	}
}

uint16_t kernel_network_checksum( uint16_t *data, uint16_t length ) {
	// initial checksum value
	uint32_t result = EMPTY;

	// add each chunk of data
	for( uint16_t i = 0; i < length >> STD_SHIFT_2; i++ ) {
		result += (uint16_t) MACRO_ENDIANNESS_WORD( data[ i ] );
	
		// if overflow
		if( result > 0xFFFF ) result = (result & STD_MASK_word) + 1;
	}

	// if result is EMPTY
	if( ! result ) return STD_MAX_unsigned;
	else return ~MACRO_ENDIANNESS_WORD( ((result >> STD_MOVE_WORD) + (result & STD_MASK_word)) );
}

void kernel_network_data_in( struct KERNEL_STRUCTURE_NETWORK_SOCKET *socket, uintptr_t packet ) {
	// block access to stack modification
	MACRO_LOCK( socket -> data_in_semaphore );

	// search for free entry
	for( uint16_t i = 0; i < KERNEL_NETWORK_SOCKET_DATA_limit; i++ ) {
		// free?
		if( socket -> data_in[ i ] ) continue;	// no

		// insert data into entry
		socket -> data_in[ i ] = packet;

		// done
		break;
	}

	// unlock
	MACRO_UNLOCK( socket -> data_in_semaphore );
}

void kernel_network_rx( uintptr_t frame ) {
	// block access to stack modification
	MACRO_LOCK( kernel -> network_rx_semaphore );

	// frame designated for us?
	struct KERNEL_STRUCTURE_NETWORK_HEADER_ETHERNET *ethernet = (struct KERNEL_STRUCTURE_NETWORK_HEADER_ETHERNET *) (frame & STD_PAGE_mask);
	if( (ethernet -> target[ 0 ] == kernel -> network_interface.ethernet_address[ 0 ] && ethernet -> target[ 1 ] == kernel -> network_interface.ethernet_address[ 1 ] && ethernet -> target[ 2 ] == kernel -> network_interface.ethernet_address[ 2 ] && ethernet -> target[ 3 ] == kernel -> network_interface.ethernet_address[ 3 ] && ethernet -> target[ 4 ] == kernel -> network_interface.ethernet_address[ 4 ] && ethernet -> target[ 5 ] == kernel -> network_interface.ethernet_address[ 5 ]) || (ethernet -> target[ 0 ] == 0xFF && ethernet -> target[ 1 ] == 0xFF && ethernet -> target[ 2 ] == 0xFF && ethernet -> target[ 3 ] == 0xFF && ethernet -> target[ 4 ] == 0xFF && ethernet -> target[ 5 ] == 0xFF) ) {
		// rx stack is full?
		if( kernel -> network_rx_limit < KERNEL_NETWORK_YX_limit )	// no
			// add frame to stack
			kernel -> network_rx_base_address[ kernel -> network_rx_limit++ ] = frame;
	}

	// unlock
	MACRO_UNLOCK( kernel -> network_rx_semaphore );
}

int64_t kernel_network_send( int64_t socket, uint8_t *data, uint64_t length ) {
	// choose action
	switch( kernel -> network_socket_list[ socket ].protocol ) {
		case STD_NETWORK_PROTOCOL_icmp: { kernel_network_ipv4_exit( (struct KERNEL_STRUCTURE_NETWORK_SOCKET *) &kernel -> network_socket_list[ socket ], data, length ); break; }
		case STD_NETWORK_PROTOCOL_udp: { kernel_network_udp_exit( (struct KERNEL_STRUCTURE_NETWORK_SOCKET *) &kernel -> network_socket_list[ socket ], data, length ); break; }
		case STD_NETWORK_PROTOCOL_tcp: { kernel_network_tcp_exit( (struct KERNEL_STRUCTURE_NETWORK_SOCKET *) &kernel -> network_socket_list[ socket ], data, length ); break; }
	}

	// sent
	return EMPTY;
}

uintptr_t kernel_network_tx( void ) {
	// tx stack is empty?
	if( ! kernel -> network_tx_limit ) return EMPTY;	// nothing to send

	// retrieve frame properties
	volatile uintptr_t frame = *kernel -> network_tx_base_address;

	// block access to stack modification
	MACRO_LOCK( kernel -> network_tx_semaphore );

	// remove frame from stack
	for( uint64_t i = 0; i < kernel -> network_tx_limit; i++ ) kernel -> network_tx_base_address[ i ] = kernel -> network_tx_base_address[ i + 1 ];

	// one frame sent
	kernel -> network_tx_limit--;

	// unlock
	MACRO_UNLOCK( kernel -> network_tx_semaphore );

	// return frame properties
	return frame;
}
