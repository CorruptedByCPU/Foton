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