/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define DHCP_PORT_target        67
#define DHCP_PORT_local	        68

#define DHCP_OP_CODE_boot_request	0x01
#define DHCP_HARDWARE_TYPE_ethernet	0x01
#define DHCP_FLAGS_unicast		0x00000
#define DHCP_FLAGS_broadcast		0x00080
#define	DHCP_MAGIC_COOKIE		0x63538263	// Big-Endian

#define	DHCP_OPTION_HOSTNAME				0x0C
#define	DHCP_OPTION_REQUESTED_IP_ADDRESS		0x32
#define	DHCP_OPTION_REQUEST				0x35
#define	DHCP_OPTION_REQUEST_DHCPDISCOVER		0x01
#define	DHCP_OPTION_REQUEST_DHCPOFFER			0x02
#define	DHCP_OPTION_REQUEST_DHCPREQUEST			0x03
#define	DHCP_OPTION_REQUEST_DHCPDECLINE			0x04
#define	DHCP_OPTION_REQUEST_DHCPACK			0x05
#define	DHCP_OPTION_REQUEST_DHCPNAK			0x06
#define	DHCP_OPTION_REQUEST_DHCPRELEASE			0x07
#define	DHCP_OPTION_REQUEST_DHCPINFORM			0x08
#define	DHCP_OPTION_PARAMETER_REQUEST_LIST		0x37
#define	DHCP_OPTION_PARAMETER_REQUEST_LIST_subnet_mask	0x01
#define	DHCP_OPTION_MAX_DHCP_MESSAGE_SIZE		0x39
#define	DHCP_OPTION_CLIENT_IDENTIFIER			0x3D
#define	DHCP_OPTION_AUTO_CONFIGURATION			0x74
#define	DHCP_OPTION_END					0xFF

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

struct DHCP_STRUCTURE_OPTION_client_identifier {
	uint8_t type;
	uint8_t	length;
	uint8_t	hardware_type;
	uint8_t	client_mac_address[ 6 ];
} __attribute__( (packed) );

struct DHCP_STRUCTURE_OPTION_request {
	uint8_t type;
	uint8_t	length;
	uint8_t	dhcp;
} __attribute__( (packed) );

struct DHCP_STRUCTURE_OPTION_parameter_request_list {
	uint8_t	type;
	uint8_t	length;
	uint8_t	list[ 17 ];
} __attribute__( (packed) );

struct DHCP_STRUCTURE_OPTION_requested_ip_address {
	uint8_t 	type;
	uint8_t		length;
	uint32_t	address;
} __attribute__( (packed) );

struct DHCP_STRUCTURE_OPTION_hostname {
	uint8_t type;
	uint8_t	length;
	uint8_t	name[ 8 ];
} __attribute__( (packed) );

struct DHCP_STRUCTURE_OPTION_auto_configuration {
	uint8_t	type;
	uint8_t	length;
	uint8_t	test;
} __attribute__( (packed) );

struct DHCP_STRUCTURE_OPTION_max_dhcp_message_size {
	uint8_t 	type;
	uint8_t		length;
	uint16_t	byte;
} __attribute__( (packed) );

struct DHCP_STRUCTURE_OPTION_end {
	uint8_t type;
} __attribute__( (packed) );

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// do not use yet...
	return 0;

	// say Hello
	print( "DHCP Client Daemon.\n" );

	// open connection with unknown (0xFFFFFFFF > 255.255.255.255) DHCP server
	int64_t socket = std_network_open( STD_NETWORK_PROTOCOL_udp, 0xFFFFFFFF, DHCP_PORT_target, DHCP_PORT_local );

	// alloc area for default message size
	struct DHCP_STRUCTURE *dhcp = (struct DHCP_STRUCTURE *) malloc( sizeof( struct DHCP_STRUCTURE ) );

	while( TRUE ) {
		// get current interface properties
		struct STD_NETWORK_STRUCTURE_INTERFACE eth0;
		std_network_interface( (struct STD_NETWORK_STRUCTURE_INTERFACE *) &eth0 );

		// prepare default request
		dhcp -> op_code = DHCP_OP_CODE_boot_request;
		dhcp -> hardware_type = DHCP_HARDWARE_TYPE_ethernet;
		dhcp -> hardware_length = 6;	// MAC address length
		dhcp -> transaction_id = (uint32_t) std_microtime();
		dhcp -> seconds = 0x0100;
		dhcp -> flags = DHCP_FLAGS_broadcast;
		dhcp -> client_ip_address = EMPTY;		// unknown
		dhcp -> your_ip_address = EMPTY;		// unknown
		dhcp -> server_ip_address = EMPTY;		// unknown
		dhcp -> gateway_ip_address = EMPTY;	// unknown
		for( uint8_t i = 0; i < 6; i++ ) dhcp -> client_hardware_address[ i ] = eth0.ethernet_mac[ i ];	// our network controller MAC address
		dhcp -> magic_cookie = DHCP_MAGIC_COOKIE;
		dhcp -> option = DHCP_OPTION_END;	// first and last option is END
	}

	return 0;
}