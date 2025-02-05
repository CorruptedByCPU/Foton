/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions of driver
	//----------------------------------------------------------------------
	#ifndef	DRIVER_SERIAL
		#include	"./serial.h"
	#endif

void driver_serial_init( void ) {
	// disable interrupt generation
	driver_port_out_byte( DRIVER_SERIAL_PORT_COM1 + DRIVER_SERIAL_REGISTER_interrupt_enable_or_divisor_high, EMPTY );

	// enable DLAB (frequency divider)
	driver_port_out_byte( DRIVER_SERIAL_PORT_COM1 + DRIVER_SERIAL_REGISTER_line_control_or_dlab, 0x80 );

	// communication frequency: 38400
	driver_port_out_byte( DRIVER_SERIAL_PORT_COM1 + DRIVER_SERIAL_REGISTER_data_or_divisor_low, 0x03 );
	driver_port_out_byte( DRIVER_SERIAL_PORT_COM1 + DRIVER_SERIAL_REGISTER_interrupt_enable_or_divisor_high, 0x00 );

	// 8 bits per sign, no parity, 1 stop bit
	driver_port_out_byte( DRIVER_SERIAL_PORT_COM1 + DRIVER_SERIAL_REGISTER_line_control_or_dlab, 0x03 );

	// enable FIFO, clear with 14 byte threshold
	driver_port_out_byte( DRIVER_SERIAL_PORT_COM1 + DRIVER_SERIAL_REGISTER_interrupt_identification_or_fifo, 0xC7 );

	// not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled
	driver_port_out_byte( DRIVER_SERIAL_PORT_COM1 + DRIVER_SERIAL_REGISTER_modem_control, 0x0F );
}

void driver_serial_char( uint8_t character ) {
	// controller is busy?
	driver_serial_pool();

	// send character
	driver_port_out_byte( DRIVER_SERIAL_PORT_COM1 + DRIVER_SERIAL_REGISTER_data_or_divisor_low, character );
}

void driver_serial_pool( void ) {
	// controller is busy?
	while( !( driver_port_in_byte( DRIVER_SERIAL_PORT_COM1 + DRIVER_SERIAL_REGISTER_line_status ) & 0b00100000 ) );
}

uint8_t driver_serial_value( uint64_t value, uint8_t base, uint8_t prefix, uint8_t character ) {
	// if base is out of range
	if( base < 2 || base > 36 ) return EMPTY;	// cancel

	// decoded value
	uint8_t i = 0;
	uint8_t array[ 64 ] = { character };	// string for 8 byte value

	// convert value to single digits
	while( value ) {
		// first digit of value
		uint8_t digit = value % base;

		// convert digit to ASCII code
		if( digit < 10 ) array[ i++ ] = digit + STD_ASCII_DIGIT_0;
		else array[ i++ ] = digit + STD_ASCII_DIGIT_7;

		// remove digit from value
		value /= base;
	};

	// complete value with ZERO digits as length of prefix
	if( prefix > i ) for( ; i < prefix; i++ ) array[ i ] = STD_ASCII_DIGIT_0;

	// preserve amount of digits
	uint8_t digits = i;

	// display all digits
	if( i )	do { driver_serial_char( array[ --i ] ); }
		while( i );
	// or ZERO in absence of
	else { driver_serial_char( STD_ASCII_DIGIT_0 ); digits = 1; }

	// return amount of digits
	return digits;
}
