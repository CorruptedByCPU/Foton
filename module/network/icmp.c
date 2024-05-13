/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void module_network_icmp( struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length ) {
	// properties of IPv4 header
	struct MODULE_NETWORK_STRUCTURE_HEADER_IPV4 *ipv4 = (struct MODULE_NETWORK_STRUCTURE_HEADER_IPV4 *) ((uintptr_t) ethernet + sizeof( struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET ));

	// IPv4 header length
	uint16_t ipv4_header_length = (ipv4 -> version_and_header_length & 0x0F) << STD_SHIFT_4;

	// properties of ICMP header
	struct MODULE_NETWORK_STRUCTURE_HEADER_ICMP *icmp = (struct MODULE_NETWORK_STRUCTURE_HEADER_ICMP *) ((uintptr_t) ethernet + sizeof( struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET ) + ipv4_header_length);

	// it's an answer?
	if( icmp -> type == MODULE_NETWORK_HEADER_ICMP_TYPE_REPLY ) {
		// search for open socket
		for( uint64_t i = 0; i < MODULE_NETWORK_SOCKET_limit; i++ ) {
			// socket with ICMP protocol?
			if( module_network_socket_list[ i ].protocol != STD_NETWORK_PROTOCOL_icmp ) continue;	// no

			// socket connected to target by MAC?
			for( uint8_t j = 0; j < 6; j++ ) if( module_network_socket_list[ j ].ethernet_mac[ j ] != kernel -> network_interface.ethernet_mac[ j ] ) continue;	// no

			// socket connected to target by IPv4?
			if( module_network_socket_list[ i ].ipv4_target != ipv4 -> local ) continue;	// no

			// socket IPv4 type is ICMP?
			if( module_network_socket_list[ i ].ipv4_protocol != MODULE_NETWORK_HEADER_IPV4_PROTOCOL_icmp ) continue;	// no

			// seems to be correct socket :)

			// move data content at beginning of area
			uint8_t *data = (uint8_t *) icmp;
			uint8_t *rewrite = (uint8_t *) ethernet;
			for( uint64_t j = 0; j < length - ((uintptr_t) icmp - (uintptr_t) ethernet); j++ ) rewrite[ j ] = data[ j ];

			// register inside socket
			module_network_data_in( (struct MODULE_NETWORK_STRUCTURE_SOCKET *) &module_network_socket_list[ i ], (uintptr_t) rewrite | (length - ((uintptr_t) icmp - (uintptr_t) ethernet)) );

			// IPv4 frame content transferred to process owning socket
			return;
		}

		// not found, ignore packet
		return;
	}

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