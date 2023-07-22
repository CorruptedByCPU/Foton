/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

__attribute__ (( preserve_all ))
void kernel_lapic_accept( void ) {
	// about handling current hardware interrupt, properly
	kernel -> lapic_base_address -> eoi = EMPTY;
}