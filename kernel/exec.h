/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_EXEC
	#define KERNEL_EXEC

	#define	KERNEL_EXEC_base_address		0x0000000000100000

	struct KERNEL_STRUCTURE_EXEC {
		uint8_t					level;
		uint8_t					*path;
		uint64_t				limit;
		struct KERNEL_STRUCTURE_VFS_SOCKET	*socket;
		uintptr_t				workbench;
		struct KERNEL_STRUCTURE_TASK		*task;
		uint8_t					*stack;
		uint64_t				stack_byte;
		uint64_t				page;
		uintptr_t				base;
	};

	// process id
	uint64_t kernel_exec( uint8_t *name, uint64_t limit, uint8_t stream, uint8_t init );

	// static void kernel_exec_cancel( struct KERNEL_STRUCTURE_EXEC *exec );
#endif
