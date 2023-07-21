/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_io_apic_connect( uint8_t irq, uint32_t io_apic_register ) {
	// lower part of register
	kernel -> io_apic_base_address -> ioregsel = (uint32_t) io_apic_register + KERNEL_IO_APIC_iowin_low;
	kernel -> io_apic_base_address -> iowin = (uint32_t) irq;

	// higher part of register
	kernel -> io_apic_base_address -> ioregsel = (uint32_t) io_apic_register + KERNEL_IO_APIC_iowin_high;
	kernel -> io_apic_base_address -> iowin = (uint32_t) EMPTY;
}