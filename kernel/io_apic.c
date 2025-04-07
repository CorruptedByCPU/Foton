/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_io_apic_attach( uint8_t line, uint32_t io_apic_register ) {
	// lower part of register
	kernel -> io_apic_base_address -> ioregsel = (uint32_t) io_apic_register + KERNEL_IO_APIC_iowin_low;
	kernel -> io_apic_base_address -> iowin = (uint32_t) line;

	// higher part of register
	kernel -> io_apic_base_address -> ioregsel = (uint32_t) io_apic_register + KERNEL_IO_APIC_iowin_high;
	kernel -> io_apic_base_address -> iowin = (uint32_t) EMPTY;
}
