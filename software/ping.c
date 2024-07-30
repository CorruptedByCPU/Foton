/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, routines, procedures
	//----------------------------------------------------------------------
	#include	"../library/network.h"

#define	PING_ICMP_TYPE_request	0x08
#define	PING_ICMP_TYPE_reply	0x00

#define	PING_ICMP_DATA_length	32

struct PING_STRUCTURE_ICMP {
	uint8_t		type;
	uint8_t		code;
	uint16_t	checksum;
	uint16_t	identificator;
	uint16_t	sequence;
} __attribute__((packed));

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// request for at least 4 replies
	uint64_t count = 4;

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
	int64_t socket = std_network_open( STD_NETWORK_PROTOCOL_icmp, ipv4, EMPTY, EMPTY );

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

	// create ICMP request
	struct PING_STRUCTURE_ICMP *icmp = (struct PING_STRUCTURE_ICMP *) malloc( sizeof( struct PING_STRUCTURE_ICMP ) + PING_ICMP_DATA_length );	// default 32 Bytes of data inside ICMP frame

	// ICMP type: request
	icmp -> type = PING_ICMP_TYPE_request;
	
	// ICMP code: EMPTY
	icmp -> code = EMPTY;

	// identificator
	icmp -> identificator = MACRO_ENDIANNESS_WORD( TRUE );

	// sequence number
	icmp -> sequence = MACRO_ENDIANNESS_WORD( TRUE );

	// insert data
	uint8_t *data = (uint8_t *) ((uintptr_t) icmp + sizeof( struct PING_STRUCTURE_ICMP ));
	for( uint16_t i = 0; i < PING_ICMP_DATA_length; i++ ) data[ i ] = i + 0x20;

	// calculate checksum
	icmp -> checksum = EMPTY;	// always
	icmp -> checksum = lib_network_checksum( (uint16_t *) icmp, sizeof( struct PING_STRUCTURE_ICMP ) + PING_ICMP_DATA_length );

	// request for N replies
	while( count ) {
		// start of timelapse
		int64_t current_microtime = std_microtime();
		int64_t end_microtime = current_microtime + 1000;	// at least 1 second

		// send request outside
		std_network_send( socket, (uint8_t *) icmp, sizeof( struct PING_STRUCTURE_ICMP ) + PING_ICMP_DATA_length );

		// properties of reply
		struct STD_NETWORK_STRUCTURE_DATA packet = { EMPTY };

		// wait for incomming reply
		while( end_microtime > current_microtime && ! packet.length ) {
			// check for incommint reply
			std_network_receive( socket, (struct STD_NETWORK_STRUCTURE_DATA *) &packet );

			// still no reply, update current time
			current_microtime = std_microtime();
		}

		// didn't receive anything?
		if( ! packet.length ) { print( "No answer.\n" ); count--; continue; }	// ignore request

		// calculate waiting time
		printf( "Reply from %u.%u.%u.%u in %ums\n", (uint8_t) (ipv4 >> 24), (uint8_t) (ipv4 >> 16), (uint8_t) (ipv4 >> 8), (uint8_t) ipv4, 1000 - (end_microtime - current_microtime) );

		// release packet area
		std_memory_release( (uintptr_t) packet.data, MACRO_PAGE_ALIGN_UP( packet.length ) >> STD_SHIFT_PAGE );

		// no more replies required?
		if( --count ) sleep( end_microtime - current_microtime );	// wait before sending next request
	}

	// release ICMP request
	free( icmp );

	// done
	return 0;
}