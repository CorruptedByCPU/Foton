/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct KERNEL *kernel = EMPTY;

uintptr_t *module_network_rx_base_address				= EMPTY;
volatile uint64_t module_network_rx_limit				= EMPTY;
volatile uint8_t module_network_rx_semaphore				= FALSE;

uintptr_t *module_network_tx_base_address				= EMPTY;
volatile uint64_t module_network_tx_limit				= EMPTY;
volatile uint8_t module_network_tx_semaphore				= FALSE;

uint32_t module_network_multicast_address				= 0xFFFFFFFF;	// 255.255.255.255

struct MODULE_NETWORK_STRUCTURE_SOCKET *module_network_socket_list	= EMPTY;
uint8_t module_network_socket_semaphore					= FALSE;
uint8_t module_network_socket_port_semaphore				= FALSE;

struct MODULE_NETWORK_STRUCTURE_ARP *module_network_arp_list		= EMPTY;
uint8_t module_network_arp_list_semaphore				= FALSE;

int64_t network_thread_pid						= EMPTY;