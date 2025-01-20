/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t kernel_io_apic_line( uint8_t irq ) {
	// check if irq line is available for use
	return kernel -> io_apic_irq_lines & (1 << irq);
}

uint8_t kernel_io_apic_line_acquire( void ) {
	// lock access to lines
	MACRO_LOCK( kernel -> io_apic_semaphore );

	// check every line
	for( uint8_t i = 0; i < 24; i++ )
		// available?
		if( ! kernel_io_apic_line( i ) ) {
			// change line status
			kernel -> io_apic_irq_lines |= (1 << i);

			// unlock
			MACRO_UNLOCK( kernel -> io_apic_semaphore );

			// debug
			kernel -> log( (uint8_t *) "[IRQ line 0x%2X acquired.]\n", i );

			// acquired
			return i;
		}

	// unlock
	MACRO_UNLOCK( kernel -> io_apic_semaphore );

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
	kernel -> io_apic_irq_lines |= (1 << (line - KERNEL_IDT_IRQ_offset));

	// debug
	kernel -> log( (uint8_t *) "[I/O APIC] Line 0x%2X set.]\n", line - KERNEL_IDT_IRQ_offset );
}