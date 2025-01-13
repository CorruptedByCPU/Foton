/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, routines, procedures
	//----------------------------------------------------------------------
	#include	"./dhcpcd/config.h"
	#include	"./dhcpcd/data.c"
	#include	"./dhcpcd/option.c"

void dhcp_default( struct DHCP_STRUCTURE *dhcp ) {
	// set default properties
	dhcp -> op_code			= DHCP_OP_CODE_boot_request;
	dhcp -> hardware_type		= DHCP_HARDWARE_TYPE_ethernet;
	dhcp -> hardware_length		= 6;	// MAC address length
	dhcp -> transaction_id		= (uint32_t) std_microtime();
	dhcp -> seconds			= EMPTY;	// initial value
	dhcp -> flags			= DHCP_FLAGS_unicast;
	dhcp -> client_ipv4_address	= EMPTY;	// you
	dhcp -> your_ipv4_address	= EMPTY;	// know
	dhcp -> server_ipv4_address	= EMPTY;	// nothing,
	dhcp -> gateway_ipv4_address	= EMPTY;	// John Snow
	dhcp -> magic_cookie		= DHCP_MAGIC_COOKIE;
	dhcp -> options			= DHCP_OPTION_TYPE_END;	// first and last option is END

	// our network controller MAC address
	for( uint8_t i = 0; i < 6; i++ ) dhcp -> client_hardware_address[ i ] = eth0.ethernet_address[ i ];
}

uint16_t dhcp_hostname( struct DHCP_STRUCTURE *dhcp, uint16_t length ) {
	// open file for read
	FILE *file = fopen( (uint8_t *) "/system/etc/hostname.txt", EMPTY );

	// if file exist, why not?
	if( file ) {
		// assign area for file content
		uint8_t *string_hostname = (uint8_t *) malloc( file -> byte );

		// load file content
		fread( file, string_hostname, file -> byte );

		// close file, no need anymore
		fclose( file );

		// keep only first word from first line
		uint64_t string_hostname_length = lib_string_word_of_letters_and_digits( string_hostname, lib_string_trim( string_hostname, lib_string_length( string_hostname ) ) );

		// length name valid?
		if( string_hostname_length < MACRO_SIZEOF( struct DHCP_STRUCTURE_OPTION_HOSTNAME, name ) ) {
			// prepare option "Hostname"
			struct DHCP_STRUCTURE_OPTION_HOSTNAME hostname = { EMPTY };
			hostname.option.type = DHCP_OPTION_TYPE_HOSTNAME;
			hostname.option.length = string_hostname_length;
			for( uint8_t i = 0; i < string_hostname_length; i++ ) hostname.name[ i ] = string_hostname[ i ];

			// add this option to DHCPDiscover message
			length = dhcp_option_add( dhcp, length, (uint8_t *) &hostname );
		}

		// release file content
		free( string_hostname );
	}

	// return new message length
	return length;
}

void dhcp_receive( struct STD_STRUCTURE_NETWORK_DATA *packet ) {
	// start of timelapse
	int64_t current_microtime = std_microtime();
	int64_t end_microtime = current_microtime + 512;

	// wait for incomming reply
	while( end_microtime > current_microtime && ! packet -> length ) {
		// check for incommint reply
		std_network_receive( socket, packet );

		// still no reply, update current time
		current_microtime = std_microtime();
	}
}

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// get current interface properties
	std_network_interface( (struct STD_STRUCTURE_NETWORK_INTERFACE *) &eth0 );

	// prepare connection with unknown (255.255.255.255) DHCP server
	socket = std_network_open( STD_NETWORK_PROTOCOL_udp, 0xFFFFFFFF, DHCP_PORT_target, DHCP_PORT_local );

	// exit, if cannot find DHCP server at local network
	uint8_t dhcp_attempts = 3;

	// main loop
	while( dhcp_attempts ) {
		// select action
		switch( dhcp_action ) {
			case Discover: {
				// if not set yet
				if( ! dhcp_discover ) {
					// alloc DHCP message area
					dhcp_discover = (struct DHCP_STRUCTURE *) malloc( dhcp_discover_length );

					// set default properties of DHCP
					dhcp_default( dhcp_discover );

					// prepare option "Request"
					struct DHCP_STRUCTURE_OPTION_REQUEST request = { EMPTY };
					request.option.type	= DHCP_OPTION_TYPE_REQUEST;
					request.option.length	= MACRO_SIZEOF( struct DHCP_STRUCTURE_OPTION_REQUEST, dhcp );
					request.dhcp	= DHCP_OPTION_TYPE_REQUEST_DHCPDISCOVER;

					// add this option to DHCP message
					dhcp_discover_length = dhcp_option_add( dhcp_discover, dhcp_discover_length, (uint8_t *) &request );
				}

				// send DHCP to local network
				std_network_send( socket, (uint8_t *) dhcp_discover, dhcp_discover_length );

				// first attempt, send
				dhcp_attempts--;

				// next action, Offer
				dhcp_action = Offer;
			}

			case Offer: {
				// properties of reply
				struct STD_STRUCTURE_NETWORK_DATA packet_offer = { EMPTY };

				// wait for incomming message
				dhcp_receive( (struct STD_STRUCTURE_NETWORK_DATA *) &packet_offer );

				// received answer?
				if( ! packet_offer.length ) {	// no
					// lets back to DHCPDiscover
					dhcp_action = Discover; continue;
				}

				// properties of DHCP message
				struct DHCP_STRUCTURE *dhcp_offer = (struct DHCP_STRUCTURE *) packet_offer.data;

				// let us know IPv4 of DHCP server which responded to out Discover
				struct DHCP_STRUCTURE_OPTION_DHCP_SERVER_IDENTIFIER *dhcp_server_identifier = (struct DHCP_STRUCTURE_OPTION_DHCP_SERVER_IDENTIFIER *) dhcp_option( (uint8_t *) &dhcp_offer -> options, DHCP_OPTION_TYPE_DHCP_SERVER_IDENTIFIER );
				if( dhcp_server_identifier ) dhcp_ipv4_server = dhcp_server_identifier -> ipv4_address;
				else {	// there is something wrong with that response
					// lets back to DHCPDiscover
					dhcp_action = Discover; continue;
				}

				// let us know too about provided IPV4 address for us
				dhcp_ipv4_client = dhcp_offer -> your_ipv4_address;

				// next action, DHCPRequest
				dhcp_action = Request;
			}

			case Request: {
				// if not set yet
				if( ! dhcp_request ) {
					// alloc DHCP message area
					dhcp_request = (struct DHCP_STRUCTURE *) malloc( dhcp_request_length );

					// set default properties of DHCP
					dhcp_default( dhcp_request );

					//--------------------------------------

					// prepare option "Request"
					struct DHCP_STRUCTURE_OPTION_REQUEST request = { EMPTY };
					request.option.type	= DHCP_OPTION_TYPE_REQUEST;
					request.option.length	= MACRO_SIZEOF( struct DHCP_STRUCTURE_OPTION_REQUEST, dhcp );
					request.dhcp		= DHCP_OPTION_TYPE_REQUEST_DHCPREQUEST;

					// add this option to DHCP message
					dhcp_request_length = dhcp_option_add( dhcp_request, dhcp_request_length, (uint8_t *) &request );

					//--------------------------------------

					// prepare option "Parameter Request List"
					struct DHCP_STRUCTURE_OPTION_REQUEST_LIST *request_list = (struct DHCP_STRUCTURE_OPTION_REQUEST_LIST *) malloc( sizeof( struct DHCP_STRUCTURE_OPTION_REQUEST_LIST ) + 6 );
					request_list -> option.type = DHCP_OPTION_TYPE_REQUEST_LIST;
					request_list -> option.length = 6;

					// option ist entries
					uint8_t *request_list_entry = (uint8_t *) request_list + sizeof( struct DHCP_STRUCTURE_OPTION_REQUEST_LIST );
					*(request_list_entry++) = DHCP_OPTION_TYPE_SUBNET_MASK;
					*(request_list_entry++) = DHCP_OPTION_TYPE_ROUTER;
					*(request_list_entry++) = DHCP_OPTION_TYPE_DNS;
					*(request_list_entry++) = DHCP_OPTION_TYPE_BROADCAST_ADDRESS;
					*(request_list_entry++) = DHCP_OPTION_TYPE_NTP;
					*(request_list_entry++) = DHCP_OPTION_TYPE_LEASE_TIME;

					// add this option to DHCP message
					dhcp_request_length = dhcp_option_add( dhcp_request, dhcp_request_length, (uint8_t *) request_list );

					// release option "parameter request list" area
					free( request_list );
				}

				//----------------------------------------------

				// remove (propably) old option "Hostname"
				dhcp_discover_length = dhcp_option_remove( dhcp_discover, dhcp_discover_length, DHCP_OPTION_TYPE_HOSTNAME );

				// insert new option "Hostname"
				dhcp_discover_length = dhcp_hostname( dhcp_discover, dhcp_discover_length );

				//----------------------------------------------

				// remove (propably) old option "Requested IPv4 Address"
				dhcp_request_length = dhcp_option_remove( dhcp_request, dhcp_request_length, DHCP_OPTION_TYPE_REQUESTED_IP_ADDRESS );

				// prepare option "Requested IPv4 Address"
				struct DHCP_STRUCTURE_OPTION_REQUESTED_IP_ADDRESS requested_ip_address = { EMPTY };
				requested_ip_address.option.type	= DHCP_OPTION_TYPE_REQUESTED_IP_ADDRESS;
				requested_ip_address.option.length	= 4;
				requested_ip_address.ipv4_address	= dhcp_ipv4_client;	// provided IPv4 address by DHCP server.

				// add this option to DHCP message
				dhcp_request_length = dhcp_option_add( dhcp_request, dhcp_request_length, (uint8_t *) &requested_ip_address );

				//----------------------------------------------

				// remove (propably) old option "DHCP Server Identifier"
				dhcp_request_length = dhcp_option_remove( dhcp_request, dhcp_request_length, DHCP_OPTION_TYPE_DHCP_SERVER_IDENTIFIER );

				// prepare option "DHCP Server Identifier"
				struct DHCP_STRUCTURE_OPTION_DHCP_SERVER_IDENTIFIER dhcp_server_identifier = { EMPTY };
				dhcp_server_identifier.option.type	= DHCP_OPTION_TYPE_DHCP_SERVER_IDENTIFIER;
				dhcp_server_identifier.option.length	= 4;
				dhcp_server_identifier.ipv4_address	= dhcp_ipv4_server;

				// add this option to DHCP message
				dhcp_request_length = dhcp_option_add( dhcp_request, dhcp_request_length, (uint8_t *) &dhcp_server_identifier );

				// send DHCP to local network
				std_network_send( socket, (uint8_t *) dhcp_request, dhcp_request_length );

				// first attempt, send
				dhcp_attempts--;

				// next action, Offer
				dhcp_action = Answer;
			}

			case Answer: {
				// properties of reply
				struct STD_STRUCTURE_NETWORK_DATA packet_answer = { EMPTY };

				// wait for incomming message
				dhcp_receive( (struct STD_STRUCTURE_NETWORK_DATA *) &packet_answer );

				// received answer?
				if( ! packet_answer.length ) {	// no
					// send another Request
					dhcp_action = Request; continue;
				}

				// properties of DHCP message
				struct DHCP_STRUCTURE *dhcp_answer = (struct DHCP_STRUCTURE *) packet_answer.data;

				// search for option "Request"
				struct DHCP_STRUCTURE_OPTION_REQUEST *request = (struct DHCP_STRUCTURE_OPTION_REQUEST *) dhcp_option( (uint8_t *) &dhcp_answer -> options, DHCP_OPTION_TYPE_REQUEST );
				// requested IPv4 address accepted?
				if( request -> dhcp != DHCP_OPTION_TYPE_REQUEST_DHCPACK ) {	// no
					// send another Discover
					dhcp_action = Discover; continue;
				}

				// search for option "Subnet Mask"
				struct DHCP_STRUCTURE_OPTION_SUBNET_MASK *subnet_mask = (struct DHCP_STRUCTURE_OPTION_SUBNET_MASK *) dhcp_option( (uint8_t *) &dhcp_answer -> options, DHCP_OPTION_TYPE_SUBNET_MASK );
				if( subnet_mask ) eth0.ipv4_mask = MACRO_ENDIANNESS_DWORD( subnet_mask -> ipv4_address );

				// search for option "Broadcast"
				struct DHCP_STRUCTURE_OPTION_BROADCAST_ADDRESS *broadcast = (struct DHCP_STRUCTURE_OPTION_BROADCAST_ADDRESS *) dhcp_option( (uint8_t *) &dhcp_answer -> options, DHCP_OPTION_TYPE_BROADCAST_ADDRESS );
				if( broadcast ) eth0.ipv4_broadcast = MACRO_ENDIANNESS_DWORD( broadcast -> ipv4_address );

				// search for option "Gateway"
				struct DHCP_STRUCTURE_OPTION_ROUTER *router = (struct DHCP_STRUCTURE_OPTION_ROUTER *) dhcp_option( (uint8_t *) &dhcp_answer -> options, DHCP_OPTION_TYPE_ROUTER );
				if( router ) eth0.ipv4_gateway = MACRO_ENDIANNESS_DWORD( router -> ipv4_address );

				// assign provided IPv4 address to network interface
				eth0.ipv4_address = MACRO_ENDIANNESS_DWORD( dhcp_ipv4_client );
				std_network_interface_set( (struct STD_STRUCTURE_NETWORK_INTERFACE *) &eth0 );

				// DHCP finished
				dhcp_attempts = EMPTY;
			}
		}
	}

	// program closed properly
	return 0;
}