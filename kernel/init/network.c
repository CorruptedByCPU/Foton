/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_network( void ) {
	// assign area for incomming/outgoing frames
	kernel -> network_rx_base_address = (uint64_t *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( KERNEL_NETWORK_YX_limit * sizeof( uintptr_t ) ) >> STD_SHIFT_PAGE );
	kernel -> network_tx_base_address = (uint64_t *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( KERNEL_NETWORK_YX_limit * sizeof( uintptr_t ) ) >> STD_SHIFT_PAGE );

	// share in/out frames functions
	kernel -> network_rx = (void *) kernel_network_rx;
	kernel -> network_tx = (void *) kernel_network_tx;

	// assign area for connection sockets
	kernel -> network_socket_list = (struct KERNEL_STRUCTURE_NETWORK_SOCKET *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( KERNEL_NETWORK_SOCKET_limit * sizeof( struct KERNEL_STRUCTURE_NETWORK_SOCKET ) ) >> STD_SHIFT_PAGE );

	// share socket function and offset
	kernel -> network_socket_close_by_pid = (void *) kernel_network_socket_close_by_pid;

	// open dummy socket, as socket with ID 0, cannot be used
	kernel_network_socket();

	// start Network thread
	uint8_t network_string_thread_name[] = "network";
	kernel_module_thread( (uintptr_t) &kernel_network, (uint8_t *) &network_string_thread_name, sizeof( network_string_thread_name ) );

	// start Network ARP thread
	uint8_t network_string_thread_name_arp[] = "network arp";
	kernel_module_thread( (uintptr_t) &kernel_network_arp_thread, (uint8_t *) &network_string_thread_name_arp, sizeof( network_string_thread_name_arp ) );

	// start Network TCP thread
	uint8_t network_string_thread_name_tcp[] = "network tcp";
	kernel_module_thread( (uintptr_t) &kernel_network_tcp_thread, (uint8_t *) &network_string_thread_name_tcp, sizeof( network_string_thread_name_tcp ) );
}