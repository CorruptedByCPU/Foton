/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_time_sleep( uint64_t t ) {
	// set release pointer
	uint64_t stop = kernel -> time_unit + t;

	// wait until we achieve
	while( stop > kernel -> time_unit ) __asm__ volatile( "int $0x20" );	// release unused AP time
}