/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

__attribute__(( no_caller_saved_registers ))
void kernel_hpet_uptime( void ) {
	// increase the real-time controller invocation count
	kernel -> time_unit++;

	// accept current interrupt call
	kernel_lapic_accept();
}

void kernel_hpet_sleep( uint64_t t ) {
	// set release pointer
	uint64_t stop = kernel -> time_unit++ + t;

	// wait until we achieve
	while( stop > kernel -> time_unit++ );
}