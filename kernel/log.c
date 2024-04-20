/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_log( uint8_t *string, ... ) {
	// properties of argument list
	va_list argv;

	// start of argument list
	va_start( argv, string );

	// for every character from string
	uint64_t length = lib_string_length( string );
	for( uint64_t i = 0; i < length; i++ ) {
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
				driver_serial_value( value, 2, p_value, '0' );

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
				driver_serial_value( value, 10, p_value, ' ' );

				// next character from string
				continue;
			}

			case 's': {
				// retrieve substring
				uint8_t *substring = va_arg( argv, uint8_t * );
				
				// show 'substring' on terminal
				kernel -> log( substring );

				// next character from string
				continue;
			}

			case 'u': {
				// retrieve value
				uint64_t value = va_arg( argv, uint64_t );

				// show 'value' on terminal
				driver_serial_value( value, 10, p_value, ' ' );

				// next character from string
				continue;
			}

			case 'X': {
				// retrieve value
				uint64_t value = va_arg( argv, uint64_t );

				// show 'value' on terminal
				driver_serial_value( value, 16, p_value, '0' );

				// next character from string
				continue;
			}
		}
	}

	// end of arguemnt list
	va_end( argv );
}