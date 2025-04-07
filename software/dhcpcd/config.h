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

#define	DHCP_OPTION_TYPE_SUBNET_MASK					0x01
#define	DHCP_OPTION_TYPE_ROUTER						0x03
#define	DHCP_OPTION_TYPE_DNS						0x06
#define	DHCP_OPTION_TYPE_HOSTNAME					0x0C
#define	DHCP_OPTION_TYPE_DOMAIN_NAME					0x0F
#define	DHCP_OPTION_TYPE_BROADCAST_ADDRESS				0x1C
#define	DHCP_OPTION_TYPE_STATIC_ROUTE					0x23
#define	DHCP_OPTION_TYPE_NTP						0x2A
#define	DHCP_OPTION_TYPE_REQUESTED_IP_ADDRESS				0x32
#define	DHCP_OPTION_TYPE_LEASE_TIME					0x33
#define	DHCP_OPTION_TYPE_REQUEST					0x35
#define	DHCP_OPTION_TYPE_DHCP_SERVER_IDENTIFIER				0x36
#define	DHCP_OPTION_TYPE_MAX_DHCP_MESSAGE_SIZE				0x39
#define	DHCP_OPTION_TYPE_CLIENT_IDENTIFIER				0x3D
#define	DHCP_OPTION_TYPE_AUTO_CONFIGURATION				0x74
#define	DHCP_OPTION_TYPE_END						0xFF

#define	DHCP_OPTION_TYPE_REQUEST_DHCPDISCOVER				0x01
#define	DHCP_OPTION_TYPE_REQUEST_DHCPOFFER				0x02
#define	DHCP_OPTION_TYPE_REQUEST_DHCPREQUEST				0x03
#define	DHCP_OPTION_TYPE_REQUEST_DHCPDECLINE				0x04
#define	DHCP_OPTION_TYPE_REQUEST_DHCPACK				0x05
#define	DHCP_OPTION_TYPE_REQUEST_DHCPNAK				0x06
#define	DHCP_OPTION_TYPE_REQUEST_DHCPRELEASE				0x07
#define	DHCP_OPTION_TYPE_REQUEST_DHCPINFORM				0x08
#define	DHCP_OPTION_TYPE_REQUEST_LIST					0x37

#define	DHCP_OPTION_TYPE_CLIENT_IDENTIFIER_HARDWARE_TYPE_ethernet	0x01

enum DHCP_ACTION { Discover, Offer, Request, Answer };

struct DHCP_STRUCTURE {
	uint8_t		op_code;
	uint8_t		hardware_type;
	uint8_t		hardware_length;
	uint8_t		hops;
	uint32_t	transaction_id;
	uint16_t	seconds;
	uint16_t	flags;
	uint32_t	client_ipv4_address;
	uint32_t	your_ipv4_address;
	uint32_t	server_ipv4_address;
	uint32_t	gateway_ipv4_address;
	uint8_t		client_hardware_address[ 16 ];
	uint8_t		server_name[ 64 ];
	uint8_t		boot_file_name[ 128 ];
	uint32_t	magic_cookie;
	uint8_t		options;
} __attribute__( (packed) );

struct DHCP_STRUCTURE_OPTION {
	uint8_t	type;
	uint8_t	length;
} __attribute__( (packed) );

struct DHCP_STRUCTURE_OPTION_BROADCAST_ADDRESS {
	struct	DHCP_STRUCTURE_OPTION	option;
	uint32_t	ipv4_address;
} __attribute__( (packed) );

struct DHCP_STRUCTURE_OPTION_CLIENT_IDENTIFIER {
	struct	DHCP_STRUCTURE_OPTION	option;
	uint8_t	hardware_type;
	uint8_t	client_hardware_address[ 6 ];
} __attribute__( (packed) );

struct DHCP_STRUCTURE_OPTION_DHCP_SERVER_IDENTIFIER {
	struct	DHCP_STRUCTURE_OPTION	option;
	uint32_t	ipv4_address;
} __attribute__( (packed) );

struct DHCP_STRUCTURE_OPTION_DNS {
	struct	DHCP_STRUCTURE_OPTION	option;
} __attribute__( (packed) );

struct DHCP_STRUCTURE_OPTION_HOSTNAME {
	struct	DHCP_STRUCTURE_OPTION	option;
	uint8_t	name[ 255 ];
} __attribute__( (packed) );

struct DHCP_STRUCTURE_OPTION_MAX_DHCP_MESSAGE_SIZE {
	struct	DHCP_STRUCTURE_OPTION	option;
	uint16_t	byte;
} __attribute__( (packed) );

struct DHCP_STRUCTURE_OPTION_REQUEST {
	struct	DHCP_STRUCTURE_OPTION	option;
	uint8_t	dhcp;
} __attribute__( (packed) );

struct DHCP_STRUCTURE_OPTION_REQUEST_LIST {
	struct	DHCP_STRUCTURE_OPTION	option;
} __attribute__( (packed) );

struct DHCP_STRUCTURE_OPTION_REQUESTED_IP_ADDRESS {
	struct	DHCP_STRUCTURE_OPTION	option;
	uint32_t	ipv4_address;
} __attribute__( (packed) );


struct DHCP_STRUCTURE_OPTION_ROUTER {
	struct	DHCP_STRUCTURE_OPTION	option;
	uint32_t	ipv4_address;
} __attribute__( (packed) );

struct DHCP_STRUCTURE_OPTION_SUBNET_MASK {
	struct	DHCP_STRUCTURE_OPTION	option;
	uint32_t	ipv4_address;
} __attribute__( (packed) );

