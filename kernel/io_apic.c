/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t kernel_io_apic_line( uint8_t irq ) {
	// check if irq line is available for use
	return kernel -> io_apic_irq_lines & (1 << irq);
}

uint8_t kernel_io_apic_line_acquire( void ) {
	// lock access to lines
	while( __sync_val_compare_and_swap( &kernel -> io_apic_semaphore, UNLOCK, LOCK ) );

	// check every line
	for( uint8_t i = 0; i < 24; i++ )
		// available?
		if( kernel_io_apic_line( i ) ) {
			// change line status
			kernel -> io_apic_irq_lines &= ~(1 << i);

			// unlock
			kernel -> io_apic_semaphore = UNLOCK;

			// acquired
			return i;
		}

	// unlock
	kernel -> io_apic_semaphore = UNLOCK;

	// unavailable
	return EMPTY;
}

void kernel_io_apic_connect( uint8_t line, uint32_t io_apic_register ) {
	// lower part of register
	kernel -> io_apic_base_address -> ioregsel = (uint32_t) io_apic_register + KERNEL_IO_APIC_iowin_low;
	kernel -> io_apic_base_address -> iowin = (uint32_t) line;

	// higher part of register
	kernel -> io_apic_base_address -> ioregsel = (uint32_t) io_apic_register + KERNEL_IO_APIC_iowin_high;
	kernel -> io_apic_base_address -> iowin = (uint32_t) EMPTY;

	// lock used IRQ line
	kernel -> io_apic_irq_lines &= ~(1 << line);
}