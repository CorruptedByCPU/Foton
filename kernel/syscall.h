/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_SYSCALL
	#define	KERNEL_SYSCALL

	// fill in file properties
	void kernel_syscall_file( struct STD_STRUCTURE_FILE *file );

	// release socket id
	void kernel_syscall_file_close( uint64_t socket_id );

	// socket id
	uint64_t kernel_syscall_file_open( uint8_t *path, uint64_t limit );

	// transfer content of socket id
	void kernel_syscall_file_read( uint64_t socket_id, uint8_t *target, uint64_t seek, uint64_t limit );
#endif
