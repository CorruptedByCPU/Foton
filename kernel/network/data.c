/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct KERNEL *kernel = EMPTY;

uintptr_t *kernel -> network_rx_base_address				= EMPTY;
volatile uint64_t kernel -> network_rx_limit				= EMPTY;
volatile uint8_t kernel -> network_rx_semaphore				= FALSE;

uintptr_t *kernel -> network_tx_base_address				= EMPTY;
volatile uint64_t kernel -> network_tx_limit				= EMPTY;
volatile uint8_t kernel -> network_tx_semaphore				= FALSE;

uint32_t kernel -> network_multicast_address				= 0xFFFFFFFF;	// 255.255.255.255

struct KERNEL_NETWORK_STRUCTURE_SOCKET *kernel -> network_socket_list	= EMPTY;
uint8_t kernel -> network_socket_semaphore					= FALSE;
uint8_t kernel -> network_socket_port_semaphore				= FALSE;

struct KERNEL_NETWORK_STRUCTURE_ARP *kernel -> network_arp_list		= EMPTY;
uint8_t kernel -> network_arp_list_semaphore				= FALSE;

int64_t network_thread_pid						= EMPTY;