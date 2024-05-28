/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, routines, procedures
	//----------------------------------------------------------------------
	#include	"../library/network.h"

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// nothing to do?
	if( argc < 2 ) return 0;	// yes

	// retrieve IPv4 address
	uint32_t ipv4 = lib_network_string_to_ipv4( (uint8_t *) argv[ 1 ] );

	// IPv4 invalid?
	if( ! ipv4 ) {
		// show error message
		printf( "Invalid IPv4 address: \e[38;5;250m%s", argv[ 1 ] );

		// end
		return 0;
	}

	// prepare connection with selected IPv4 address
	int64_t socket = std_network_open( STD_NETWORK_PROTOCOL_tcp, ipv4, 80, EMPTY );

	// status of socket
	switch( socket ) {
		case EMPTY: {
			// error messaage
			print( "Cannot open socket." );

			// exit
			return EMPTY;
		}

		case STD_ERROR_unavailable: {
			// error messaage
			print( "No connection." );

			// exit
			return STD_ERROR_unavailable;
		}
	}

	// create HTTP request
	uint8_t dl_string_get[] = "GET / HTTP/1.0\r\n\r\n";
	uint8_t *http = (uint8_t *) malloc( sizeof( dl_string_get ) - 1 );

	// insert data
	for( uint8_t i = 0; i < sizeof( dl_string_get ); i++ ) http[ i ] = dl_string_get[ i ];

	// send request outside
	std_network_send( socket, (uint8_t *) http, sizeof( dl_string_get ) - 1 );

	// properties of reply
	struct STD_NETWORK_STRUCTURE_DATA packet = { EMPTY };

	// start of timelapse
	int64_t current_microtime = std_microtime();
	int64_t end_microtime = current_microtime + 1000;	// at least 1 second

	// wait for incomming reply
	while( end_microtime > current_microtime && ! packet.length ) {
		// check for incommint reply
		std_network_receive( socket, (struct STD_NETWORK_STRUCTURE_DATA *) &packet );

		// still no reply, update current time
		current_microtime = std_microtime();
	}

	// received answer?
	if( packet.length ) {
		// print content
		printf( "%s\n", (uintptr_t) packet.data );

		// release packet area
		std_memory_release( (uintptr_t) packet.data, MACRO_PAGE_ALIGN_UP( packet.length ) >> STD_SHIFT_PAGE );
	} else
		// send message
		print( "No answer.\n" );

	// release HTTP request
	free( http );

	// done
	return 0;
}