/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define	MODULE_NETWORK_YX_limit						512

#define	MODULE_NETWORK_PORT_limit					128
#define	MODULE_NETWORK_SOCKET_limit					32

#define	MODULE_NETWORK_SOCKET_FLAG_active				0b00000001
#define	MODULE_NETWORK_SOCKET_FLAG_init					0b10000000

#define	MODULE_NETWORK_SOCKET_PROTOCOL_arp				1

#define	MODULE_NETWORK_HEADER_ETHERNET_TYPE_arp				0x0608	// Big-Endian
#define	MODULE_NETWORK_HEADER_ETHERNET_TYPE_ipv4			0x0008	// Big-Endian
#define	MODULE_NETWORK_HEADER_ETHERNET_TYPE_ipv6			0xDD86	// Big-Endian

#define	MODULE_NETWORK_HEADER_ARP_HARDWARE_ETHERNET_type		0x0100	// Big-Endian
#define	MODULE_NETWORK_HEADER_ARP_PROTOCOL_IPV4_type			0x0008	// Big-Endian
#define	MODULE_NETWORK_HEADER_ARP_HARDWARE_MAC_length			0x06	// xx:xx:xx:xx:xx:xx
#define	MODULE_NETWORK_HEADER_ARP_PROTOCOL_IPV4_length			0x04	// x.x.x.x
#define	MODULE_NETWORK_HEADER_ARP_OPERATION_request			0x0100	// Big-Endian
#define	MODULE_NETWORK_HEADER_ARP_OPERATION_answer			0x0200	// Big-Endian

#define	MODULE_NETWORK_HEADER_IPV4_VERSION_AND_HEADER_LENGTH_default	0x45	// version: 0100, length: 0101
#define	MODULE_NETWORK_HEADER_IPV4_ECN_default 				0x00
#define	MODULE_NETWORK_HEADER_IPV4_FLAGS_AND_OFFSET_default		0x0040	// Big-Endian
#define	MODULE_NETWORK_HEADER_IPV4_TTL_default				0x40	// 64
#define	MODULE_NETWORK_HEADER_IPV4_PROTOCOL_icmp			STD_NETWORK_PROTOCOL_icmp
#define	MODULE_NETWORK_HEADER_IPV4_PROTOCOL_tcp				STD_NETWORK_PROTOCOL_tcp
#define	MODULE_NETWORK_HEADER_IPV4_PROTOCOL_udp				STD_NETWORK_PROTOCOL_udp

struct	MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET {
	uint8_t 	target[ 6 ];
	uint8_t		source[ 6 ];
	uint16_t	type;
} __attribute__((packed));

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

struct MODULE_NETWORK_STRUCTURE_HEADER_IPV4 {
	uint8_t		version_and_header_length;
	uint8_t		ecn;
	uint16_t	length;
	uint16_t	id;
	uint16_t	flags_and_offset;
	uint8_t		ttl;
	uint8_t		protocol;
	uint16_t	checksum;
	uint32_t	source;
	uint32_t	target;
} __attribute__((packed));

struct MODULE_NETWORK_STRUCTURE_HEADER_ICMP {
	uint8_t		type;
	uint8_t		code;
	uint16_t	checksum;
	uint32_t	reserved;
	uint8_t		data[ 32 ];
} __attribute__((packed));

struct	MODULE_NETWORK_STRUCTURE_SOCKET {
	int64_t		pid;
	uint8_t		flags;
	uint8_t		protocol;
	uint8_t		ethernet_mac[ 6 ];
	uint16_t	port_local;
	uint16_t	port_target;
	uint32_t	ipv4_target;
};

uint8_t module_network_arp( struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length );

void module_network_ethernet_encapsulate( struct MODULE_NETWORK_STRUCTURE_SOCKET *socket, struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length );

// network module initialization
void module_network_init( void );

uint8_t module_network_ipv4( struct MODULE_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length );

uint8_t module_network_port( uint16_t port );

// storage function for incomming packets
void module_network_rx( uintptr_t packet );

struct MODULE_NETWORK_STRUCTURE_SOCKET *module_network_socket( void );

// returns physical address and size of packet to transfer outside of host
uintptr_t module_network_tx( void );