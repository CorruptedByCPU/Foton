/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/string.h"

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// get current interface properties
	struct STD_NETWORK_STRUCTURE_INTERFACE eth0;
	std_network_interface( (struct STD_NETWORK_STRUCTURE_INTERFACE *) &eth0 );

	// change IPV4 address?
	if( argc > 1 ) {	// yes
		// new IPv4 address acquired?
		uint8_t acquired = TRUE;

		// ipv4 string to parse and its length
		uint8_t *ipv4_string = (uint8_t *) argv[ 1 ];

		// raed IPv4 address from user
		for( int8_t i = 3; i >= 0; i-- ) {
			// first octet length in digits
			uint8_t length = lib_string_length_scope_digit( ipv4_string );

			// invalid value?
			if( ! length ) { acquired = FALSE; break; }	// yes

			// set first octet of interface IPV4 address
			*(uint8_t *) ((uintptr_t) &eth0.ipv4_address + i) = lib_string_to_integer( ipv4_string, 10 );

			// invalid octet value?
			if( *(uint8_t *) ((uintptr_t) &eth0.ipv4_address + i) > 255 ) { acquired = FALSE; break; }

			// move pointer to next value
			ipv4_string += length;

			// not last octet?
			if( i ) {	// yes
				// next will be separator?
				if( *ipv4_string != STD_ASCII_DOT ) { acquired = FALSE; break; }

				// omit semaphore
				ipv4_string++;
			}
		}

		// if invalid IPv4 string provided
		if( ! acquired || *ipv4_string ) {
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