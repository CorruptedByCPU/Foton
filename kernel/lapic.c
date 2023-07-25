/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

__attribute__ (( preserve_all ))
void kernel_lapic_accept( void ) {
	// about handling current hardware interrupt, properly
	kernel -> lapic_base_address -> eoi = EMPTY;
}

uint8_t kernel_lapic_id() {
	// return BSP/logical CPU ID
	return kernel -> lapic_base_address -> id >> 24;
}

void kernel_lapic_reload() {
	// to call internal clock interrupt in about N cycles
	kernel -> lapic_base_address -> tic = KERNEL_LAPIC_Hz;
}