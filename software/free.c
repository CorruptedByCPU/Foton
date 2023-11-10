/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// retrieve properties of system memory
	struct STD_SYSCALL_STRUCTURE_MEMORY memory;
	std_memory( (struct STD_SYSCALL_STRUCTURE_MEMORY *) &memory );

	printf( "available %u", memory.available );

	// exit
	return 0;
}
