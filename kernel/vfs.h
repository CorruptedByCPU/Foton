/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_VFS
	#define	KERNEL_VFS

	#ifndef	LIBRARY_VFS
		#include	"../library/vfs.h"
	#endif

	#define	KERNEL_VFS_limit		(STD_PAGE_byte / sizeof( struct KERNEL_STRUCTURE_VFS ))

	struct	KERNEL_STRUCTURE_VFS {
		uint64_t			storage;
		uint64_t			knot;
		int64_t				pid;
	};
#endif
