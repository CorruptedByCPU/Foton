/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_network_udp_exit( struct KERNEL_NETWORK_STRUCTURE_SOCKET *socket, uint8_t *data, uint64_t length ) {
	// amount of data already sent
	uint16_t data_sent = EMPTY;

	// send data in parts of 512 Bytes
	while( data_sent < length ) {
		// prepare ethernet header
		struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet = (struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET *) kernel_memory_alloc( TRUE );

		// load data to UDP data frame
		uint8_t *source = (uint8_t *) &data[ data_sent ];
		uint8_t	*target = (uint8_t *) ((uintptr_t) ethernet + sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET ) + sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_IPV4 ) + sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_UDP ) );

		// length of data part
		uint16_t data_length = 512;
		if( length < 512 ) data_length = length;

		// round up length to parity
		if( data_length % 2 ) data_length++;

		// copy part of data for send
		for( uint64_t i = 0; i < data_length; i++ ) target[ i ] = source[ i ];

		// wrap data into a UDP frame and send
		kernel_network_udp_encapsulate( socket, ethernet, data_length );

		// part of data sent
		data_sent += data_length;
	}
}

void kernel_network_udp_encapsulate( struct KERNEL_NETWORK_STRUCTURE_SOCKET *socket, struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length ) {
	// properties of UDP haeder
	struct KERNEL_NETWORK_STRUCTURE_HEADER_UDP *udp = (struct KERNEL_NETWORK_STRUCTURE_HEADER_UDP *) ((uintptr_t) ethernet + sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET ) + sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_IPV4 ));
	udp -> local = socket -> port_local;
	udp -> target = socket -> port_target;
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