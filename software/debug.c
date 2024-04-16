/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct STD_SYSCALL_STRUCTURE_MEMORY memory;

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	std_memory( (struct STD_SYSCALL_STRUCTURE_MEMORY *) &memory );
	printf( "%u, %u", memory.available >> STD_SHIFT_PAGE, memory.paging >> STD_SHIFT_PAGE );
	return 0;
}
