/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t kernel_network_tcp( struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length ) {
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

		// ACK
		if( tcp -> flags & KERNEL_NETWORK_HEADER_TCP_FLAG_ACK ) {
			// remove ACK flag if not with SYN
			if( ! (tcp -> flags & KERNEL_NETWORK_HEADER_TCP_FLAG_SYN) ) tcp -> flags ^= KERNEL_NETWORK_HEADER_TCP_FLAG_ACK;

			// sequence malform?
			if( socket -> tcp_acknowledgment_required && tcp -> acknowledgment != MACRO_ENDIANNESS_DWORD( socket -> tcp_acknowledgment_required ) ) {
				// TODO: something nasty... like RST
				break;
			}

			// answer to out request?
			if( socket -> tcp_acknowledgment_required ) {
				// synchronize sequence
				socket -> tcp_sequence = socket -> tcp_acknowledgment_required;

				// nothing to do
				socket -> tcp_acknowledgment_required = EMPTY;
			}

			// acquire target value
			socket -> tcp_acknowledgment = MACRO_ENDIANNESS_DWORD( tcp -> sequence );

			// update keep-alive timeout to ~1 hours
			socket -> tcp_keep_alive = kernel -> time_unit + KERNEL_NETWORK_HEADER_TCP_KEEP_ALIVE_timeout;
		}

		// SYN
		if( tcp -> flags & KERNEL_NETWORK_HEADER_TCP_FLAG_SYN ) {
			// remove SYN flag
			tcp -> flags ^= KERNEL_NETWORK_HEADER_TCP_FLAG_SYN;

			// we didn't start connection?
			if( ! (socket -> tcp_flags & KERNEL_NETWORK_HEADER_TCP_FLAG_SYN ) ) break;	// nothing to do...

			// change socket status to connected
			socket -> tcp_flags = KERNEL_NETWORK_HEADER_TCP_FLAG_ACK;

			// preserve connection properties
			socket -> tcp_acknowledgment = MACRO_ENDIANNESS_DWORD( tcp -> sequence ) + 1;

			// update keep-alive timeout to ~1 hours
			socket -> tcp_keep_alive = kernel -> time_unit + KERNEL_NETWORK_HEADER_TCP_KEEP_ALIVE_timeout;

			// do not respond to ACK response
			if( tcp -> flags != KERNEL_NETWORK_HEADER_TCP_FLAG_ACK ) {
				// remove ACK flag
				tcp -> flags ^= KERNEL_NETWORK_HEADER_TCP_FLAG_ACK;

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

		// PSH
		if( tcp -> flags & KERNEL_NETWORK_HEADER_TCP_FLAG_PSH ) {
			// remove PSH flag
			tcp -> flags ^= KERNEL_NETWORK_HEADER_TCP_FLAG_PSH;

			// amount of data
			uint64_t bytes = length - ((uintptr_t) tcp - (uintptr_t) ethernet) - sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_TCP );

			// IPv4 header length
			uint16_t tcp_header_length = tcp -> header_length >> STD_SHIFT_4;

			// move data content at beginning of area
			uint8_t *data = (uint8_t *) tcp + tcp_header_length;
			uint8_t *rewrite = (uint8_t *) ethernet;
			for( uint64_t j = 0; j < bytes; j++ ) rewrite[ j ] = data[ j ];

			// register inside socket
			kernel_network_data_in( socket, (uintptr_t) rewrite | bytes );

			// IPv4 frame content transferred to process owning socket
			return FALSE;
		}

		// FIN
		if( tcp -> flags & KERNEL_NETWORK_HEADER_TCP_FLAG_FIN ) {
			// remove FIN flag
			tcp -> flags ^= KERNEL_NETWORK_HEADER_TCP_FLAG_FIN;

			// connection closed
			socket -> tcp_flags = KERNEL_NETWORK_HEADER_TCP_FLAG_FIN;

			// accept connection close
			socket -> tcp_acknowledgment++;

			// update keep-alive timeout to ~1 hours
			socket -> tcp_keep_alive = kernel -> time_unit + KERNEL_NETWORK_HEADER_TCP_KEEP_ALIVE_timeout;

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

		// RST
		if( tcp -> flags & KERNEL_NETWORK_HEADER_TCP_FLAG_RST ) {
			// remove RST flag
			tcp -> flags ^= KERNEL_NETWORK_HEADER_TCP_FLAG_RST;

			// connection terminated
			socket -> tcp_flags = KERNEL_NETWORK_HEADER_TCP_FLAG_RST;
		}

		// TCP frame parsed?
		if( ! tcp -> flags ) return TRUE;	// yes
	}

	// release frame
	return TRUE;
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

void kernel_network_tcp_exit( struct KERNEL_NETWORK_STRUCTURE_SOCKET *socket, uint8_t *data, uint16_t length ) {
	// align data length to WORD
	if( length % STD_SIZE_WORD_byte ) length++;

	// wait for socket ready
	while( socket -> tcp_flags != KERNEL_NETWORK_HEADER_TCP_FLAG_ACK ) kernel_time_sleep( TRUE );

	// begin connection/synchronization
	socket -> tcp_flags = KERNEL_NETWORK_HEADER_TCP_FLAG_PSH | KERNEL_NETWORK_HEADER_TCP_FLAG_ACK;

	// sending X Bytes
	socket -> tcp_acknowledgment_required = socket -> tcp_sequence + length;

	// PSH valid ~1 second
	socket -> tcp_keep_alive = kernel -> time_unit + DRIVER_RTC_Hz;

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
	tcp -> acknowledgment	= MACRO_ENDIANNESS_DWORD( socket -> tcp_acknowledgment );
	tcp -> header_length	= KERNEL_NETWORK_HEADER_TCP_HEADER_LENGTH_default;

	// copy data
	uint8_t *tcp_data = (uint8_t *) ((uintptr_t) tcp + sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_TCP ));
	for( uint16_t i = 0; i < length; i++ ) tcp_data[ i ] = data[ i ];

	// encapsulate TCP frame and send
	kernel_network_tcp_encapsulate( socket, ethernet, sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_TCP ) + length );
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

			// TCP socket waiting for initialization or previous initialization is outdated
			if( socket -> flags & KERNEL_NETWORK_SOCKET_FLAG_init || (socket -> tcp_flags == KERNEL_NETWORK_HEADER_TCP_FLAG_SYN && socket -> tcp_keep_alive < kernel -> time_unit) ) {
				// start connection initialiation
				socket -> flags &= ~KERNEL_NETWORK_SOCKET_FLAG_init;

				// set initial socket configuration of TCP protocol

				// begin connection/synchronization
				socket -> tcp_flags = KERNEL_NETWORK_HEADER_TCP_FLAG_SYN;

				// generate new sequence number
				socket -> tcp_sequence = EMPTY;	// DEBUG
				socket -> tcp_acknowledgment_required = socket -> tcp_sequence + 1;

				// default window size
				socket -> tcp_window_size = KERNEL_NETWORK_HEADER_TCP_WINDOW_SIZE_default;

				// SYN valid for ~3 second
				socket -> tcp_keep_alive = kernel -> time_unit + (DRIVER_RTC_Hz * 3);

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

			// TCP socket waiting for initialization or previous initialization is outdated
			if( socket -> tcp_flags == KERNEL_NETWORK_HEADER_TCP_FLAG_ACK && socket -> tcp_keep_alive < kernel -> time_unit) {
				// request same sequence number
				socket -> tcp_acknowledgment_required	= socket -> tcp_sequence;

				// ACK valid for ~1 second
				socket -> tcp_keep_alive = kernel -> time_unit + DRIVER_RTC_Hz;

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
				tcp -> sequence		= MACRO_ENDIANNESS_DWORD( (socket -> tcp_sequence - 1) );
				tcp -> acknowledgment	= MACRO_ENDIANNESS_DWORD( (socket -> tcp_acknowledgment - 1) );
				tcp -> header_length	= KERNEL_NETWORK_HEADER_TCP_HEADER_LENGTH_default;

				// encapsulate TCP frame and send
				kernel_network_tcp_encapsulate( socket, ethernet, sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_TCP ) );
			}
		}
	}
}