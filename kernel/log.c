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
	for( uint64_t i = INIT; i < length; i++ ) {
		// special character?
		if( string[ i ] != '%' ) {
			// no, show it
			lib_terminal_char( (struct LIB_TERMINAL_STRUCTURE *) &kernel -> terminal, (uint8_t) string[ i ] );

			// next character
			continue;
		}

		// prefix before type?
		uint64_t prefix = lib_string_length_scope_digit( (uint8_t *) &string[ ++i ] );
		uint64_t p_value = lib_string_to_integer( (uint8_t *) &string[ i ], 10 );

		// omit prefix value if existed
		i += prefix;

		// definied prefix length?
		if( string[ i ] == '*' ) {
			// amount of digits after digit delimiter
			p_value = va_arg( argv, uint64_t );

			// leave predefinied prefix
			i += 1;
		}

		// definied suffix length?
		uint64_t pre_suffix = EMPTY;
		if( string[ i ] == '.' && string[ i + 1 ] == '*' ) {
			// amount of digits after digit delimiter
			pre_suffix = va_arg( argv, uint64_t );

			// leave predefinied suffix
			i += 2;
		}

		// check sequence type
		switch( string[ i ] ) {
			case '%': {
				// just show '%' character
				lib_terminal_char( (struct LIB_TERMINAL_STRUCTURE *) &kernel -> terminal, '%' );

				// next character
				break;
			}

			case 'b': {
				// retrieve value
				uint64_t value = va_arg( argv, uint64_t );

				// show 'value' on terminal
				lib_terminal_value( (struct LIB_TERMINAL_STRUCTURE *) &kernel -> terminal, value, 2, p_value, STD_ASCII_DIGIT_0 );

				// next character from string
				continue;
			}

			case 'c': {
				// retrieve character
				uint8_t c = va_arg( argv, uint64_t );
				
				// show 'character' on terminal
				lib_terminal_char( (struct LIB_TERMINAL_STRUCTURE *) &kernel -> terminal, c );

				// next character from string
				continue;
			}

			case 'd': {
				// retrieve value
				uint64_t value = va_arg( argv, uint64_t );

				// value signed?
				if( value & 0x8000000000000000 ) {
					// show 'character' on terminal
					lib_terminal_char( (struct LIB_TERMINAL_STRUCTURE *) &kernel -> terminal, '-' );

					// remove sign
					value = ~value + 1;
				}

				// show 'value' on terminal
				lib_terminal_value( (struct LIB_TERMINAL_STRUCTURE *) &kernel -> terminal, value, 10, p_value, STD_ASCII_DIGIT_0 );

				// next character from string
				continue;
			}

			case 's': {
				// retrieve substring
				uint8_t *substring = va_arg( argv, uint8_t * );

				uint64_t limit = lib_string_length( substring );

				// change string length if predefinied value exist
				if( pre_suffix ) {
					// new length
					limit = pre_suffix;
				}

				// show 'substring' on terminal
				lib_terminal_string( (struct LIB_TERMINAL_STRUCTURE *) &kernel -> terminal, substring, lib_string_length( substring ) );

				// next character from string
				continue;
			}

			case 'u': {
				// retrieve value
				uint64_t value = va_arg( argv, uint64_t );

				// show 'value' on terminal
				lib_terminal_value( (struct LIB_TERMINAL_STRUCTURE *) &kernel -> terminal, value, 10, p_value, STD_ASCII_DIGIT_0 );

				// next character from string
				continue;
			}

			case 'p': {
				// retrieve value
				uint64_t value = va_arg( argv, uint64_t );

				// show 'value' on terminal
				lib_terminal_string( (struct LIB_TERMINAL_STRUCTURE *) &kernel -> terminal, (uint8_t *) "0x", 2 );
				lib_terminal_value( (struct LIB_TERMINAL_STRUCTURE *) &kernel -> terminal, value, 16, 16, STD_ASCII_DIGIT_0 );

				// next character from string
				continue;
			}

			case 'X': {
				// retrieve value
				uint64_t value = va_arg( argv, uint64_t );

				// show 'value' on terminal
				lib_terminal_value( (struct LIB_TERMINAL_STRUCTURE *) &kernel -> terminal, value, 16, p_value, STD_ASCII_DIGIT_0 );

				// next character from string
				continue;
			}
		}
	}

	// end of arguemnt list
	va_end( argv );
}
