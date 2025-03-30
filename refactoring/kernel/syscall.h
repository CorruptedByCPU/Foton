/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_SYSCALL
	#define	KERNEL_SYSCALL

	// shared exclusively for File Systems > dir (eg. kernel_vfs_dir)
	uintptr_t kernel_syscall_memory_alloc( uint64_t page );
#endif