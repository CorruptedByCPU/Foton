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
			#ifdef LIB_TERMINAL
				lib_terminal_char( (struct LIB_TERMINAL_STRUCTURE *) &kernel -> terminal, (uint8_t) string[ i ] );
			#else
				driver_serial_char( (uint8_t) string[ i ] );
			#endif

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
				#ifdef LIB_TERMINAL
					lib_terminal_char( (struct LIB_TERMINAL_STRUCTURE *) &kernel -> terminal, '%' );
				#else
					driver_serial_char( '%' );
				#endif

				// next character
				break;
			}

			case 'b': {
				// retrieve value
				uint64_t value = va_arg( argv, uint64_t );

				// show 'value' on terminal
				#ifdef LIB_TERMINAL
					lib_terminal_value( (struct LIB_TERMINAL_STRUCTURE *) &kernel -> terminal, value, 2, p_value, ' ' );
				#else
					driver_serial_value( value, 2, p_value, '0' );
				#endif

				// next character from string
				continue;
			}

			case 'c': {
				// retrieve character
				uint8_t c = va_arg( argv, uint64_t );
				
				// show 'character' on terminal
				#ifdef LIB_TERMINAL
					lib_terminal_char( (struct LIB_TERMINAL_STRUCTURE *) &kernel -> terminal, c );
				#else
					driver_serial_char( c );
				#endif

				// next character from string
				continue;
			}

			case 'd': {
				// retrieve value
				uint64_t value = va_arg( argv, uint64_t );

				// value signed?
				if( value & 0x8000000000000000 ) {
					// show 'character' on terminal
					#ifdef LIB_TERMINAL
						lib_terminal_char( (struct LIB_TERMINAL_STRUCTURE *) &kernel -> terminal, '-' );
					#else
						driver_serial_char( '-' );
					#endif

					// remove sign
					value = ~value + 1;
				}

				// show 'value' on terminal
				#ifdef LIB_TERMINAL
					lib_terminal_value( (struct LIB_TERMINAL_STRUCTURE *) &kernel -> terminal, value, 10, p_value, ' ' );
				#else
					driver_serial_value( value, 10, p_value, ' ' );
				#endif

				// next character from string
				continue;
			}

			case 's': {
				// retrieve substring
				uint8_t *substring = va_arg( argv, uint8_t * );
				
				// show 'substring' on terminal
				for( uint64_t j = 0; j < lib_string_length( substring ); j++ )
					#ifdef LIB_TERMINAL
						lib_terminal_string( (struct LIB_TERMINAL_STRUCTURE *) &kernel -> terminal, substring, lib_string_length( substring ) );
					#else
						driver_serial_char( substring[ j ] );
					#endif

				// next character from string
				continue;
			}

			case 'u': {
				// retrieve value
				uint64_t value = va_arg( argv, uint64_t );

				// show 'value' on terminal
				#ifdef LIB_TERMINAL
					lib_terminal_value( (struct LIB_TERMINAL_STRUCTURE *) &kernel -> terminal, value, 10, p_value, ' ' );
				#else
					driver_serial_value( value, 10, p_value, ' ' );
				#endif

				// next character from string
				continue;
			}

			case 'X': {
				// retrieve value
				uint64_t value = va_arg( argv, uint64_t );

				// show 'value' on terminal
				#ifdef LIB_TERMINAL
					lib_terminal_value( (struct LIB_TERMINAL_STRUCTURE *) &kernel -> terminal, value, 16, p_value, '0' );
				#else
					driver_serial_value( value, 16, p_value, '0' );
				#endif

				// next character from string
				continue;
			}
		}
	}

	// end of arguemnt list
	va_end( argv );
}