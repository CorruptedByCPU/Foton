/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct KERNEL_STRUCTURE_NETWORK_SOCKET *kernel_network_socket( void ) {
	// block access to socket list
	MACRO_LOCK( kernel -> network_socket_semaphore );

	// search for closed socket
	for( uint64_t i = 0; i < KERNEL_NETWORK_SOCKET_limit; i++ ) {
		// socket already in use?
		if( kernel -> network_socket_list[ i ].pid ) continue;	// yes, leave it

		// register socket for current task
		kernel -> network_socket_list[ i ].pid = kernel_task_pid();

		// clear flags, socket in undefinied state
		kernel -> network_socket_list[ i ].flags = EMPTY;

		// unlock
		MACRO_UNLOCK( kernel -> network_socket_semaphore );

		// assign incomming data area
		kernel -> network_socket_list[ i ].data_in = (uintptr_t *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( KERNEL_NETWORK_SOCKET_DATA_limit * sizeof( uintptr_t ) ) >> STD_SHIFT_PAGE );

		// return socket pointer
		return (struct KERNEL_STRUCTURE_NETWORK_SOCKET *) &kernel -> network_socket_list[ i ];
	}

	// unlock
	MACRO_UNLOCK( kernel -> network_socket_semaphore );

	// no available sockets for use
	return EMPTY;
}

void kernel_network_socket_close( struct KERNEL_STRUCTURE_NETWORK_SOCKET *socket ) {
	// remove all packets from incomming queue
	for( uint64_t i = 0; i < KERNEL_NETWORK_SOCKET_DATA_limit; i++ )
		// unparsed packet?
		if( socket -> data_in[ i ] ) kernel_memory_release( MACRO_PAGE_ALIGN_DOWN( socket -> data_in[ i ] ), MACRO_PAGE_ALIGN_UP( socket -> data_in[ i ] & ~STD_PAGE_mask ) >> STD_SHIFT_PAGE );

	// release queue itself
	kernel_memory_release( (uintptr_t) socket -> data_in, MACRO_PAGE_ALIGN_UP( KERNEL_NETWORK_SOCKET_DATA_limit * sizeof( uintptr_t ) ) >> STD_SHIFT_PAGE );

	// release socket
	socket -> pid = EMPTY;
}

void kernel_network_socket_close_by_pid( int64_t pid ) {
	// search for ant socket related to selected process
	for( uint64_t i = 0; i < KERNEL_NETWORK_SOCKET_limit; i++ ) {
		// owned by selected process?
		if( kernel -> network_socket_list[ i ].pid == pid ) {
			// mark socket as closed
			kernel -> network_socket_list[ i ].flags |= KERNEL_NETWORK_SOCKET_FLAG_close;

			// release socket
			kernel_network_socket_close( (struct KERNEL_STRUCTURE_NETWORK_SOCKET *) &kernel -> network_socket_list[ i ] );
		}
	}
}

uint8_t kernel_network_socket_port( struct KERNEL_STRUCTURE_NETWORK_SOCKET *socket, uint16_t port ) {
	// block access to socket list
	MACRO_LOCK( kernel -> network_socket_port_semaphore );

	// by default port is free to use
	uint8_t allow = TRUE;

	// if port is not selected
	if( ! port ) {
		// try to find available port
		while( TRUE ) {
			// next retry
			allow = TRUE;

			// check this port
			port = kernel_network_socket_port_random( 32768 ) | 0x8000;

			// search for port in use
			for( uint64_t i = 0; allow && i < KERNEL_NETWORK_SOCKET_limit; i++ ) {
				// port already in use?
				if( kernel -> network_socket_list[ i ].port_local == port ) allow = FALSE;	// yes, god dammit
			}

			// found?
			if( allow ) break;	// yes
		}
	} else
		// search for port in use
		for( uint64_t i = 0; allow && i < KERNEL_NETWORK_SOCKET_limit; i++ ) {
			// port already in use?
			if( kernel -> network_socket_list[ i ].port_local == port ) allow = FALSE;	// yes, god dammit
		}

	// port not in use?
	if( allow ) socket -> port_local = port;	// reserve

	// unlock
	MACRO_UNLOCK( kernel -> network_socket_port_semaphore );

	// port already in use
	return allow;
}

uint16_t kernel_network_socket_port_random( uint16_t limit ) {
	// retrieve next state
	uint64_t random = kernel -> time_rtc;

	// make some xor shifts
	random ^= random >> 12;
	random ^= random << 25;
	random ^= random >> 27;

	// return presudorandom value
	return (random * UINT64_C( 2685821657736338717 )) % limit;
}
