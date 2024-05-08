/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define	MODULE_NETWORK_YX_limit						512

#define	MODULE_NETWORK_SOCKET_limit					256
#define	MODULE_NETWORK_ARP_limit					1024

#define	MODULE_NETWORK_SOCKET_FLAG_active				0b00000001
#define	MODULE_NETWORK_SOCKET_FLAG_init					0b10000000

#define	MODULE_NETWORK_HEADER_ETHERNET_TYPE_arp				0x0608	// Big-Endian
#define	MODULE_NETWORK_HEADER_ETHERNET_TYPE_ipv4			0x0008	// Big-Endian
#define	MODULE_NETWORK_HEADER_ETHERNET_TYPE_ipv6			0xDD86	// Big-Endian

#define	MODULE_NETWORK_HEADER_ARP_HARDWARE_TYPE_ethernet		0x0100	// Big-Endian
#define	MODULE_NETWORK_HEADER_ARP_PROTOCOL_TYPE_ipv4			0x0008	// Big-Endian
#define	MODULE_NETWORK_HEADER_ARP_HARDWARE_LENGTH_mac			0x06	// xx:xx:xx:xx:xx:xx
#define	MODULE_NETWORK_HEADER_ARP_PROTOCOL_LENGTH_ipv4			0x04	// x.x.x.x
#define	MODULE_NETWORK_HEADER_ARP_OPERATION_request			0x0100	// Big-Endian
#define	MODULE_NETWORK_HEADER_ARP_OPERATION_answer			0x0200	// Big-Endian

#define	MODULE_NETWORK_HEADER_IPV4_VERSION_AND_HEADER_LENGTH_default	0x45	// version: 0100, length: 0101
#define	MODULE_NETWORK_HEADER_IPV4_ECN_default 				0x00
#define	MODULE_NETWORK_HEADER_IPV4_FLAGS_AND_OFFSET_default		0x0040	// Big-Endian
#define	MODULE_NETWORK_HEADER_IPV4_TTL_default				0x40	// 64
#define	MODULE_NETWORK_HEADER_IPV4_PROTOCOL_icmp			0x01
#define	MODULE_NETWORK_HEADER_IPV4_PROTOCOL_tcp				0x06
#define	MODULE_NETWORK_HEADER_IPV4_PROTOCOL_udp				0x11

#define	MODULE_NETWORK_HEADER_ICMP_TYPE_REQUEST				0x08
#define	MODULE_NETWORK_HEADER_ICMP_TYPE_REPLY				0x00

struct	MODULE_NETWORK_STRUCTURE_ARP {
	uint64_t	lease_time;
	uint32_t	ipv4_address;
	uint8_t		mac_address[ 6 ];
};

struct	MODULE_NETWORK_STRUCTURE_HEADER_ARP {
	uint16_t	hardware_type;
	uint16_t	protocol_type;
	uint8_t		hardware_length;
	uint8_t		protocol_length;
	uint16_t	operation;
	uint8_t		source_mac[ 6 ];
	uint32_t	source_ipv4;
	uint8_t		target_mac[ 6 ];
	uint32_t	target_ipv4;
} __attribute__((packed));

struct	MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET {
	uint8_t 	target[ 6 ];
	uint8_t		source[ 6 ];
	uint16_t	type;
} __attribute__((packed));

struct MODULE_NETWORK_STRUCTURE_HEADER_ICMP {
	uint8_t		type;
	uint8_t		code;
	uint16_t	checksum;
} __attribute__((packed));

struct MODULE_NETWORK_STRUCTURE_HEADER_IPV4 {
	uint8_t		version_and_header_length;
	uint8_t		ecn;
	uint16_t	length;
	uint16_t	id;
	uint16_t	flags_and_offset;
	uint8_t		ttl;
	uint8_t		protocol;
	uint16_t	checksum;
	uint32_t	local;
	uint32_t	target;
} __attribute__((packed));

struct MODULE_NETWORK_STRUCTURE_HEADER_PSEUDO {
	uint32_t	local;
	uint32_t	target;
	uint8_t		reserved;
	uint8_t		protocol;
	uint16_t	length;
} __attribute__((packed));

struct MODULE_NETWORK_STRUCTURE_HEADER_UDP {
	uint16_t	local;
	uint16_t	target;
	uint16_t	length;
	uint16_t	checksum;
} __attribute__((packed));

struct	MODULE_NETWORK_STRUCTURE_SOCKET {
	int64_t		pid;
	uint8_t		flags;
	uint8_t		protocol;
	uint16_t	ethernet_type;
	uint8_t		ethernet_mac[ 6 ];
	uint16_t	port_local;
	uint16_t	port_target;
	uint8_t		ipv4_protocol;
	uint32_t	ipv4_target;
	uint16_t	ipv4_id;
	uint8_t		ipv4_ttl;
};

uint8_t module_network_arp( struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length );

void module_network_arp_list_add( struct MODULE_NETWORK_STRUCTURE_HEADER_ARP *arp );

struct MODULE_NETWORK_STRUCTURE_ARP *module_network_arp_list_search( uint32_t ipv4_address );

uint16_t module_network_checksum( uint16_t *data, uint16_t length );

void module_network_ethernet_encapsulate( struct MODULE_NETWORK_STRUCTURE_SOCKET *socket, struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length );

void module_network_icmp( struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length );

// network module initialization
void module_network_init( void );

uint8_t module_network_ipv4( struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length );

void module_network_ipv4_encapsulate( struct MODULE_NETWORK_STRUCTURE_SOCKET *socket, struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length );

void module_network_ipv4_exit( struct MODULE_NETWORK_STRUCTURE_SOCKET *socket, uint8_t *data, uint16_t length );

// storage function for incomming packets
void module_network_rx( uintptr_t frame );

int64_t module_network_send( int64_t socket, uint8_t *data, uint64_t length );

struct MODULE_NETWORK_STRUCTURE_SOCKET *module_network_socket( void );

uint8_t module_network_socket_port( struct MODULE_NETWORK_STRUCTURE_SOCKET *socket, uint16_t port );

void module_network_arp_thread( void );

// returns physical address and size of packet to transfer outside of host
uintptr_t module_network_tx( void );

void module_network_udp( struct MODULE_NETWORK_STRUCTURE_SOCKET *socket, uint8_t *data, uint64_t length );

void module_network_udp_encapsulate( struct MODULE_NETWORK_STRUCTURE_SOCKET *socket, struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length );