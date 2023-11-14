/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_rtc( void ) {
	// state of real-time controller register
	uint8_t register_status = EMPTY;

	// controller is idle?
	do {
		// get state of register A
		driver_port_out_byte( KERNEL_RTC_PORT_command, KERNEL_RTC_STATUS_REGISTER_A );
		register_status = driver_port_in_byte( KERNEL_RTC_PORT_data );
	} while( register_status & KERNEL_RTC_STATUS_REGISTER_A_update_in_progress );

	// put controller into modification mode of register A
	driver_port_out_byte( KERNEL_RTC_PORT_command, KERNEL_RTC_STATUS_REGISTER_A | 0x80 );

	// set calling frequency to 1024 Hz
	driver_port_out_byte( KERNEL_RTC_PORT_command, KERNEL_RTC_STATUS_REGISTER_A );
	driver_port_out_byte( KERNEL_RTC_PORT_data, KERNEL_RTC_STATUS_REGISTER_A_divider | KERNEL_RTC_STATUS_REGISTER_A_rate );

	// put controller into modification mode of register B
	driver_port_out_byte( KERNEL_RTC_PORT_command, KERNEL_RTC_STATUS_REGISTER_B | 0x80 );

	// get current state of registry flags
	register_status = driver_port_in_byte( KERNEL_RTC_PORT_data );

	// set registry flags
	register_status |= KERNEL_RTC_STATUS_REGISTER_B_24_hour_mode;
	register_status |= KERNEL_RTC_STATUS_REGISTER_B_data_mode_binary;
	register_status |= KERNEL_RTC_STATUS_REGISTER_B_periodic_interrupt;
	register_status &= ~KERNEL_RTC_STATUS_REGISTER_B_update_ended_interrupt;
	register_status &= ~KERNEL_RTC_STATUS_REGISTER_B_alarm_interrupt;
	driver_port_out_byte( KERNEL_RTC_PORT_data, register_status );

	// receive pending interrupt of real-time controller
	driver_port_out_byte( KERNEL_RTC_PORT_command, KERNEL_RTC_STATUS_REGISTER_C );
	driver_port_in_byte( KERNEL_RTC_PORT_data );

	// connect real-time controller interrupt handler
	kernel_idt_mount( KERNEL_IDT_IRQ_offset + KERNEL_RTC_IRQ_number, KERNEL_IDT_TYPE_gate_interrupt, (uintptr_t) kernel_rtc_entry );

	// connect interrupt vector from IDT table in IOAPIC controller
	kernel_io_apic_connect( KERNEL_IDT_IRQ_offset + KERNEL_RTC_IRQ_number, KERNEL_RTC_IO_APIC_register );

	// mark IRQ line as used
	kernel -> io_apic_irq_lines &= ~(1 << KERNEL_RTC_IRQ_number);
}