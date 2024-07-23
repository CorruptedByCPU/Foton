/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_rtc( void ) {
	// connect real-time controller interrupt handler
	kernel_idt_mount( KERNEL_IDT_IRQ_offset + DRIVER_RTC_IRQ_number, KERNEL_IDT_TYPE_gate_interrupt, (uintptr_t) driver_rtc_entry );

	// connect interrupt vector from IDT table in IOAPIC controller
	kernel_io_apic_connect( KERNEL_IDT_IRQ_offset + DRIVER_RTC_IRQ_number, DRIVER_RTC_IO_APIC_register );

	// state of real-time controller register
	uint8_t local_register_status = EMPTY;

	//----------------------------------------------------------------------
	// set RTC interrupt rate at 1024 Hz (even if set by default)
	//----------------------------------------------------------------------

	// controller is idle?
	do {
		// get state of register A
		driver_port_out_byte( DRIVER_RTC_PORT_command, DRIVER_RTC_STATUS_REGISTER_A );
		local_register_status = driver_port_in_byte( DRIVER_RTC_PORT_data );
	} while( local_register_status & DRIVER_RTC_STATUS_REGISTER_A_update_in_progress );

	// put controller into modification mode of register A
	driver_port_out_byte( DRIVER_RTC_PORT_command, DRIVER_RTC_STATUS_REGISTER_A | 0x80 );

	// set calling frequency to 1024 Hz
	driver_port_out_byte( DRIVER_RTC_PORT_command, DRIVER_RTC_STATUS_REGISTER_A );
	driver_port_out_byte( DRIVER_RTC_PORT_data, (local_register_status & 0xF0) | DRIVER_RTC_STATUS_REGISTER_A_rate );

	//----------------------------------------------------------------------
	// turn on interrupts and change clock range to 24h instead of 12h
	//----------------------------------------------------------------------

	// controller is idle?
	do {
		// get state of register B
		driver_port_out_byte( DRIVER_RTC_PORT_command, DRIVER_RTC_STATUS_REGISTER_B );
		local_register_status = driver_port_in_byte( DRIVER_RTC_PORT_data );
	} while( local_register_status & DRIVER_RTC_STATUS_REGISTER_B_update_in_progress );

	// put controller into modification mode of register B
	driver_port_out_byte( DRIVER_RTC_PORT_command, DRIVER_RTC_STATUS_REGISTER_B | 0x80 );

	// set registry flags
	local_register_status |= DRIVER_RTC_STATUS_REGISTER_B_24_hour_mode;
	local_register_status |= DRIVER_RTC_STATUS_REGISTER_B_periodic_interrupt;
	driver_port_out_byte( DRIVER_RTC_PORT_data, local_register_status );

	//----------------------------------------------------------------------
	// remove overdue interrupts
	//----------------------------------------------------------------------

	// retrieve pending interrupt of real-time controller
	driver_port_out_byte( DRIVER_RTC_PORT_command, DRIVER_RTC_STATUS_REGISTER_C );
	driver_port_in_byte( DRIVER_RTC_PORT_data );
}