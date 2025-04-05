/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_VFS
	#define	KERNEL_VFS

	#ifndef	LIBRARY_VFS
		#include	"../library/vfs.h"
	#endif

	#define	KERNEL_VFS_limit			(STD_PAGE_byte / sizeof( struct KERNEL_STRUCTURE_VFS_SOCKET ))

	struct KERNEL_STRUCTURE_VFS {
		uint64_t				root;
		struct KERNEL_STRUCTURE_VFS_FILE	(*file)( struct KERNEL_STRUCTURE_STORAGE *storage, uintptr_t directory, uint8_t *path, uint64_t limit );
		void					(*file_read)( struct KERNEL_STRUCTURE_VFS_SOCKET *socket, uint8_t *target, uint64_t seek, uint64_t limit );
	};

	struct KERNEL_STRUCTURE_VFS_FILE {
		uint8_t					type;
		uint64_t				limit;
		uint64_t				knot;
	};

	struct	KERNEL_STRUCTURE_VFS_SOCKET {
		uint64_t				storage;
		struct KERNEL_STRUCTURE_VFS_FILE	file;
		int64_t					pid;
	};

	// pointer address to block content
	uintptr_t kernel_vfs_block_by_id( struct LIB_VFS_STRUCTURE *vfs, uint64_t b );

	// properties of opened file
	struct KERNEL_STRUCTURE_VFS_SOCKET *kernel_vfs_socket( uint64_t knot );

	// releases file
	void kernel_vfs_socket_delete( struct KERNEL_STRUCTURE_VFS_SOCKET *socket );
#endif
