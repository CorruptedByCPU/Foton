/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_task( void ) {
	// reload CPU cycle counter in APIC controller
	kernel_lapic_reload();

	// accept current interrupt call
	kernel_lapic_accept();
}