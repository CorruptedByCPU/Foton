/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_time_sleep( uint64_t u ) {
	// set release pointer
	uint64_t wait = kernel -> time_units + u;

	// release CPU time
	kernel -> apic_base_address -> tic = TRUE;
	while( wait > kernel -> time_units ) __asm__ volatile( "hlt" );
}
