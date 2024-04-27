/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define DHCP_PORT_target        67
#define DHCP_PORT_local	        68

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// do not use yet...
	return 0;

	// say Hello
	print( "DHCP Client Daemon.\n" );

	// open connection with unknown (0xFFFFFFFF > 255.255.255.255) DHCP server
	int64_t socket = std_network_open( STD_NETWORK_PROTOCOL_udp, 0xFFFFFFFF, DHCP_PORT_target, DHCP_PORT_local );

	while( TRUE ) {
	}

	return 0;
}