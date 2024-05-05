/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/network.h"

uint32_t lib_network_string_to_ipv4( uint8_t *string ) {
	// ipv4 address
	uint32_t ipv4_address = EMPTY;	// 0.0.0.0

	// IPv4 address translated?
	uint8_t translated = TRUE;

	// raed IPv4 address from user
	for( int8_t i = 3; i >= 0; i-- ) {
		// first octet length in digits
		uint8_t length = lib_string_length_scope_digit( string );

		// invalid value?
		if( ! length ) { translated = FALSE; break; }	// yes

		// set first octet of interface IPV4 address
		*(uint8_t *) ((uintptr_t) &ipv4_address + i) = lib_string_to_integer( string, 10 );

		// invalid octet value?
		if( *(uint8_t *) ((uintptr_t) &ipv4_address + i) > 255 ) { translated = FALSE; break; }

		// move pointer to next value
		string += length;

		// not last octet?
		if( i ) {	// yes
			// next will be separator?
			if( *string != STD_ASCII_DOT ) { translated = FALSE; break; }

			// omit semaphore
			string++;
		}
	}

	// if invalid IPv4 string provided
	if( ! translated || *string ) return EMPTY;	// string malformed

	// return translated IPV4 address
	return ipv4_address;
}