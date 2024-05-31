/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define	KERNEL_NETWORK_INTERFACE_limit					2

#define	KERNEL_NETWORK_YX_limit						512

#define	KERNEL_NETWORK_SOCKET_limit					256
#define	KERNEL_NETWORK_ARP_limit					1024

#define	KERNEL_NETWORK_SOCKET_FLAG_active				0b00000001
#define	KERNEL_NETWORK_SOCKET_FLAG_pause				0b00100000
#define	KERNEL_NETWORK_SOCKET_FLAG_close				0b01000000
#define	KERNEL_NETWORK_SOCKET_FLAG_init					0b10000000
#define	KERNEL_NETWORK_SOCKET_DATA_limit				511	// packets, last entry 512th always empty

#define	KERNEL_NETWORK_HEADER_ETHERNET_TYPE_arp				0x0806
#define	KERNEL_NETWORK_HEADER_ETHERNET_TYPE_ipv4			0x0800
#define	KERNEL_NETWORK_HEADER_ETHERNET_TYPE_ipv6			0x86DD

#define	KERNEL_NETWORK_HEADER_ARP_HARDWARE_TYPE_ethernet		0x0001
#define	KERNEL_NETWORK_HEADER_ARP_PROTOCOL_TYPE_ipv4			0x0800
#define	KERNEL_NETWORK_HEADER_ARP_HARDWARE_LENGTH_mac			0x06	// xx:xx:xx:xx:xx:xx
#define	KERNEL_NETWORK_HEADER_ARP_PROTOCOL_LENGTH_ipv4			0x04	// x.x.x.x
#define	KERNEL_NETWORK_HEADER_ARP_OPERATION_request			0x0001
#define	KERNEL_NETWORK_HEADER_ARP_OPERATION_answer			0x0002

#define	KERNEL_NETWORK_HEADER_IPV4_VERSION_AND_HEADER_LENGTH_default	0x45	// version: 0100, length: 0101
#define	KERNEL_NETWORK_HEADER_IPV4_ECN_default 				0x00
#define	KERNEL_NETWORK_HEADER_IPV4_FLAGS_AND_OFFSET_default		0x4000
#define	KERNEL_NETWORK_HEADER_IPV4_TTL_default				0x40	// 64
#define	KERNEL_NETWORK_HEADER_IPV4_PROTOCOL_icmp			0x01
#define	KERNEL_NETWORK_HEADER_IPV4_PROTOCOL_tcp				0x06
#define	KERNEL_NETWORK_HEADER_IPV4_PROTOCOL_udp				0x11

#define	KERNEL_NETWORK_HEADER_ICMP_TYPE_REQUEST				0x08
#define	KERNEL_NETWORK_HEADER_ICMP_TYPE_REPLY				0x00

#define	KERNEL_NETWORK_HEADER_TCP_HEADER_LENGTH_default			0x50	// 20 Bytes
#define	KERNEL_NETWORK_HEADER_TCP_FLAG_FIN				0b00000001
#define	KERNEL_NETWORK_HEADER_TCP_FLAG_SYN				0b00000010
#define	KERNEL_NETWORK_HEADER_TCP_FLAG_RST				0b00000100
#define	KERNEL_NETWORK_HEADER_TCP_FLAG_PSH				0b00001000
#define	KERNEL_NETWORK_HEADER_TCP_FLAG_ACK				0b00010000
#define	KERNEL_NETWORK_HEADER_TCP_FLAG_URG				0b00100000
#define	KERNEL_NETWORK_HEADER_TCP_WINDOW_SIZE_default			0x05B4
#define	KERNEL_NETWORK_HEADER_TCP_KEEP_ALIVE_timeout			(DRIVER_RTC_Hz * 3600)

struct	KERNEL_NETWORK_STRUCTURE_ARP {
	uint64_t	lease_time;
	uint32_t	ipv4_address;
	uint8_t		mac_address[ 6 ];
};

struct	KERNEL_NETWORK_STRUCTURE_HEADER_ARP {
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

struct	KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET {
	uint8_t 	target[ 6 ];
	uint8_t		source[ 6 ];
	uint16_t	type;
} __attribute__((packed));

struct KERNEL_NETWORK_STRUCTURE_HEADER_ICMP {
	uint8_t		type;
	uint8_t		code;
	uint16_t	checksum;
} __attribute__((packed));

struct KERNEL_NETWORK_STRUCTURE_HEADER_IPV4 {
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

struct KERNEL_NETWORK_STRUCTURE_HEADER_PSEUDO {
	uint32_t	local;
	uint32_t	target;
	uint8_t		reserved;
	uint8_t		protocol;
	uint16_t	length;
} __attribute__((packed));

struct KERNEL_NETWORK_STRUCTURE_HEADER_UDP {
	uint16_t	local;
	uint16_t	target;
	uint16_t	length;
	uint16_t	checksum;
} __attribute__((packed));

struct KERNEL_NETWORK_STRUCTURE_HEADER_TCP {
	uint16_t	port_source;
	uint16_t	port_local;
	uint32_t	sequence;
	uint32_t	acknowledgment;
	uint8_t		header_length;
	uint8_t		flags;
	uint16_t	window_size;
	uint16_t	checksum;
	uint16_t	urgent_pointer;
} __attribute__((packed));

struct	KERNEL_NETWORK_STRUCTURE_SOCKET {
	int64_t		pid;
	uint8_t		flags;
	uint8_t		protocol;
	uint16_t	ethernet_type;
	uint8_t		ethernet_mac[ 6 ];
	volatile uint64_t	ethernet_mac_lease;
	uint16_t	port_local;
	uint16_t	port_target;
	uint8_t		ipv4_protocol;
	uint32_t	ipv4_target;
	uint16_t	ipv4_id;
	uint8_t		ipv4_ttl;
	uint32_t	tcp_sequence;
	uint32_t	tcp_acknowledgment;
	uint32_t	tcp_acknowledgment_required;
	uint8_t		tcp_flags;
	uint16_t	tcp_window_size;
	uint64_t	tcp_keep_alive;
	uintptr_t	*data_in;
	uint8_t		data_in_semaphore;
	uintptr_t	*data_out;
	uint8_t		data_out_semaphore;
};

uint8_t kernel_network_arp( struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length );

uint16_t kernel_network_checksum( uint16_t *data, uint16_t length );

void kernel_network_data_in( struct KERNEL_NETWORK_STRUCTURE_SOCKET *socket, uintptr_t packet );

void kernel_network_ethernet_encapsulate( struct KERNEL_NETWORK_STRUCTURE_SOCKET *socket, struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length );

uint8_t kernel_network_icmp( struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length );

// network module initialization
void kernel_network_init( void );

uint8_t kernel_network_ipv4( struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length );

void kernel_network_ipv4_encapsulate( struct KERNEL_NETWORK_STRUCTURE_SOCKET *socket, struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length );

void kernel_network_ipv4_exit( struct KERNEL_NETWORK_STRUCTURE_SOCKET *socket, uint8_t *data, uint16_t length );

void kernel_network_receive( int64_t socket, struct STD_NETWORK_STRUCTURE_DATA *packet );

// storage function for incomming packets
void kernel_network_rx( uintptr_t frame );

int64_t kernel_network_send( int64_t socket, uint8_t *data, uint64_t length );

struct KERNEL_NETWORK_STRUCTURE_SOCKET *kernel_network_socket( void );

void kernel_network_socket_close( struct KERNEL_NETWORK_STRUCTURE_SOCKET *socket );

void kernel_network_socket_close_by_pid( int64_t pid );

uint8_t kernel_network_socket_port( struct KERNEL_NETWORK_STRUCTURE_SOCKET *socket, uint16_t port );

uint16_t kernel_network_socket_port_random( uint16_t limit );

void kernel_network_arp_thread( void );

void kernel_network_tcp_thread( void );

// returns physical address and size of packet to transfer outside of host
uintptr_t kernel_network_tx( void );

void kernel_network_udp_exit( struct KERNEL_NETWORK_STRUCTURE_SOCKET *socket, uint8_t *data, uint64_t length );

void kernel_network_udp_encapsulate( struct KERNEL_NETWORK_STRUCTURE_SOCKET *socket, struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length );

uint8_t kernel_network_ethernet_resolve( struct KERNEL_NETWORK_STRUCTURE_SOCKET *socket );

uint8_t kernel_network_tcp( struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length );

void kernel_network_tcp_encapsulate( struct KERNEL_NETWORK_STRUCTURE_SOCKET *socket, struct KERNEL_NETWORK_STRUCTURE_HEADER_ETHERNET *ethernet, uint16_t length );