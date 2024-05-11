/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t module_network_arp( struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length ) {
	// properties of ARP header
	struct MODULE_NETWORK_STRUCTURE_HEADER_ARP *arp = (struct MODULE_NETWORK_STRUCTURE_HEADER_ARP *) ((uintptr_t) ethernet + sizeof( struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET ));

	// reply targeting us?
	if( arp -> operation == MODULE_NETWORK_HEADER_ARP_OPERATION_answer ) {
		// update all sockets with provided IPV4 address
		for( uint64_t i = 0; i < MODULE_NETWORK_SOCKET_limit; i++ ) {
			// socket with destination IPv4 address?
			if( module_network_socket_list[ i ].ipv4_target == arp -> source_ipv4 ) {
				// update
				for( uint8_t j = 0; j < 6; j++ ) module_network_socket_list[ i ].ethernet_mac[ j ] = arp -> source_mac[ j ];

				// lease time
				module_network_socket_list[ i ].ethernet_mac_lease = kernel -> time_unit + (300 * DRIVER_RTC_Hz);	// ~5 min
			}
		}

		// answer parsed
		return TRUE;
	}

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

	// close socket
	module_network_socket_close( socket );

	// frame transferred to driver, do not release
	return FALSE;
}

void module_network_arp_thread( void ) {
	// open new socket for this thread
	struct MODULE_NETWORK_STRUCTURE_SOCKET *socket = module_network_socket();

	// set socket properties

	// protocol
	socket -> protocol = STD_NETWORK_PROTOCOL_arp;

	// target MAC address
	for( uint8_t i = 0; i < 6; i++ ) socket -> ethernet_mac[ i ] = 0xFF;	// broadcast

	// socket configured, activate
	socket -> flags = MODULE_NETWORK_SOCKET_FLAG_active;

	// main loop
	while( TRUE ) {
		// analyze sockets properties
		for( uint64_t i = 0; i < MODULE_NETWORK_SOCKET_limit; i++ ) {
			// resolve IPv4 address?
			if( ! module_network_socket_list[ i ].pid || module_network_socket_list[ i ].protocol == STD_NETWORK_PROTOCOL_arp ) continue;	// no need

			// up to date?
			if( module_network_socket_list[ i ].ethernet_mac_lease > kernel -> time_unit ) continue;	// yes

			//----------------------------------------------------------------------

			// allocate area for ethernet/arp frame
			struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet = (struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET *) kernel -> memory_alloc( TRUE );

			// properties of ARP frame
			struct MODULE_NETWORK_STRUCTURE_HEADER_ARP *arp_frame = (struct MODULE_NETWORK_STRUCTURE_HEADER_ARP *) ((uintptr_t) ethernet + sizeof( struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET ) );

			// set ARP properties
			arp_frame -> hardware_type	= MODULE_NETWORK_HEADER_ARP_HARDWARE_TYPE_ethernet;
			arp_frame -> protocol_type	= MODULE_NETWORK_HEADER_ARP_PROTOCOL_TYPE_ipv4;
			arp_frame -> hardware_length	= MODULE_NETWORK_HEADER_ARP_HARDWARE_LENGTH_mac;
			arp_frame -> protocol_length	= MODULE_NETWORK_HEADER_ARP_PROTOCOL_LENGTH_ipv4;
			arp_frame -> operation	= MODULE_NETWORK_HEADER_ARP_OPERATION_request;

			// set source MAC
			for( uint8_t i = 0; i < 6; i++ ) arp_frame -> source_mac[ i ] = kernel -> network_interface.ethernet_mac[ i ];

			// set source IPv4
			arp_frame -> source_ipv4 = MACRO_ENDIANNESS_DWORD( kernel -> network_interface.ipv4_address );

			// set target IPv4
			arp_frame -> target_ipv4 = module_network_socket_list[ i ].ipv4_target;

			// encapsulate ARP frame and send
			module_network_ethernet_encapsulate( socket, ethernet, sizeof( struct MODULE_NETWORK_STRUCTURE_HEADER_ARP ) );
		}

		// release AP time
		kernel -> time_sleep( 1024 );
	}
}