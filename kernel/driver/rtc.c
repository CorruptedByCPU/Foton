/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

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
