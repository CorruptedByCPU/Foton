/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t kernel_network_arp( struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length ) {
	// properties of ARP header
	struct KERNEL_NETWORK_STRUCTURE_HEADER_ARP *arp = (struct KERNEL_NETWORK_STRUCTURE_HEADER_ARP *) ((uintptr_t) ethernet + sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET ));

	// reply targeting us?
	if( arp -> operation == MACRO_ENDIANNESS_WORD( KERNEL_NETWORK_HEADER_ARP_OPERATION_answer ) ) {
		// update all sockets with provided IPV4 address
		for( uint64_t i = 0; i < KERNEL_NETWORK_SOCKET_limit; i++ ) {
			// socket with destination IPv4 address?
			if( arp -> source_ipv4 != MACRO_ENDIANNESS_DWORD( kernel -> network_socket_list[ i ].ipv4_target ) ) continue;	// no
		
			// update
			for( uint8_t j = 0; j < 6; j++ ) kernel -> network_socket_list[ i ].ethernet_mac[ j ] = arp -> source_mac[ j ];

			// lease time
			kernel -> network_socket_list[ i ].ethernet_mac_lease = kernel -> time_unit + (300 * DRIVER_RTC_Hz);	// ~5 min
		}

		// answer parsed
		return TRUE;
	}

	// inquiry about our IPv4 address?
	if( MACRO_ENDIANNESS_WORD( arp -> operation ) == KERNEL_NETWORK_HEADER_ARP_OPERATION_request && MACRO_ENDIANNESS_DWORD( arp -> target_ipv4 ) != kernel -> network_interface.ipv4_address ) return TRUE;	// no, ignore message and release frame area

	//----------------------------------------------------------------------

	// open new socket for this task
	struct KERNEL_NETWORK_STRUCTURE_SOCKET *socket = kernel_network_socket();

	// set socket properties

	// protocol
	socket -> protocol = STD_NETWORK_PROTOCOL_arp;

	// target IPv4 address
	socket -> ipv4_target = MACRO_ENDIANNESS_DWORD( arp -> source_ipv4 );

	// target MAC address
	for( uint8_t i = 0; i < 6; i++ ) socket -> ethernet_mac[ i ] = arp -> source_mac[ i ];

	// socket configured, activate
	socket -> flags = KERNEL_NETWORK_SOCKET_FLAG_active;

	//----------------------------------------------------------------------

	// change ARP content to answer
	arp -> operation = MACRO_ENDIANNESS_WORD( KERNEL_NETWORK_HEADER_ARP_OPERATION_answer );

	// set target MAC
	for( uint8_t i = 0; i < 6; i++ ) arp -> target_mac[ i ] = socket -> ethernet_mac[ i ];

	// set source MAC
	for( uint8_t i = 0; i < 6; i++ ) arp -> source_mac[ i ] = kernel -> network_interface.ethernet_mac[ i ];
	
	// set target IPv4
	arp -> target_ipv4 = MACRO_ENDIANNESS_DWORD( socket -> ipv4_target );

	// set source IPv4
	arp -> source_ipv4 = MACRO_ENDIANNESS_DWORD( kernel -> network_interface.ipv4_address );

	// encapsulate ARP frame and send
	kernel_network_ethernet_encapsulate( socket, ethernet, sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_ARP ) );

	// close socket
	kernel_network_socket_close( socket );

	// frame transferred to driver, do not release
	return FALSE;
}

void kernel_network_arp_thread( void ) {
	// open new socket for this thread
	struct KERNEL_NETWORK_STRUCTURE_SOCKET *socket = kernel_network_socket();

	// set socket properties

	// protocol
	socket -> protocol = STD_NETWORK_PROTOCOL_arp;

	// target MAC address
	for( uint8_t i = 0; i < 6; i++ ) socket -> ethernet_mac[ i ] = 0xFF;	// broadcast

	// socket configured, activate
	socket -> flags = KERNEL_NETWORK_SOCKET_FLAG_active;

	// main loop
	while( TRUE ) {
		// analyze sockets properties
		for( uint64_t i = 0; i < KERNEL_NETWORK_SOCKET_limit; i++ ) {
			// resolve IPv4 address?
			if( ! kernel -> network_socket_list[ i ].pid || kernel -> network_socket_list[ i ].protocol == STD_NETWORK_PROTOCOL_arp ) continue;	// no need

			// up to date?
			if( kernel -> network_socket_list[ i ].ethernet_mac_lease > kernel -> time_unit ) continue;	// yes

			//----------------------------------------------------------------------

			// allocate area for ethernet/arp frame
			struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet = (struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET *) kernel_memory_alloc( TRUE );

			// properties of ARP frame
			struct KERNEL_NETWORK_STRUCTURE_HEADER_ARP *arp = (struct KERNEL_NETWORK_STRUCTURE_HEADER_ARP *) ((uintptr_t) ethernet + sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET ) );

			// set ARP properties
			arp -> hardware_type	= MACRO_ENDIANNESS_WORD( KERNEL_NETWORK_HEADER_ARP_HARDWARE_TYPE_ethernet );
			arp -> protocol_type	= MACRO_ENDIANNESS_WORD( KERNEL_NETWORK_HEADER_ARP_PROTOCOL_TYPE_ipv4 );
			arp -> hardware_length	= KERNEL_NETWORK_HEADER_ARP_HARDWARE_LENGTH_mac;
			arp -> protocol_length	= KERNEL_NETWORK_HEADER_ARP_PROTOCOL_LENGTH_ipv4;
			arp -> operation	= MACRO_ENDIANNESS_WORD( KERNEL_NETWORK_HEADER_ARP_OPERATION_request );

			// set source MAC
			for( uint8_t i = 0; i < 6; i++ ) arp -> source_mac[ i ] = kernel -> network_interface.ethernet_mac[ i ];

			// set source IPv4
			arp -> source_ipv4 = MACRO_ENDIANNESS_DWORD( kernel -> network_interface.ipv4_address );

			// set target IPv4
			arp -> target_ipv4 = MACRO_ENDIANNESS_DWORD( kernel -> network_socket_list[ i ].ipv4_target );

			// encapsulate ARP frame and send
			kernel_network_ethernet_encapsulate( socket, ethernet, sizeof( struct KERNEL_NETWORK_STRUCTURE_HEADER_ARP ) );
		}

		// release AP time
		kernel_time_sleep( 1 );
	}
}