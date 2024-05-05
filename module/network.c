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
		// frame for translation?
		if( ! module_network_rx_limit ) continue;	// nope

		// properties of first header
		struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet = (struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET *) (*module_network_rx_base_address & STD_PAGE_mask);

		// release frame area?
		uint8_t release = TRUE;	// yes, why not?

		// choose action
		switch( ethernet -> type ) {
			case MODULE_NETWORK_HEADER_ETHERNET_TYPE_arp: {
				// debug
				kernel -> log( (uint8_t *) "ARP packet.\n" );

				// parse as ARP frame
				release = module_network_arp( ethernet, *module_network_rx_base_address & ~STD_PAGE_mask );

				// done
				break;
			}

			case MODULE_NETWORK_HEADER_ETHERNET_TYPE_ipv4: {
				// debug
				kernel -> log( (uint8_t *) "IPv4 packet" );

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

uint8_t module_network_arp( struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length ) {
	// properties of ARP header
	struct MODULE_NETWORK_STRUCTURE_HEADER_ARP *arp = (struct MODULE_NETWORK_STRUCTURE_HEADER_ARP *) ((uintptr_t) ethernet + sizeof( struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET ));

	// inquiry about our IPv4 address?
	if( arp -> operation == MODULE_NETWORK_HEADER_ARP_OPERATION_request && arp -> target_ipv4 != MACRO_ENDIANNESS_DWORD( kernel -> network_interface.ipv4_address ) ) return TRUE;	// no, ignore message and release frame area

	//----------------------------------------------------------------------

	// open new socket for this task
	struct MODULE_NETWORK_STRUCTURE_SOCKET *socket = module_network_socket();

	// set socket properties

	// protocol
	socket -> protocol = STD_NETWORK_PROTOCOL_arp;

	// target IPv4 address
	socket -> ipv4_target = arp -> source_ipv4;

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
	arp -> target_ipv4 = socket -> ipv4_target;

	// set source IPv4
	arp -> source_ipv4 = MACRO_ENDIANNESS_DWORD( kernel -> network_interface.ipv4_address );

	// encapsulate ARP frame and send
	module_network_ethernet_encapsulate( socket, ethernet, sizeof( struct MODULE_NETWORK_STRUCTURE_HEADER_ARP ) );

	// frame transferred to driver
	return FALSE;
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

void module_network_ethernet_encapsulate( struct MODULE_NETWORK_STRUCTURE_SOCKET *socket, struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length ) {
	// set target and host MAC addresses
	for( uint8_t i = 0; i < 6; i++ ) ethernet -> target[ i ] = socket -> ethernet_mac[ i ];
	for( uint8_t i = 0; i < 6; i++ ) ethernet -> source[ i ] = kernel -> network_interface.ethernet_mac[ i ];

	// set type of Ethernet header
	switch( socket -> protocol ) {
		case STD_NETWORK_PROTOCOL_arp: { ethernet -> type = MODULE_NETWORK_HEADER_ETHERNET_TYPE_arp; break; }
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

	// if socket was type of ARP or ICMP
	if( socket -> protocol == STD_NETWORK_PROTOCOL_arp || socket -> protocol == STD_NETWORK_PROTOCOL_icmp ) socket -> pid = EMPTY;	// release it
}

void module_network_icmp( struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length ) {
	// properties of IPv4 header
	struct MODULE_NETWORK_STRUCTURE_HEADER_IPV4 *ipv4 = (struct MODULE_NETWORK_STRUCTURE_HEADER_IPV4 *) ((uintptr_t) ethernet + sizeof( struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET ));

	// IPv4 header length
	uint16_t ipv4_header_length = (ipv4 -> version_and_header_length & 0x0F) << STD_SHIFT_4;

	// properties of ICMP header
	struct MODULE_NETWORK_STRUCTURE_HEADER_ICMP *icmp = (struct MODULE_NETWORK_STRUCTURE_HEADER_ICMP *) ((uintptr_t) ethernet + sizeof( struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET ) + ipv4_header_length);

	//----------------------------------------------------------------------

	// open new socket for this task
	struct MODULE_NETWORK_STRUCTURE_SOCKET *socket = module_network_socket();

	// cannot open socket?
	if( ! socket ) return;	// ignore request

	// set socket properties

	// socket protocol
	socket -> protocol = STD_NETWORK_PROTOCOL_icmp;

	// target MAC address
	for( uint8_t i = 0; i < 6; i++ ) socket -> ethernet_mac[ i ] = ethernet -> source[ i ];

	// ethernet type
	socket -> ethernet_type = MODULE_NETWORK_HEADER_ETHERNET_TYPE_ipv4;

	// target ID transmission
	socket -> ipv4_id = ipv4 -> id;

	// target TTL decreased value
	socket -> ipv4_ttl = --ipv4 -> ttl;

	// communication protocol
	socket -> ipv4_protocol = MODULE_NETWORK_HEADER_IPV4_PROTOCOL_icmp;

	// target IPv4 address
	socket -> ipv4_target = ipv4 -> local;

	// socket configured, activate
	socket -> flags = MODULE_NETWORK_SOCKET_FLAG_active;

	//----------------------------------------------------------------------

	// ICMP frame length
	uint16_t icmp_frame_length = MACRO_ENDIANNESS_WORD( ipv4 -> length ) - ipv4_header_length;

	// change ICMP content to answer
	icmp -> type = MODULE_NETWORK_HEADER_ICMP_TYPE_REPLY;

	// calculate checksum
	icmp -> checksum = EMPTY;	// always
	icmp -> checksum = module_network_checksum( (uint16_t *) ((uintptr_t) ethernet + sizeof( struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET ) + ipv4_header_length), icmp_frame_length );

	// encapsulate ICMP frame and send
	module_network_ipv4_encapsulate( socket, ethernet, icmp_frame_length );
}

void module_network_init( void ) {
	// assign area for incomming/outgoing frames
	module_network_rx_base_address = (uint64_t *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( MODULE_NETWORK_YX_limit * sizeof( uintptr_t ) ) >> STD_SHIFT_PAGE );
	module_network_tx_base_address = (uint64_t *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( MODULE_NETWORK_YX_limit * sizeof( uintptr_t ) ) >> STD_SHIFT_PAGE );

	// share in/out frames functions
	kernel -> network_rx = (void *) module_network_rx;
	kernel -> network_tx = (void *) module_network_tx;

	// share send/receive functions
	kernel -> network_send = (void *) module_network_send;

	// create port table
	module_network_port_table = (int64_t *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( MODULE_NETWORK_PORT_limit * sizeof( int64_t ) ) >> STD_SHIFT_PAGE );

	// share port function
	kernel -> network_port = (void *) module_network_port;

	// assign area for connection sockets
	module_network_socket_list = (struct MODULE_NETWORK_STRUCTURE_SOCKET *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( MODULE_NETWORK_SOCKET_limit * sizeof( struct MODULE_NETWORK_STRUCTURE_SOCKET ) ) >> STD_SHIFT_PAGE );

	// share socket function and offset
	kernel -> network_socket = (void *) module_network_socket;
	kernel -> network_socket_offset = (uintptr_t) module_network_socket_list;

	// open dummy socket, as socket with ID 0, cannot be used
	module_network_socket();
}

uint8_t module_network_ipv4( struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length ) {
	// properties of IPv4 header
	struct MODULE_NETWORK_STRUCTURE_HEADER_IPV4 *ipv4 = (struct MODULE_NETWORK_STRUCTURE_HEADER_IPV4 *) ((uintptr_t) ethernet + sizeof( struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET ));

	// inquiry about our IPv4 address or multicast?
	if( ipv4 -> target != MACRO_ENDIANNESS_DWORD( kernel -> network_interface.ipv4_address ) && ipv4 -> target != module_network_multicast_address ) {
		// debug
		kernel -> log( (uint8_t *) ", not for us.\n" );

		return TRUE;	// ignore
	}

	// IPv4 header length
	uint16_t ipv4_header_length = (ipv4 -> version_and_header_length >> STD_SHIFT_4) << STD_SHIFT_32;

	// // packet dump
	// uint8_t *memory = (uint8_t *) ethernet;
	// for( uint8_t y = 0; y < (length / 16) + 1 ; y++ ) kernel -> log( (uint8_t *) "%8X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X\n", (uintptr_t) &memory[ y * 16 ], memory[ (y * 16) + 0 ], memory[ (y * 16) + 1 ], memory[ (y * 16) + 2 ], memory[ (y * 16) + 3 ], memory[ (y * 16) + 4 ], memory[ (y * 16) + 5 ], memory[ (y * 16) + 6 ], memory[ (y * 16) + 7 ], memory[ (y * 16) + 8 ], memory[ (y * 16) + 9 ], memory[ (y * 16) + 10 ], memory[ (y * 16) + 11 ], memory[ (y * 16) + 12 ], memory[ (y * 16) + 13 ], memory[ (y * 16) + 14 ], memory[ (y * 16) + 15 ]); kernel -> log( (uint8_t *) "\n" );

	// choose action
	switch( ipv4 -> protocol ) {
		case MODULE_NETWORK_HEADER_IPV4_PROTOCOL_icmp: {
			// debug
			kernel -> log( (uint8_t *) " with ICMP frame.\n" );

			// parse as ICMP frame
			module_network_icmp( ethernet, length );

			// done
			break;
		}

		case MODULE_NETWORK_HEADER_IPV4_PROTOCOL_udp: {
			// debug
			kernel -> log( (uint8_t *) " with UDP frame.\n" );

			// module_network_udp( ethernet, length );
		}

		case MODULE_NETWORK_HEADER_IPV4_PROTOCOL_tcp: {
			// debug
			kernel -> log( (uint8_t *) " with TCP frame.\n" );

			// module_network_tcp( ethernet, length );
		}
	}

	// debug
	return FALSE;
}

void module_network_ipv4_encapsulate( struct MODULE_NETWORK_STRUCTURE_SOCKET *socket, struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length ) {
	// properties of IPv4 header
	struct MODULE_NETWORK_STRUCTURE_HEADER_IPV4 *ipv4 = (struct MODULE_NETWORK_STRUCTURE_HEADER_IPV4 *) ((uintptr_t) ethernet + sizeof( struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET ));
	ipv4 -> version_and_header_length = MODULE_NETWORK_HEADER_IPV4_VERSION_AND_HEADER_LENGTH_default;
	ipv4 -> ecn = MODULE_NETWORK_HEADER_IPV4_ECN_default;
	ipv4 -> length = MACRO_ENDIANNESS_WORD( (length + ((MODULE_NETWORK_HEADER_IPV4_VERSION_AND_HEADER_LENGTH_default & 0x0F) << STD_SHIFT_4)) );
	ipv4 -> id = socket -> ipv4_id;
	ipv4 -> flags_and_offset = MODULE_NETWORK_HEADER_IPV4_FLAGS_AND_OFFSET_default;
	ipv4 -> ttl = MODULE_NETWORK_HEADER_IPV4_TTL_default;
	ipv4 -> protocol = socket -> ipv4_protocol;
	ipv4 -> local = MACRO_ENDIANNESS_DWORD( kernel -> network_interface.ipv4_address );
	ipv4 -> target = socket -> ipv4_target;

	// calculate checksum
	ipv4 -> checksum = EMPTY;	// always
	ipv4 -> checksum = module_network_checksum( (uint16_t *) ((uintptr_t) ethernet + sizeof( struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET )), sizeof( struct MODULE_NETWORK_STRUCTURE_HEADER_IPV4 ) );

	// wrap data into a Ethernet frame and send
	module_network_ethernet_encapsulate( socket, ethernet, length + ((MODULE_NETWORK_HEADER_IPV4_VERSION_AND_HEADER_LENGTH_default & 0x0F) << STD_SHIFT_4) );
}

uint8_t module_network_port( uint16_t port ) {
	// port overflow?
	if( MODULE_NETWORK_PORT_limit <= port ) return FALSE;	// yes

	// block access to port table
	MACRO_LOCK( module_network_port_semaphore );

	// port is free to use?
	if( ! module_network_port_table[ port ] ) {
		// assign task to it
		module_network_port_table[ port ] = kernel -> task_pid();

		// unlock
		MACRO_UNLOCK( module_network_port_semaphore );

		// port assigned
		return TRUE;
	}

	// unlock
	MACRO_UNLOCK( module_network_port_semaphore );

	// port already in use
	return FALSE;
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
		case STD_NETWORK_PROTOCOL_udp: { module_network_udp( (struct MODULE_NETWORK_STRUCTURE_SOCKET *) &module_network_socket_list[ socket ], data, length ); break; }
	}

	// sent
	return EMPTY;
}

struct MODULE_NETWORK_STRUCTURE_SOCKET *module_network_socket( void ) {
	// block access to socket list
	MACRO_LOCK( module_network_socket_semaphore );

	// search for closed socket
	for( uint64_t i = 0; i < MODULE_NETWORK_SOCKET_limit; i++ ) {
		// socket already in use?
		if( module_network_socket_list[ i ].pid ) continue;	// yes, leave it

		// register socket for current task
		module_network_socket_list[ i ].pid = kernel -> task_pid();

		// unlock
		MACRO_UNLOCK( module_network_socket_semaphore );

		// return socket pointer
		return (struct MODULE_NETWORK_STRUCTURE_SOCKET *) &module_network_socket_list[ i ];
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

void module_network_udp( struct MODULE_NETWORK_STRUCTURE_SOCKET *socket, uint8_t *data, uint64_t length ) {
	// amount of data already sent
	uint16_t data_sent = EMPTY;

	// send data in parts of 512 Bytes
	while( data_sent < length ) {
		// prepare ethernet header
		struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet = (struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET *) kernel -> memory_alloc( TRUE );

		// load data to UDP data frame
		uint8_t *source = (uint8_t *) &data[ data_sent ];
		uint8_t	*target = (uint8_t *) ((uintptr_t) ethernet + sizeof( struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET ) + sizeof( struct MODULE_NETWORK_STRUCTURE_HEADER_IPV4 ) + sizeof( struct MODULE_NETWORK_STRUCTURE_HEADER_UDP ) );

		// length of data part
		uint16_t data_length = 512;
		if( length < 512 ) data_length = length;

		// round up length to parity
		if( data_length % 2 ) data_length++;

		// copy part of data for send
		for( uint64_t i = 0; i < data_length; i++ ) target[ i ] = source[ i ];

		// wrap data into a UDP frame and send
		module_network_udp_encapsulate( socket, ethernet, data_length );

		// part of data sent
		data_sent += data_length;
	}
}

void module_network_udp_encapsulate( struct MODULE_NETWORK_STRUCTURE_SOCKET *socket, struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length ) {
	// properties of UDP haeder
	struct MODULE_NETWORK_STRUCTURE_HEADER_UDP *udp = (struct MODULE_NETWORK_STRUCTURE_HEADER_UDP *) ((uintptr_t) ethernet + sizeof( struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET ) + sizeof( struct MODULE_NETWORK_STRUCTURE_HEADER_IPV4 ));
	udp -> local = socket -> port_local;
	udp -> target = socket -> port_target;
	udp -> length = MACRO_ENDIANNESS_WORD( (length + sizeof( struct MODULE_NETWORK_STRUCTURE_HEADER_UDP)) );

	// properties of UDP Pseudo header
	struct MODULE_NETWORK_STRUCTURE_HEADER_PSEUDO *pseudo = (struct MODULE_NETWORK_STRUCTURE_HEADER_PSEUDO *) ((uintptr_t) udp - sizeof( struct MODULE_NETWORK_STRUCTURE_HEADER_PSEUDO ));
	pseudo -> local = MACRO_ENDIANNESS_DWORD( kernel -> network_interface.ipv4_address );
	pseudo -> target = socket -> ipv4_target;
	pseudo -> reserved = EMPTY;	// always
	pseudo -> protocol = MODULE_NETWORK_HEADER_IPV4_PROTOCOL_udp;
	pseudo -> length = udp -> length;

	// calculate checksum
	udp -> checksum = EMPTY;	// always
	udp -> checksum = module_network_checksum( (uint16_t *) pseudo, sizeof( struct MODULE_NETWORK_STRUCTURE_HEADER_PSEUDO ) + MACRO_ENDIANNESS_WORD( udp -> length ) );

	// wrap data into a IPv4 frame and send
	module_network_ipv4_encapsulate( socket, ethernet, length + sizeof( struct MODULE_NETWORK_STRUCTURE_HEADER_UDP ) );
}
