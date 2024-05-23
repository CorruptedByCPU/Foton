/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_network_tcp( struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length ) {
	// properties of IPv4 header
	struct KERNEL_NETWORK_STRUCTURE_HEADER_IPV4 *ipv4 = (struct KERNEL_NETWORK_STRUCTURE_HEADER_IPV4 *) ((uintptr_t) ethernet + sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET ));

	// IPv4 header length
	uint16_t ipv4_header_length = (ipv4 -> version_and_header_length & 0x0F) << STD_SHIFT_4;

	// properties of TCP header
	struct KERNEL_NETWORK_STRUCTURE_HEADER_TCP *tcp = (struct KERNEL_NETWORK_STRUCTURE_HEADER_TCP *) ((uintptr_t) ipv4 + ipv4_header_length);

	// search for corresponding socket
	for( uint64_t i = 0; i < KERNEL_NETWORK_SOCKET_limit; i++ ) {
		// properties of socket
		struct KERNEL_NETWORK_STRUCTURE_SOCKET *socket = (struct KERNEL_NETWORK_STRUCTURE_SOCKET *) &kernel -> network_socket_list[ i ];

		// designed port?
		if( tcp -> port_local != MACRO_ENDIANNESS_WORD( socket -> port_local ) ) continue;	// no

		// answer to SYN request?
		if( socket -> tcp_flags == KERNEL_NETWORK_HEADER_TCP_FLAG_SYN && tcp -> flags == (KERNEL_NETWORK_HEADER_TCP_FLAG_SYN | KERNEL_NETWORK_HEADER_TCP_FLAG_ACK) ) {	// yes
			// debug
			socket -> tcp_sequence = socket -> tcp_sequence_ack;
		
			// preserve connection properties
			socket -> tcp_acknowledgment = MACRO_ENDIANNESS_DWORD( tcp -> sequence ) + 1;

			// keey-alive timeout
			socket -> tcp_keep_alive = kernel -> time_unit + (1024 * 60);

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
			tcp -> sequence		= MACRO_ENDIANNESS_DWORD( socket -> tcp_sequence );
			tcp -> acknowledgment	= MACRO_ENDIANNESS_DWORD( socket -> tcp_acknowledgment );
			tcp -> header_length	= KERNEL_NETWORK_HEADER_TCP_HEADER_LENGTH_default;
			tcp -> flags		= KERNEL_NETWORK_HEADER_TCP_FLAG_ACK;	// thank you

			// encapsulate TCP frame and send
			kernel_network_tcp_encapsulate( socket, ethernet, sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_TCP ) );
		}
	}
}

void kernel_network_tcp_encapsulate( struct KERNEL_NETWORK_STRUCTURE_SOCKET *socket, struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length ) {
	// properties of IPv4 header
	struct KERNEL_NETWORK_STRUCTURE_HEADER_IPV4 *ipv4 = (struct KERNEL_NETWORK_STRUCTURE_HEADER_IPV4 *) ((uintptr_t) ethernet + sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET ));

	// IPv4 header length
	uint16_t ipv4_header_length = (ipv4 -> version_and_header_length & 0x0F) << STD_SHIFT_4;

	// properties of TCP header
	struct KERNEL_NETWORK_STRUCTURE_HEADER_TCP *tcp = (struct KERNEL_NETWORK_STRUCTURE_HEADER_TCP *) ((uintptr_t) ipv4 + ipv4_header_length);

	// default properties of TCP header
	tcp -> port_source	= MACRO_ENDIANNESS_WORD( socket -> port_local );
	tcp -> port_local	= MACRO_ENDIANNESS_WORD( socket -> port_target );
	tcp -> window_size	= MACRO_ENDIANNESS_WORD( socket -> tcp_window_size );

	// properties of TCP Pseudo header
	struct KERNEL_NETWORK_STRUCTURE_HEADER_PSEUDO *pseudo = (struct KERNEL_NETWORK_STRUCTURE_HEADER_PSEUDO *) ((uintptr_t) tcp - sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_PSEUDO ));
	pseudo -> local = MACRO_ENDIANNESS_DWORD( kernel -> network_interface.ipv4_address );
	pseudo -> target = MACRO_ENDIANNESS_DWORD( socket -> ipv4_target );
	pseudo -> reserved = EMPTY;	// always
	pseudo -> protocol = KERNEL_NETWORK_HEADER_IPV4_PROTOCOL_tcp;
	pseudo -> length = MACRO_ENDIANNESS_WORD( length );

	// calculate checksum
	tcp -> checksum = EMPTY;	// always
	tcp -> checksum = kernel_network_checksum( (uint16_t *) pseudo, sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_PSEUDO ) + length );

	// wrap data into a IPv4 frame and send
	kernel_network_ipv4_encapsulate( socket, ethernet, length );
}

void kernel_network_tcp_thread( void ) {
	// hold the door
	while( TRUE ) {
		// search for socket to initialize
		for( uint64_t i = 0; i < KERNEL_NETWORK_SOCKET_limit; i++ ) {
			// properties of socket
			struct KERNEL_NETWORK_STRUCTURE_SOCKET *socket = (struct KERNEL_NETWORK_STRUCTURE_SOCKET *) &kernel -> network_socket_list[ i ];

			// ignore sockets other than TCP
			if( socket -> protocol != STD_NETWORK_PROTOCOL_tcp ) continue;

			// TCP socket waiting for initialization?
			if( socket -> flags & KERNEL_NETWORK_SOCKET_FLAG_init ) {
				// start connection initialiation
				socket -> flags &= ~KERNEL_NETWORK_SOCKET_FLAG_init;

				// set initial socket configuration of TCP protocol

				// begin connection/synchronization
				socket -> tcp_flags = KERNEL_NETWORK_HEADER_TCP_FLAG_SYN;

				// generate new sequence number
				socket -> tcp_sequence = EMPTY;	// DEBUG
				socket -> tcp_sequence_ack = socket -> tcp_sequence + 1;

				// default window size
				socket -> tcp_window_size = KERNEL_NETWORK_HEADER_TCP_WINDOW_SIZE_default;

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
				tcp -> flags		= socket -> tcp_flags;
				tcp -> sequence		= MACRO_ENDIANNESS_DWORD( socket -> tcp_sequence );
				tcp -> acknowledgment	= EMPTY;	// we do not know it yet
				tcp -> header_length	= KERNEL_NETWORK_HEADER_TCP_HEADER_LENGTH_default;

				// encapsulate TCP frame and send
				kernel_network_tcp_encapsulate( socket, ethernet, sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_TCP ) );
			}

			// TCP socket received synchronization approve?
			if( socket -> tcp_flags == (KERNEL_NETWORK_HEADER_TCP_FLAG_SYN | KERNEL_NETWORK_HEADER_TCP_FLAG_ACK) ) {
				MACRO_DEBUF();
			}
		}
	}
}