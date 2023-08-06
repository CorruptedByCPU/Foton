/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_EXEC
	#define KERNEL_EXEC

	#define	KERNEL_EXEC_base_address	0x0000000000100000

	// resolves function addresses to ifself or remote libraries
	void kernel_exec_link( struct LIB_ELF_STRUCTURE_SECTION *elf_s, uint64_t elf_s_count, uintptr_t exec_base_address );
#endif
