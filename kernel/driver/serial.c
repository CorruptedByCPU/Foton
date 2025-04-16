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
	while( !( driver_port_in_byte( DRIVER_SERIAL_PORT_COM1 + DRIVER_SERIAL_REGISTER_line_status ) & 0x20 ) );
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

void driver_serial( uint8_t *string, ... ) {
	#ifndef	RELEASE
	// properties of argument list
	va_list argv;

	// start of argument list
	va_start( argv, string );

	// for every character from string
	uint64_t limit = lib_string_length( string );
	for( uint64_t i = 0; i < limit; i++ ) {
		// special character?
		if( string[ i ] != '%' ) {
			// no, show it
			driver_serial_char( (uint8_t) string[ i ] );

			// next character
			continue;
		}

		// prefix before type?
		uint64_t prefix = lib_string_length_scope_digit( (uint8_t *) &string[ ++i ] );
		uint64_t p_value = lib_string_to_integer( (uint8_t *) &string[ i ], 10 );

		// omit prefix value if existed
		i += prefix;

		// check sequence type
		switch( string[ i ] ) {
			case '%': {
				// just show '%' character
				driver_serial_char( '%' );

				// next character
				break;
			}

			case 'b': {
				// retrieve value
				uint64_t value = va_arg( argv, uint64_t );

				// show 'value' on terminal
				driver_serial_value( value, 2, p_value, STD_ASCII_DIGIT_0 );

				// next character from string
				continue;
			}

			case 'c': {
				// retrieve character
				uint8_t c = va_arg( argv, uint64_t );
				
				// show 'character' on terminal
				driver_serial_char( c );

				// next character from string
				continue;
			}

			case 'd': {
				// retrieve value
				uint64_t value = va_arg( argv, uint64_t );

				// value signed?
				if( value & 0x8000000000000000 ) {
					// show 'character' on terminal
					driver_serial_char( '-' );

					// remove sign
					value = ~value + 1;
				}

				// show 'value' on terminal
				driver_serial_value( value, 10, p_value, STD_ASCII_DIGIT_0 );

				// next character from string
				continue;
			}

			case 's': {
				// retrieve substring
				uint8_t *substring = va_arg( argv, uint8_t * );
				
				// show 'substring' on terminal
				for( uint64_t j = 0; j < lib_string_length( substring ); j++ )
					driver_serial_char( substring[ j ] );

				// next character from string
				continue;
			}

			case 'u': {
				// retrieve value
				uint64_t value = va_arg( argv, uint64_t );

				// show 'value' on terminal
				driver_serial_value( value, 10, p_value, STD_ASCII_DIGIT_0 );

				// next character from string
				continue;
			}

			case 'X': {
				// retrieve value
				uint64_t value = va_arg( argv, uint64_t );

				// show 'value' on terminal
				driver_serial_value( value, 16, p_value, STD_ASCII_DIGIT_0 );

				// next character from string
				continue;
			}
		}
	}

	// end of arguemnt list
	va_end( argv );
	#endif
}
