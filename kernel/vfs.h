/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_VFS
	#define	KERNEL_VFS

	#ifndef	LIBRARY_VFS
		#include	"../library/vfs.h"
	#endif

	#define	KERNEL_VFS_limit			(STD_PAGE_byte / sizeof( struct KERNEL_STRUCTURE_VFS_SOCKET ))

	struct	KERNEL_STRUCTURE_VFS_SOCKET {
		uint64_t				storage;
		uint64_t				knot;
		int64_t					pid;
	};

	// struct KERNEL_STRUCTURE_VFS_USE {
	// 	void					(*block_read)( struct KERNEL_STRUCTURE_STORAGE *storage, uint64_t block, uint8_t *target, uint64_t limit );
	// 	void					(*block_write)( struct KERNEL_STRUCTURE_STORAGE *storage, uint64_t block, uint8_t *source, uint64_t limit );
	// 	struct KERNEL_STRUCTURE_VFS_SOCKET	(*file_open)( struct KERNEL_STRUCTURE_STORAGE *storage, uint8_t *path, uint64_t length );
	// 	void					(*file_close)( struct KERNEL_STRUCTURE_VFS_SOCKET *socket );
	// 	void					(*file_read)( struct KERNEL_STRUCTURE_VFS_SOCKET *socket, uint8_t *target, uint64_t seek, uint64_t limit );
	// };

	// pointer address to block content
	uintptr_t kernel_vfs_block_by_id( struct LIB_VFS_STRUCTURE *vfs, uint64_t b );
#endif
