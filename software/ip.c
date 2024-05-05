/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/network.h"

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// get current interface properties
	struct STD_NETWORK_STRUCTURE_INTERFACE eth0;
	std_network_interface( (struct STD_NETWORK_STRUCTURE_INTERFACE *) &eth0 );

	// change IPV4 address?
	if( argc > 1 ) {	// yes
		// retrieve IPv4 address
		eth0.ipv4_address = lib_network_string_to_ipv4( (uint8_t *) argv[ 1 ] );

		// IPv4 invalid?
		if( ! eth0.ipv4_address ) {
			// show error message
			printf( "Invalid IPv4 address: \e[38;5;250m%s", argv[ 1 ] );

			// end
			return 0;
		}

		// update interface configuration
		std_network_interface_set( (struct STD_NETWORK_STRUCTURE_INTERFACE *) &eth0 );
	// no, show current
	} else printf( "%u.%u.%u.%u", (uint8_t) (eth0.ipv4_address >> 0x18), (uint8_t) (eth0.ipv4_address >> 0x10), (uint8_t) (eth0.ipv4_address >> 0x08), (uint8_t) eth0.ipv4_address );

	// program closed properly
	return 0;
}