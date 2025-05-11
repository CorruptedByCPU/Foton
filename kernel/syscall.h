/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_SYSCALL
	#define	KERNEL_SYSCALL

	// release socket id
	void kernel_syscall_file_close( uint64_t socket_id );

	// socket id
	uint64_t kernel_syscall_file_open( uint8_t *path, uint64_t limit );

	// transfer content of socket id
	void kernel_syscall_file_read( uint64_t socket_id, uint8_t *target, uint64_t seek, uint64_t limit );

	uintptr_t kernel_syscall_memory_alloc( uint64_t n );

	// 
	void kernel_syscall_memory_release( uintptr_t address, uint64_t n );
#endif
