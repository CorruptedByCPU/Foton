/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t kernel_io_apic_available( uint8_t irq ) {
	// check if irq line is available for use
	return kernel -> io_apic_irq_lines & (1 << irq);
}

void kernel_io_apic_connect( uint8_t irq, uint32_t io_apic_register ) {
	// lower part of register
	kernel -> io_apic_base_address -> ioregsel = (uint32_t) io_apic_register + KERNEL_IO_APIC_iowin_low;
	kernel -> io_apic_base_address -> iowin = (uint32_t) irq;

	// higher part of register
	kernel -> io_apic_base_address -> ioregsel = (uint32_t) io_apic_register + KERNEL_IO_APIC_iowin_high;
	kernel -> io_apic_base_address -> iowin = (uint32_t) EMPTY;

	// lock used IRQ line
	kernel -> io_apic_irq_lines &= ~(1 << irq);
}