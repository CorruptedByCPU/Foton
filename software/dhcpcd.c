/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, routines, procedures
	//----------------------------------------------------------------------
	#include	"./dhcpcd/config.h"
	#include	"./dhcpcd/data.c"
	#include	"./dhcpcd/option.c"

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// say Hello
	print( "DHCP Client Daemon.\n" );

	// prepare connection with unknown (0xFFFFFFFF > 255.255.255.255) DHCP server
	int64_t socket = std_network_open( STD_NETWORK_PROTOCOL_udp, 0xFFFFFFFF, DHCP_PORT_target, DHCP_PORT_local );

	// exit, if cannot find DHCP server at local network
	uint8_t discover_attempts = 128;

	// main loop
	while( TRUE ) {
		// alloc area for default message size
		dhcp = (struct DHCP_STRUCTURE *) malloc( sizeof( struct DHCP_STRUCTURE ) );
		dhcp_length = sizeof( struct DHCP_STRUCTURE );

		// get current interface properties
		struct STD_NETWORK_STRUCTURE_INTERFACE eth0;
		std_network_interface( (struct STD_NETWORK_STRUCTURE_INTERFACE *) &eth0 );

		// prepare default request
		dhcp -> op_code = DHCP_OP_CODE_boot_request;
		dhcp -> hardware_type = DHCP_HARDWARE_TYPE_ethernet;
		dhcp -> hardware_length = 6;	// MAC address length
		dhcp -> transaction_id = (uint32_t) std_microtime();
		dhcp -> seconds = 0x0100;
		dhcp -> flags = DHCP_FLAGS_unicast;	// initially, later unicast
		dhcp -> client_ip_address = EMPTY;		// unknown
		dhcp -> your_ip_address = EMPTY;		// unknown
		dhcp -> server_ip_address = EMPTY;		// unknown
		dhcp -> gateway_ip_address = EMPTY;	// unknown
		for( uint8_t i = 0; i < 6; i++ ) dhcp -> client_hardware_address[ i ] = eth0.ethernet_mac[ i ];	// our network controller MAC address
		dhcp -> magic_cookie = DHCP_MAGIC_COOKIE;
		dhcp -> option = DHCP_OPTION_TYPE_END;	// first and last option is END

		// add "request" option
		struct DHCP_STRUCTURE_OPTION_request option_request = { EMPTY };
		option_request.type = DHCP_OPTION_TYPE_REQUEST;
		option_request.length = MACRO_SIZEOF( struct DHCP_STRUCTURE_OPTION_request, dhcp );
		option_request.dhcp = DHCP_OPTION_TYPE_REQUEST_DHCPDISCOVER;
		dhcp_option_add( (uint8_t *) &option_request );

		// // add "max dhcp message size"
		// struct DHCP_STRUCTURE_OPTION_max_dhcp_message_size option_max_dhcp_message_size = { EMPTY };
		// option_max_dhcp_message_size.type = DHCP_OPTION_TYPE_MAX_DHCP_MESSAGE_SIZE;
		// option_max_dhcp_message_size.length = MACRO_SIZEOF( struct DHCP_STRUCTURE_OPTION_max_dhcp_message_size, byte );
		// option_max_dhcp_message_size.byte = (uint16_t) MACRO_ENDIANNESS_WORD( 1024 );
		// dhcp_option_add( (uint8_t *) &option_max_dhcp_message_size );

		// // add "client identifier" option
		// struct DHCP_STRUCTURE_OPTION_client_identifier option_client_identifier = { EMPTY };
		// option_client_identifier.type = DHCP_OPTION_TYPE_CLIENT_IDENTIFIER;
		// option_client_identifier.length = MACRO_SIZEOF( struct DHCP_STRUCTURE_OPTION_client_identifier, hardware_type ) + MACRO_SIZEOF( struct DHCP_STRUCTURE_OPTION_client_identifier, client_mac_address );
		// option_client_identifier.hardware_type = DHCP_OPTION_TYPE_CLIENT_IDENTIFIER_HARDWARE_TYPE_ethernet;
		// for( uint8_t i = 0; i < 6; i++ ) option_client_identifier.client_mac_address[ i ] = eth0.ethernet_mac[ i ];	// our network controller MAC address
		// dhcp_option_add( (uint8_t *) &option_client_identifier );

		// // add "parameter request list" option
		// uint8_t option_parameter_request_list_entries = 5;
		// struct DHCP_STRUCTURE_OPTION_parameter_request_list *option_parameter_request_list = (struct DHCP_STRUCTURE_OPTION_parameter_request_list *) malloc( sizeof( struct DHCP_STRUCTURE_OPTION_parameter_request_list ) + option_parameter_request_list_entries );
		// option_parameter_request_list -> type = DHCP_OPTION_TYPE_PARAMETER_REQUEST_LIST;
		// option_parameter_request_list -> length = option_parameter_request_list_entries;
		// uint8_t *option_parameter_request_list_entry = (uint8_t *) option_parameter_request_list + sizeof( struct DHCP_STRUCTURE_OPTION_parameter_request_list );
		// *(option_parameter_request_list_entry++) = DHCP_OPTION_TYPE_PARAMETER_REQUEST_LIST_subnet_mask;
		// *(option_parameter_request_list_entry++) = DHCP_OPTION_TYPE_PARAMETER_REQUEST_LIST_router;
		// *(option_parameter_request_list_entry++) = DHCP_OPTION_TYPE_PARAMETER_REQUEST_LIST_dns;
		// *(option_parameter_request_list_entry++) = DHCP_OPTION_TYPE_PARAMETER_REQUEST_LIST_ntp;
		// *(option_parameter_request_list_entry++) = DHCP_OPTION_TYPE_PARAMETER_REQUEST_LIST_lease_time;
		// dhcp_option_add( (uint8_t *) option_parameter_request_list );
		// // release option "parameter request list" area
		// free( option_parameter_request_list );

		// open file for read
		FILE *file = fopen( (uint8_t *) "/system/etc/hostname" );
		if( file ) {
			// assign area for file content
			uint8_t *hostname;
			if( ! (hostname = (uint8_t *) malloc( file -> byte )) ) return STD_ERROR_memory_low;

			// load file content
			fread( file, hostname, file -> byte );

			// close file
			fclose( file );

			// remove "white" characters from first line
			uint64_t hostname_length = lib_string_word_of_letters_and_digits( hostname, lib_string_trim( hostname, lib_string_length( hostname ) ) );

			// length name valid?
			if( hostname_length < 256 ) {
				// add "hostname" option
				struct DHCP_STRUCTURE_OPTION_hostname option_hostname = { EMPTY };
				option_hostname.type = DHCP_OPTION_TYPE_HOSTNAME;
				option_hostname.length = hostname_length;
				for( uint8_t i = 0; i < hostname_length; i++ ) option_hostname.name[ i ] = hostname[ i ];
				dhcp_option_add( (uint8_t *) &option_hostname );
			}

			// release file content
			free( hostname );
		}

		// request prepared
		printf( "DHCPDiscover (%u).\n", discover_attempts );

		// send request outside
		std_network_send( socket, (uint8_t *) dhcp, dhcp_length );

		// release default message
		free( dhcp );

		// wait before sending next discover message
		std_sleep( 16 );

		// cannot locate DHCP server?
		if( ! --discover_attempts ) break;	// yep
	}

	// error
	print( "No response from network." );

	// program closed properly
	return 0;
}