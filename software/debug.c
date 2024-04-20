/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct STD_SYSCALL_STRUCTURE_MEMORY memory;

void thread( void ) {
	while( TRUE );
}

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	std_memory( (struct STD_SYSCALL_STRUCTURE_MEMORY *) &memory );
	printf( "%u, %u", memory.available >> STD_SHIFT_PAGE, memory.paging >> STD_SHIFT_PAGE );

	uint8_t debug[] = "debug thread";
	std_thread( (uintptr_t) &thread, (uint8_t *) &debug, sizeof( debug ) );

	while( TRUE ) {
		if( FALSE ) return 0;
	}
}
