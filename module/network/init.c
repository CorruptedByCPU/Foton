/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void module_network_init( void ) {
	// assign area for incomming/outgoing frames
	module_network_rx_base_address = (uint64_t *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( MODULE_NETWORK_YX_limit * sizeof( uintptr_t ) ) >> STD_SHIFT_PAGE );
	module_network_tx_base_address = (uint64_t *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( MODULE_NETWORK_YX_limit * sizeof( uintptr_t ) ) >> STD_SHIFT_PAGE );

	// share in/out frames functions
	kernel -> network_rx = (void *) module_network_rx;
	kernel -> network_tx = (void *) module_network_tx;

	// share send/receive functions
	kernel -> network_send = (void *) module_network_send;

	// assign area for connection sockets
	module_network_socket_list = (struct MODULE_NETWORK_STRUCTURE_SOCKET *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( MODULE_NETWORK_SOCKET_limit * sizeof( struct MODULE_NETWORK_STRUCTURE_SOCKET ) ) >> STD_SHIFT_PAGE );

	// share socket function and offset
	kernel -> network_socket = (void *) module_network_socket;
	kernel -> network_socket_offset = (uintptr_t) module_network_socket_list;
	kernel -> network_socket_port = (void *) module_network_socket_port;

	// open dummy socket, as socket with ID 0, cannot be used
	module_network_socket();

	// assign area for ARP list
	module_network_arp_list = (struct MODULE_NETWORK_STRUCTURE_ARP *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( MODULE_NETWORK_ARP_limit * sizeof( struct MODULE_NETWORK_STRUCTURE_ARP ) ) >> STD_SHIFT_PAGE );

	// enable thread for ARP resolving
	uint8_t network_string_thread_name[] = "network arp";
	network_thread_pid = kernel -> module_thread( (uintptr_t) &module_network_arp_thread, (uint8_t *) &network_string_thread_name, sizeof( network_string_thread_name ) );
}