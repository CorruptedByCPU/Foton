/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

__attribute__(( preserve_most ))
void kernel_rtc( void ) {
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