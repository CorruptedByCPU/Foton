/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct MODULE_NETWORK_STRUCTURE_SOCKET *module_network_socket( void ) {
	// block access to socket list
	MACRO_LOCK( module_network_socket_semaphore );

	// search for closed socket
	for( uint64_t i = 0; i < MODULE_NETWORK_SOCKET_limit; i++ ) {
		// socket already in use?
		if( module_network_socket_list[ i ].pid ) continue;	// yes, leave it

		// register socket for current task
		module_network_socket_list[ i ].pid = kernel -> task_pid();

		// unlock
		MACRO_UNLOCK( module_network_socket_semaphore );

		// assign incomming data area
		module_network_socket_list[ i ].data_in = (uintptr_t *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( MODULE_NETWORK_SOCKET_DATA_limit * sizeof( uintptr_t ) ) >> STD_SHIFT_PAGE );

		// return socket pointer
		return (struct MODULE_NETWORK_STRUCTURE_SOCKET *) &module_network_socket_list[ i ];
	}

	// unlock
	MACRO_UNLOCK( module_network_socket_semaphore );

	// no available sockets for use
	return EMPTY;
}

void module_network_socket_close( struct MODULE_NETWORK_STRUCTURE_SOCKET *socket ) {
	// debug
	socket -> pid = EMPTY;
}

void module_network_socket_close_by_pid( int64_t pid ) {
	// search for ant socket related to selected process
	for( uint64_t i = 0; i < MODULE_NETWORK_SOCKET_limit; i++ ) {
		// owned by selected process?
		if( module_network_socket_list[ i ].pid == pid ) module_network_socket_close( (struct MODULE_NETWORK_STRUCTURE_SOCKET *) &module_network_socket_list[ i ] );	// yes, close it
	}
}

uint8_t module_network_socket_port( struct MODULE_NETWORK_STRUCTURE_SOCKET *socket, uint16_t port ) {
	// block access to socket list
	MACRO_LOCK( module_network_socket_port_semaphore );

	// by default port is free to use
	uint8_t allow = TRUE;

	// if port is not selected
	if( ! port ) {
		// try to find available port
		while( TRUE ) {
			// next retry
			allow = TRUE;

			// check this port
			port = module_network_socket_port_random( 32768 ) | 0x8000;

			// search for port in use
			for( uint64_t i = 0; allow && i < MODULE_NETWORK_SOCKET_limit; i++ ) {
				// port already in use?
				if( module_network_socket_list[ i ].port_local == port ) allow = FALSE;	// yes, god dammit
			}

			// found?
			if( allow ) break;	// yes
		}
	} else
		// search for port in use
		for( uint64_t i = 0; allow && i < MODULE_NETWORK_SOCKET_limit; i++ ) {
			// port already in use?
			if( module_network_socket_list[ i ].port_local == port ) allow = FALSE;	// yes, god dammit
		}

	// port not in use?
	if( allow ) socket -> port_local = port;	// reserve

	// unlock
	MACRO_UNLOCK( module_network_socket_port_semaphore );

	// port already in use
	return allow;
}

uint16_t module_network_socket_port_random( uint16_t limit ) {
	// retrieve next state
	uint64_t random = kernel -> time_unit;

	// make some xor shifts
	random ^= random >> 12;
	random ^= random << 25;
	random ^= random >> 27;

	// return presudorandom value
	return (random * UINT64_C( 2685821657736338717 )) % limit;
}