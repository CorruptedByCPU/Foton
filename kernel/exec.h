/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_EXEC
	#define KERNEL_EXEC

	#define	KERNEL_EXEC_base_address	0x0000000000100000

	// returns new process ID
	int64_t kernel_exec( uint8_t *name, uint64_t length, uint8_t stream_flow );
#endif
