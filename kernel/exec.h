/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_EXEC
	#define KERNEL_EXEC

	#define	KERNEL_EXEC_base_address		0x0000000000100000

	struct KERNEL_EXEC_STRUCTURE_INIT {
		uint8_t					level;
		struct KERNEL_VFS_STRUCTURE		*socket;
		struct KERNEL_VFS_STRUCTURE_PROPERTIES	properties;
		struct KERNEL_TASK_STRUCTURE		*task;
		uintptr_t				workbench_address;
		uintptr_t				base_address;
		uint64_t				page;
		uint8_t					*stack;
		uint64_t				stack_byte;
	};

	// returns new process ID or error
	int64_t kernel_exec( uint8_t *name, uint64_t length, uint8_t stream_flow );
#endif
