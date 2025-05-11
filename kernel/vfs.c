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

struct KERNEL_STRUCTURE_VFS_FILE kernel_vfs_file( struct KERNEL_STRUCTURE_STORAGE *storage, uintptr_t directory, uint8_t *name, uint64_t limit ) {
	// properties of directory
	struct LIB_VFS_STRUCTURE *dir = (struct LIB_VFS_STRUCTURE *) directory;

	// for each data block of directory
	for( uint64_t b = 0; b < (dir -> limit >> STD_SHIFT_PAGE); b++ ) {
		// properties of directory entry
		struct LIB_VFS_STRUCTURE *vfs = (struct LIB_VFS_STRUCTURE *) kernel_vfs_block_by_id( dir, b );

		// for every possible entry
		for( uint8_t e = 0; e < STD_PAGE_byte / sizeof( struct LIB_VFS_STRUCTURE ); e++ )
			// if found
			if( vfs[ e ].name_limit == limit && lib_string_compare( (uint8_t *) vfs[ e ].name, name, limit ) ) {
				// properties of found file
				struct LIB_VFS_STRUCTURE *file = (struct LIB_VFS_STRUCTURE *) &vfs[ e ];

				// follow symlink, if exist
				while( file -> type == STD_FILE_TYPE_link ) file = (struct LIB_VFS_STRUCTURE *) file -> block[ FALSE ];

				// done
				return (struct KERNEL_STRUCTURE_VFS_FILE) { file -> type, file -> limit, (uintptr_t) file };
			}
	}

	// debug
	return (struct KERNEL_STRUCTURE_VFS_FILE) { EMPTY };
}

void kernel_vfs_file_read( struct KERNEL_STRUCTURE_VFS_SOCKET *socket, uint8_t *target, uint64_t seek, uint64_t limit ) {
	// properties of file
	struct LIB_VFS_STRUCTURE *vfs = (struct LIB_VFS_STRUCTURE *) socket -> file.knot;

	// first block of data
	uint64_t b = MACRO_PAGE_ALIGN_DOWN( seek ) >> STD_SHIFT_PAGE;

	// read first part of file
	seek %= STD_PAGE_byte;

	// source block pointer
	uint8_t *source;

	// read all blocks of requested data
	while( limit ) {
		// select block pointer
		source = (uint8_t *) kernel_vfs_block_by_id( vfs, b++ );

		// full or part of block?
		uint64_t x = STD_PAGE_byte;
		if( x > limit ) { x = limit; limit = EMPTY; }
		else limit -= x;

		// copy data from block
		for( uint64_t i = seek; i < x; i++ ) *(target++) = source[ i ];

		// start from begining of next block
		seek = EMPTY;
	}
}

void kernel_vfs_dir( struct KERNEL_STRUCTURE_VFS_SOCKET *socket, struct LIB_VFS_STRUCTURE *dir ) {
	// properties of file
	struct LIB_VFS_STRUCTURE *vfs = (struct LIB_VFS_STRUCTURE *) socket -> file.knot;

	// properties of current block of directory
	struct LIB_VFS_STRUCTURE *tmp = EMPTY;

	// first block of data
	uint64_t b = 0;

	// parse all files from each block
	while( TRUE ) {
		// source block pointer
		if( ! (tmp = (struct LIB_VFS_STRUCTURE *) kernel_vfs_block_by_id( vfs, b++ )) ) break;	// done, nothing more

		// for every possible entry
		for( uint8_t i = 0; i < STD_PAGE_byte / sizeof( struct LIB_VFS_STRUCTURE ); i++ ) {
			// if not exist
			if( ! tmp[ i ].type ) continue; // next one

			// share important infomration about file

			// type of file
			dir -> type	= tmp[ i ].type;

			// length in Bytes
			dir -> limit	= tmp[ i ].limit;

			// name
			dir -> name_limit	= tmp[ i ].name_limit;
			for( uint8_t k = 0; k < tmp[ i ].name_limit; k++ ) dir -> name[ k ] = tmp[ i ].name[ k ];

			// file shared
			dir++;
		}
	}
}

struct KERNEL_STRUCTURE_VFS_SOCKET *kernel_vfs_socket( uint64_t pid ) {
	// exclusive access
	MACRO_LOCK( kernel -> vfs_lock );

	// properties of available socket
	struct KERNEL_STRUCTURE_VFS_SOCKET *socket = EMPTY;

	// search thru all sockets
	for( uint64_t i = 1; i < kernel -> vfs_limit; i++ )
		// available for use?
		if( ! kernel -> vfs_base_address[ i ].pid ) {
			// yes
			socket = (struct KERNEL_STRUCTURE_VFS_SOCKET *) &kernel -> vfs_base_address[ i ];

			// reserved by
			socket -> pid = pid;

			// done
			break;
		}

	// release
	MACRO_UNLOCK( kernel -> vfs_lock );

	// socket
	return socket;
}

void kernel_vfs_socket_delete( struct KERNEL_STRUCTURE_VFS_SOCKET *socket ) {
	// current task properties
	struct KERNEL_STRUCTURE_TASK *current = (struct KERNEL_STRUCTURE_TASK *) kernel_task_current();

	// socket belongs to process?
	if( current -> pid != socket -> pid ) return;	// no, ignore

	// close connection to file
	socket -> pid = EMPTY;
}
