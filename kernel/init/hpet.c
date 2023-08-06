/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_hpet( void ) {
	// HPET controler available?
	if( ! kernel -> hpet_base_address ) {
		// no, show proper information
		lib_terminal_printf( &kernel_terminal, (uint8_t *) "HPET controller not available. Operating system halted!\n" );

		// hold the door
		while( TRUE );
	}

	// retrieve amount of available timers
	uint8_t timer_count = (kernel -> hpet_base_address -> general_capabilities_and_id >> 8) & 0b00011111;

	// register available timers of HPET
	for( uint8_t i = 0; i < timer_count; i++ ) kernel -> hpet_timers |= 1 << i;

	// disable HPET controller and Legacy Replacement Route option.
	kernel -> hpet_base_address -> general_configuration = EMPTY;

	// reset counter value
	kernel -> hpet_base_address -> main_counter_value = EMPTY;

	// configure first capable timer as uptime counter
	for( uint8_t i = 0; i < timer_count; i++ ) {
		// properties of timer
		volatile struct KERNEL_HPET_STRUCTURE_TIMER *timer = (struct KERNEL_HPET_STRUCTURE_TIMER *) ((uintptr_t) kernel -> hpet_base_address + KERNEL_HPET_TIMER_offset + (sizeof( struct KERNEL_HPET_STRUCTURE_TIMER ) * i) );

		// retrieve IRQ lines capable by Timer
		volatile uint32_t irq = timer -> configuration_and_capabilities >> 32;

		// check capable IRQ lines of this Timer
		for( uint8_t j = 2; j < 24; j++ ) {
			// capable IRQ line for I/O APIC found?
			if( ((irq >> j) & 1) && kernel_io_apic_available( j ) ) {
				// show information about uptime Timer
				lib_terminal_printf( &kernel_terminal, (uint8_t *) "HPET: Timer %u selected for uptime, IRQ line 0x%2X.\n", i, j );
			
				// IRQ number of IDT and I/O APIC
				volatile uint64_t bits = (uint64_t) j << 9;

				// allow to set own periodic value
				bits |= (uint64_t) 1 << 6;

				// periodic type interrupts
				bits |= (uint64_t) 1 << 3;

				// enable interrupts
				bits |= (uint64_t) 1 << 2;

				// edge triggered interrupts
				bits |= (uint64_t) 0 << 1;

				// write configuration
				timer -> configuration_and_capabilities = bits;

				// interval every 1ms
				timer -> comparator = (uint64_t) 100000;

				// connect HPET controller Timer 0 to interrupt handler
				kernel_idt_mount( KERNEL_IDT_IRQ_offset + j, KERNEL_IDT_TYPE_irq, (uintptr_t) kernel_hpet_uptime_entry );

				// connect interrupt vector from IDT table in I/O APIC controller
				volatile uint32_t hpet_irq_io_apic_register = KERNEL_IO_APIC_iowin + (j * 0x02);
				kernel_io_apic_connect( KERNEL_IDT_IRQ_offset + j, hpet_irq_io_apic_register );

				// enable HPET controller
				kernel -> hpet_base_address -> general_configuration |= 1;

				// mark Timer as reserved
				kernel -> hpet_timers &= ~(1 << i);

				// found
				return;
			}
		}
	}

	// show proper message
	lib_terminal_printf( &kernel_terminal, (uint8_t *) "HPET: No capable timer found. Operating system halted.\n" );

	// hold the door
	while( TRUE );
}