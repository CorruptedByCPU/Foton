/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t kernel_network_udp( struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length ) {
	// properties of IPv4 header
	struct KERNEL_NETWORK_STRUCTURE_HEADER_IPV4 *ipv4 = (struct KERNEL_NETWORK_STRUCTURE_HEADER_IPV4 *) ((uintptr_t) ethernet + sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET ));

	// IPv4 header length
	uint16_t ipv4_header_length = (ipv4 -> version_and_header_length & 0x0F) << STD_SHIFT_4;

	// properties of TCP header
	struct KERNEL_NETWORK_STRUCTURE_HEADER_UDP *udp = (struct KERNEL_NETWORK_STRUCTURE_HEADER_UDP *) ((uintptr_t) ipv4 + ipv4_header_length);

	// search for corresponding socket
	for( uint64_t i = 0; i < KERNEL_NETWORK_SOCKET_limit; i++ ) {
		// properties of socket
		struct KERNEL_NETWORK_STRUCTURE_SOCKET *socket = (struct KERNEL_NETWORK_STRUCTURE_SOCKET *) &kernel -> network_socket_list[ i ];

		// designed port?
		if( udp -> target != MACRO_ENDIANNESS_WORD( socket -> port_local ) ) continue;	// no

		// amount of data
		uint64_t bytes = length - ((uintptr_t) udp - (uintptr_t) ethernet) - sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_UDP );

		// move data content at beginning of area
		uint8_t *data = (uint8_t *) udp + sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_UDP );
		uint8_t *rewrite = (uint8_t *) ethernet;
		for( uint64_t j = 0; j < bytes; j++ ) rewrite[ j ] = data[ j ];

		// register inside socket
		kernel_network_data_in( socket, (uintptr_t) rewrite | bytes );

		// UDP frame content transferred to process owning socket
		return FALSE;
	}

	// release frame
	return TRUE;
}

void kernel_network_udp_exit( struct KERNEL_NETWORK_STRUCTURE_SOCKET *socket, uint8_t *data, uint64_t length ) {
	// align data length to WORD
	if( length % STD_SIZE_WORD_byte ) length++;

	// allocate area for ethernet/udp frame
	struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet = (struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET *) kernel_memory_alloc( TRUE );

	// properties of IPv4 header
	struct KERNEL_NETWORK_STRUCTURE_HEADER_IPV4 *ipv4 = (struct KERNEL_NETWORK_STRUCTURE_HEADER_IPV4 *) ((uintptr_t) ethernet + sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET ));

	// default IPv4 header properties
	ipv4 -> version_and_header_length = KERNEL_NETWORK_HEADER_IPV4_VERSION_AND_HEADER_LENGTH_default;

	// IPv4 header length
	uint16_t ipv4_header_length = (ipv4 -> version_and_header_length & 0x0F) << STD_SHIFT_4;

	// properties of UDP header
	struct KERNEL_NETWORK_STRUCTURE_HEADER_UDP *udp = (struct KERNEL_NETWORK_STRUCTURE_HEADER_UDP *) ((uintptr_t) ipv4 + ipv4_header_length);

	// copy data
	uint8_t *udp_data = (uint8_t *) ((uintptr_t) udp + sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_UDP ));
	for( uint16_t i = 0; i < length; i++ ) udp_data[ i ] = data[ i ];

	// encapsulate UDP frame and send
	kernel_network_udp_encapsulate( socket, ethernet, sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_UDP ) + length );
}

void kernel_network_udp_encapsulate( struct KERNEL_NETWORK_STRUCTURE_SOCKET *socket, struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length ) {
	// properties of UDP haeder
	struct KERNEL_NETWORK_STRUCTURE_HEADER_UDP *udp = (struct KERNEL_NETWORK_STRUCTURE_HEADER_UDP *) ((uintptr_t) ethernet + sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET ) + sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_IPV4 ));
	udp -> local = MACRO_ENDIANNESS_WORD( socket -> port_local );
	udp -> target = MACRO_ENDIANNESS_WORD( socket -> port_target );
	udp -> length = MACRO_ENDIANNESS_WORD( (length + sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_UDP)) );

	// properties of UDP Pseudo header
	struct KERNEL_NETWORK_STRUCTURE_HEADER_PSEUDO *pseudo = (struct KERNEL_NETWORK_STRUCTURE_HEADER_PSEUDO *) ((uintptr_t) udp - sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_PSEUDO ));
	pseudo -> local = MACRO_ENDIANNESS_DWORD( kernel -> network_interface.ipv4_address );
	pseudo -> target = socket -> ipv4_target;
	pseudo -> reserved = EMPTY;	// always
	pseudo -> protocol = KERNEL_NETWORK_HEADER_IPV4_PROTOCOL_udp;
	pseudo -> length = udp -> length;

	// calculate checksum
	udp -> checksum = EMPTY;	// always
	udp -> checksum = kernel_network_checksum( (uint16_t *) pseudo, sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_PSEUDO ) + MACRO_ENDIANNESS_WORD( udp -> length ) );

	// wrap data into a IPv4 frame and send
	kernel_network_ipv4_encapsulate( socket, ethernet, length + sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_UDP ) );
}