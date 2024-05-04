/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define DHCP_PORT_target						67
#define DHCP_PORT_local							68

#define DHCP_OP_CODE_boot_request					0x01
#define DHCP_HARDWARE_TYPE_ethernet					0x01
#define DHCP_FLAGS_unicast						0x00000
#define DHCP_FLAGS_broadcast						0x00080
#define	DHCP_MAGIC_COOKIE						0x63538263	// Big-Endian

#define	DHCP_OPTION_TYPE_HOSTNAME					0x0C
#define	DHCP_OPTION_TYPE_REQUESTED_IP_ADDRESS				0x32
#define	DHCP_OPTION_TYPE_REQUEST					0x35
#define	DHCP_OPTION_TYPE_REQUEST_DHCPDISCOVER				0x01
#define	DHCP_OPTION_TYPE_REQUEST_DHCPOFFER				0x02
#define	DHCP_OPTION_TYPE_REQUEST_DHCPREQUEST				0x03
#define	DHCP_OPTION_TYPE_REQUEST_DHCPDECLINE				0x04
#define	DHCP_OPTION_TYPE_REQUEST_DHCPACK				0x05
#define	DHCP_OPTION_TYPE_REQUEST_DHCPNAK				0x06
#define	DHCP_OPTION_TYPE_REQUEST_DHCPRELEASE				0x07
#define	DHCP_OPTION_TYPE_REQUEST_DHCPINFORM				0x08
#define	DHCP_OPTION_TYPE_PARAMETER_REQUEST_LIST				0x37
#define	DHCP_OPTION_TYPE_PARAMETER_REQUEST_LIST_subnet_mask		0x01
#define	DHCP_OPTION_TYPE_PARAMETER_REQUEST_LIST_router			0x03
#define	DHCP_OPTION_TYPE_PARAMETER_REQUEST_LIST_dns			0x06
#define	DHCP_OPTION_TYPE_PARAMETER_REQUEST_LIST_hostname		0x0C
#define	DHCP_OPTION_TYPE_PARAMETER_REQUEST_LIST_domain_name		0x0F
#define	DHCP_OPTION_TYPE_PARAMETER_REQUEST_LIST_broadcast_address	0x1C
#define	DHCP_OPTION_TYPE_PARAMETER_REQUEST_LIST_static_route		0x23
#define	DHCP_OPTION_TYPE_PARAMETER_REQUEST_LIST_ntp			0x2A
#define	DHCP_OPTION_TYPE_PARAMETER_REQUEST_LIST_lease_time		0x33
#define	DHCP_OPTION_TYPE_MAX_DHCP_MESSAGE_SIZE				0x39
#define	DHCP_OPTION_TYPE_CLIENT_IDENTIFIER				0x3D
#define	DHCP_OPTION_TYPE_CLIENT_IDENTIFIER_HARDWARE_TYPE_ethernet	0x01
#define	DHCP_OPTION_TYPE_AUTO_CONFIGURATION				0x74
#define	DHCP_OPTION_TYPE_END						0xFF

struct DHCP_STRUCTURE {
	uint8_t		op_code;
	uint8_t		hardware_type;
	uint8_t		hardware_length;
	uint8_t		hops;
	uint32_t	transaction_id;
	uint16_t	seconds;
	uint16_t	flags;
	uint32_t	client_ip_address;
	uint32_t	your_ip_address;
	uint32_t	server_ip_address;
	uint32_t	gateway_ip_address;
	uint8_t		client_hardware_address[ 16 ];
	uint8_t		server_name[ 64 ];
	uint8_t		boot_file_name[ 128 ];
	uint32_t	magic_cookie;
	uint8_t		option;
} __attribute__( (packed) );

struct DHCP_STRUCTURE_OPTION_DEFAULT {
	uint8_t	type;
	uint8_t	length;
} __attribute__( (packed) );

struct DHCP_STRUCTURE_OPTION_auto_configuration {
	uint8_t	type;
	uint8_t	length;
	uint8_t	test;
} __attribute__( (packed) );

struct DHCP_STRUCTURE_OPTION_client_identifier {
	uint8_t type;
	uint8_t	length;
	uint8_t	hardware_type;
	uint8_t	client_mac_address[ 6 ];
} __attribute__( (packed) );

struct DHCP_STRUCTURE_OPTION_end {
	uint8_t type;
} __attribute__( (packed) );

struct DHCP_STRUCTURE_OPTION_hostname {
	uint8_t type;
	uint8_t	length;
	uint8_t	name[ 255 ];
} __attribute__( (packed) );

struct DHCP_STRUCTURE_OPTION_max_dhcp_message_size {
	uint8_t 	type;
	uint8_t		length;
	uint16_t	byte;
} __attribute__( (packed) );

struct DHCP_STRUCTURE_OPTION_parameter_request_list {
	uint8_t	type;
	uint8_t	length;
} __attribute__( (packed) );

struct DHCP_STRUCTURE_OPTION_request {
	uint8_t type;
	uint8_t	length;
	uint8_t	dhcp;
} __attribute__( (packed) );

struct DHCP_STRUCTURE_OPTION_requested_ip_address {
	uint8_t 	type;
	uint8_t		length;
	uint32_t	address;
} __attribute__( (packed) );

struct DHCP_STRUCTURE *dhcp = EMPTY;
uint16_t dhcp_length = EMPTY;

void dhcp_option_add( uint8_t *option ) {
	// calculate new DHCP message length
	dhcp_length += option[ 1 ] + sizeof( struct DHCP_STRUCTURE_OPTION_DEFAULT );

	// extend area of dhcp message
	dhcp = (struct DHCP_STRUCTURE *) realloc( dhcp, dhcp_length );

	// set pointer at begining of options list
	uint8_t *options = (uint8_t *) &dhcp -> option;

	// move pointer at end of all options
	while( *options != DHCP_OPTION_TYPE_END ) options += options[ 1 ] + sizeof( struct DHCP_STRUCTURE_OPTION_DEFAULT );

	// insert option at end of options list
	for( uint16_t i = 0; i < option[ 1 ] + sizeof( struct DHCP_STRUCTURE_OPTION_DEFAULT ); i++ ) options[ i ] = option[ i ];

	// close options list
	options[ option[ 1 ] + sizeof( struct DHCP_STRUCTURE_OPTION_DEFAULT ) ] = DHCP_OPTION_TYPE_END;
}

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// do not use yet...
	while( TRUE );

	// say Hello
	print( "DHCP Client Daemon.\n" );

	// open connection with unknown (0xFFFFFFFF > 255.255.255.255) DHCP server
	int64_t socket = std_network_open( STD_NETWORK_PROTOCOL_udp, 0xFFFFFFFF, DHCP_PORT_target, DHCP_PORT_local );

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
		dhcp -> flags = DHCP_FLAGS_broadcast;	// initially, later unicast
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

		// add "max dhcp message size"
		struct DHCP_STRUCTURE_OPTION_max_dhcp_message_size option_max_dhcp_message_size = { EMPTY };
		option_max_dhcp_message_size.type = DHCP_OPTION_TYPE_MAX_DHCP_MESSAGE_SIZE;
		option_max_dhcp_message_size.length = MACRO_SIZEOF( struct DHCP_STRUCTURE_OPTION_max_dhcp_message_size, byte );
		option_max_dhcp_message_size.byte = (uint16_t) MACRO_ENDIANNESS_WORD( 1024 );
		dhcp_option_add( (uint8_t *) &option_max_dhcp_message_size );

		// add "client identifier" option
		struct DHCP_STRUCTURE_OPTION_client_identifier option_client_identifier = { EMPTY };
		option_client_identifier.type = DHCP_OPTION_TYPE_CLIENT_IDENTIFIER;
		option_client_identifier.length = MACRO_SIZEOF( struct DHCP_STRUCTURE_OPTION_client_identifier, hardware_type ) + MACRO_SIZEOF( struct DHCP_STRUCTURE_OPTION_client_identifier, client_mac_address );
		option_client_identifier.hardware_type = DHCP_OPTION_TYPE_CLIENT_IDENTIFIER_HARDWARE_TYPE_ethernet;
		for( uint8_t i = 0; i < 6; i++ ) option_client_identifier.client_mac_address[ i ] = eth0.ethernet_mac[ i ];	// our network controller MAC address
		dhcp_option_add( (uint8_t *) &option_client_identifier );

		// add "parameter request list" option
		uint8_t option_parameter_request_list_entries = 5;
		struct DHCP_STRUCTURE_OPTION_parameter_request_list *option_parameter_request_list = (struct DHCP_STRUCTURE_OPTION_parameter_request_list *) malloc( sizeof( struct DHCP_STRUCTURE_OPTION_parameter_request_list ) + option_parameter_request_list_entries );
		option_parameter_request_list -> type = DHCP_OPTION_TYPE_PARAMETER_REQUEST_LIST;
		option_parameter_request_list -> length = option_parameter_request_list_entries;
		uint8_t *option_parameter_request_list_entry = (uint8_t *) option_parameter_request_list + sizeof( struct DHCP_STRUCTURE_OPTION_parameter_request_list );
		*(option_parameter_request_list_entry++) = DHCP_OPTION_TYPE_PARAMETER_REQUEST_LIST_subnet_mask;
		*(option_parameter_request_list_entry++) = DHCP_OPTION_TYPE_PARAMETER_REQUEST_LIST_router;
		*(option_parameter_request_list_entry++) = DHCP_OPTION_TYPE_PARAMETER_REQUEST_LIST_dns;
		*(option_parameter_request_list_entry++) = DHCP_OPTION_TYPE_PARAMETER_REQUEST_LIST_ntp;
		*(option_parameter_request_list_entry++) = DHCP_OPTION_TYPE_PARAMETER_REQUEST_LIST_lease_time;
		dhcp_option_add( (uint8_t *) option_parameter_request_list );
		// release option "parameter request list" area
		free( option_parameter_request_list );

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

		print( "DHCPDiscover.\n" );

		// send packet outside
		std_network_send( socket, (uint8_t *) dhcp, dhcp_length );

		// release default message
		free( dhcp );

		// wait before sending next discover message
		std_sleep( 512 );
	}

	return 0;
}