/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_network_tcp_encapsulate( struct KERNEL_NETWORK_STRUCTURE_SOCKET *socket, struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length ) {
	// properties of IPv4 header
	struct KERNEL_NETWORK_STRUCTURE_HEADER_IPV4 *ipv4 = (struct KERNEL_NETWORK_STRUCTURE_HEADER_IPV4 *) ((uintptr_t) ethernet + sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET ));

	// IPv4 header length
	uint16_t ipv4_header_length = (ipv4 -> version_and_header_length & 0x0F) << STD_SHIFT_4;

	// properties of TCP header
	struct KERNEL_NETWORK_STRUCTURE_HEADER_TCP *tcp = (struct KERNEL_NETWORK_STRUCTURE_HEADER_TCP *) ((uintptr_t) ipv4 + ipv4_header_length);

	// default properties of TCP header
	tcp -> port_source	= socket -> port_local;
	tcp -> port_local	= socket -> port_target;

	// properties of TCP Pseudo header
	struct KERNEL_NETWORK_STRUCTURE_HEADER_PSEUDO *pseudo = (struct KERNEL_NETWORK_STRUCTURE_HEADER_PSEUDO *) ((uintptr_t) tcp - sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_PSEUDO ));
	pseudo -> local = MACRO_ENDIANNESS_DWORD( kernel -> network_interface.ipv4_address );
	pseudo -> target = socket -> ipv4_target;
	pseudo -> reserved = EMPTY;	// always
	pseudo -> protocol = KERNEL_NETWORK_HEADER_IPV4_PROTOCOL_tcp;
	pseudo -> length = MACRO_ENDIANNESS_WORD( (length + sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_UDP)) );

	// calculate checksum
	tcp -> checksum = EMPTY;	// always
	tcp -> checksum = kernel_network_checksum( (uint16_t *) pseudo, sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_PSEUDO ) + MACRO_ENDIANNESS_WORD( pseudo -> length ) );

	// wrap data into a IPv4 frame and send
	kernel_network_ipv4_encapsulate( socket, ethernet, length + sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_UDP ) );
}

void kernel_network_tcp_thread( void ) {
	// hold the door
	while( TRUE ) {
		// search for socket to initialize
		for( uint64_t i = 0; i < KERNEL_NETWORK_SOCKET_limit; i++ ) {
			// TCP socket waiting for initialization?
			if( kernel -> network_socket_list[ i ].protocol != STD_NETWORK_PROTOCOL_tcp && ! (kernel -> network_socket_list[ i ].flags & KERNEL_NETWORK_SOCKET_FLAG_init) ) continue;	// next socket

			// start connection initialiation
			kernel -> network_socket_list[ i ].flags ^= KERNEL_NETWORK_SOCKET_FLAG_init;

			MACRO_DEBUF();

			// allocate area for ethernet/tcp frame
			struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet = (struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET *) kernel_memory_alloc( TRUE );

			// properties of IPv4 header
			struct KERNEL_NETWORK_STRUCTURE_HEADER_IPV4 *ipv4 = (struct KERNEL_NETWORK_STRUCTURE_HEADER_IPV4 *) ((uintptr_t) ethernet + sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET ));

			// default IPv4 header properties
			ipv4 -> version_and_header_length = KERNEL_NETWORK_HEADER_IPV4_VERSION_AND_HEADER_LENGTH_default;

			// IPv4 header length
			uint16_t ipv4_header_length = (ipv4 -> version_and_header_length & 0x0F) << STD_SHIFT_4;

			// properties of TCP header
			struct KERNEL_NETWORK_STRUCTURE_HEADER_TCP *tcp = (struct KERNEL_NETWORK_STRUCTURE_HEADER_TCP *) ((uintptr_t) ipv4 + ipv4_header_length);

			// properties of TCP frame
			tcp -> sequence		= EMPTY;	// for debug, testing
			tcp -> acknowledgment	= EMPTY;	// for debug, testing
			tcp -> header_length	= KERNEL_NETWORK_HEADER_TCP_HEADER_LENGTH_default;
			tcp -> flags		= KERNEL_NETWORK_HEADER_TCP_FLAG_SYN;	// start synchronization
			tcp -> window_size	= KERNEL_NETWORK_HEADER_TCP_WINDOW_SIZE_default;

			// encapsulate TCP frame and send
			kernel_network_tcp_encapsulate( (struct KERNEL_NETWORK_STRUCTURE_SOCKET *) &kernel -> network_socket_list[ i ], ethernet, sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_TCP ) );
		}
	}
}