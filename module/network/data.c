/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct KERNEL *kernel = EMPTY;

uintptr_t *module_network_rx_base_address	= EMPTY;
uint64_t module_network_rx_limit		= EMPTY;
uint8_t module_network_rx_semaphore		= FALSE;

uint32_t module_network_ipv4_address		= EMPTY;	// 0.0.0.0
uint32_t module_network_multicast_address	= 0xFFFFFFFF;	// 255.255.255.255