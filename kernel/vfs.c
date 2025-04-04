/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uintptr_t kernel_vfs_block_by_id( struct LIB_VFS_STRUCTURE *vfs, uint64_t b ) {
	// direct block?
	if( ! b-- ) return vfs -> block[ 0 ];	// return pointer address

	// indirect block?
	if( b < 512 ) {
		// properties of indirect block
		uintptr_t *indirect = (uintptr_t *) vfs -> block[ 1 ];

		// return pointer address
		return indirect[ b ];
	}

	// no support for double-indirect and so on, yet
	return EMPTY;
}

struct KERNEL_STRUCTURE_VFS_SOCKET *kernel_vfs_socket( uint64_t knot ) {
	// exclusive access
	MACRO_LOCK( kernel -> vfs_lock );

	// properties of available socket
	struct KERNEL_STRUCTURE_VFS_SOCKET *socket = EMPTY;

	// search thru all sockets
	for( uint64_t i = INIT; i < kernel -> vfs_limit; i++ )
		// available for use?
		if( kernel -> vfs_base_address[ i ].knot ) {
			// yes
			socket = (struct KERNEL_STRUCTURE_VFS_SOCKET *) &kernel -> vfs_base_address[ i ];

			// reserved by
			socket -> pid = knot;

			// done
			break;
		}

	// release
	MACRO_UNLOCK( kernel -> vfs_lock );

	// socket
	return socket;
}
