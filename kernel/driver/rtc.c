/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

__attribute__(( preserve_most ))
void driver_rtc( void ) {
	// receive pending interrupt of the real-time controller
	while( TRUE ) {
		driver_port_out_byte( DRIVER_RTC_PORT_command, DRIVER_RTC_STATUS_REGISTER_C );
		uint8_t C = driver_port_in_byte( DRIVER_RTC_PORT_data );

		// no more interrupts?
		if( ! C ) break;	// yes

		// periodic interrupt?
		if( C & DRIVER_RTC_STATUS_REGISTER_C_interrupt_periodic )
			// increase the real-time controller invocation count
			kernel -> time_rtc++;
	}

	// accept current interrupt call
	kernel_lapic_accept();
}

void driver_rtc_init( void ) {
	// connect real-time controller interrupt handler
	kernel_idt_mount( KERNEL_IDT_IRQ_offset + DRIVER_RTC_IRQ_number, KERNEL_IDT_TYPE_gate_interrupt, (uintptr_t) driver_rtc_entry );

	// connect interrupt vector from IDT table in IOAPIC controller
	kernel_io_apic_connect( KERNEL_IDT_IRQ_offset + DRIVER_RTC_IRQ_number, DRIVER_RTC_IO_APIC_register );

	// debug
	kernel -> log( (uint8_t *) "[RTC] IRQ 0x%2X, connected.\n", DRIVER_RTC_IRQ_number );

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

uint64_t driver_rtc_time( void ) {
	// select register
	driver_port_out_byte( DRIVER_RTC_PORT_command, DRIVER_RTC_STATUS_REGISTER_B );

	// get current state of registry flags
	uint8_t register_status = driver_port_in_byte( DRIVER_RTC_PORT_data );

	// check if RTC controller operating in Binary mode
	uint8_t mode = ! ( register_status & DRIVER_RTC_STATUS_REGISTER_B_data_mode_binary );

	// retrieve weekday
	driver_port_out_byte( DRIVER_RTC_PORT_command, DRIVER_RTC_REGISTER_weekday );
	uint64_t current = driver_port_in_byte( DRIVER_RTC_PORT_data );

	// retrieve year
	driver_port_out_byte( DRIVER_RTC_PORT_command, DRIVER_RTC_REGISTER_year );
	current = (current << STD_SHIFT_256) | driver_rtc_register( mode );

	// retrieve month
	driver_port_out_byte( DRIVER_RTC_PORT_command, DRIVER_RTC_REGISTER_month );
	current = (current << STD_SHIFT_256) | driver_rtc_register( mode );

	// retrieve day
	driver_port_out_byte( DRIVER_RTC_PORT_command, DRIVER_RTC_REGISTER_day_of_month );
	current = (current << STD_SHIFT_256) | driver_rtc_register( mode );

	// retrieve hour
	driver_port_out_byte( DRIVER_RTC_PORT_command, DRIVER_RTC_REGISTER_hour );
	current = (current << STD_SHIFT_256) | driver_rtc_register( mode );

	// retrieve minutes
	driver_port_out_byte( DRIVER_RTC_PORT_command, DRIVER_RTC_REGISTER_minutes );
	current = (current << STD_SHIFT_256) | driver_rtc_register( mode );

	// retrieve seconds
	driver_port_out_byte( DRIVER_RTC_PORT_command, DRIVER_RTC_REGISTER_seconds );
	current = (current << STD_SHIFT_256) | driver_rtc_register( mode );

	// return current time and date
	return current;
}

uint8_t driver_rtc_register( uint8_t mode ) {
	// read value from RTC
	uint8_t value = driver_port_in_byte( DRIVER_RTC_PORT_data );

	// convert value from BCD to Binary if needed
	return mode ? (value >> 4) * 10 + (value & 0x0F) : value;
}