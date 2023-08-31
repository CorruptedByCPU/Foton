/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

__attribute__(( preserve_all ))
void kernel_hpet_uptime( void ) {
	// increase the real-time controller invocation count
	kernel -> hpet_miliseconds++;

	// accept current interrupt call
	kernel_lapic_accept();
}

void kernel_hpet_sleep( uint64_t t ) {
	// set release pointer
	uint64_t stop = kernel -> hpet_miliseconds + t;

	// wait until we achieve
	while( stop > kernel -> hpet_miliseconds );
}