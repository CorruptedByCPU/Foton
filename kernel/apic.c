/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_apic_accept( void ) {
	// about handling current hardware interrupt, properly
	kernel -> apic_base_address -> eoi = EMPTY;
}

uint8_t kernel_apic_id( void ) {
	// return BS/A id
	return kernel -> apic_base_address -> id >> 24;
}

void kernel_apic_reload( void ) {
	// to call internal clock interrupt in about N cycles
	kernel -> apic_base_address -> tic = KERNEL_APIC_Hz;
}
