/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t console_vt100( uint8_t *string, uint64_t length ) {
	// parsed sequence length
	uint8_t sequence_length = 0;

	// control code?
	if( *(string++) != STD_ASCII_ESC ) return 0;	// no

	// escape sequence?
	if( *(string++) != STD_ASCII_BRACKET_SQUARE_OPEN ) return 0;	// no

	// start of sequence
	sequence_length += 2;

	// choose type of action
	switch( lib_string_to_integer( string, 10 ) ) {
		// reset to default?
		case 0: {
			// move string index to next option
			string++;

			// reset properties of terminal?
			if( *string != 'm' ) return 0;	// no
		
			// set default foreground/background color of terminal
			console_terminal.color_foreground = lib_color( 255 );
			console_terminal.color_background = lib_color( 232 );

			// update sequence length
			sequence_length += 1 + 1;

			// done
			break;
		}

		// clear and move cursor at beginning?
		case 2: {
			// move string index to next option
			string++;

			// clear

			// whole screen
			if( *string == 'J' ) lib_terminal_drain( (struct LIB_TERMINAL_STRUCTURE *) &console_terminal );

			// current line
			// if( *string == 'K' ) lib_terminal_drain_line( (struct LIB_TERMINAL_STRUCTURE *) &console_terminal );

			// update sequence length
			sequence_length += 1 + 1;	// type + option

			// done
			break;
		}

		// change foreground color?
		case 38: {
			// move string index to next option
			string += 2 + 1;	// leave separator

			// choose color from predefinied palette?
			if( lib_string_to_integer( string, 10 ) == 5 ) {
				// update sequence length
				sequence_length += 1 + 1;	// leave separator

				// move string index to requested color
				string += 1 + 1;	// leave separator

				// set selected foreground color
				console_terminal.color_foreground = lib_color( lib_string_to_integer( string, 10 ) );

				// sequence length
				sequence_length += lib_string_length_scope_digit( string ) + 1;
			}

			// update sequence length
			sequence_length += 2 + 1;

			// done
			break;
		}

		// change background color?
		case 48: {
			// move string index to next option
			string += 2 + 1;	// leave separator

			// choose color from predefinied palette?
			if( lib_string_to_integer( string, 10 ) == 5 ) {
				// update sequence length
				sequence_length += 1 + 1;	// leave separator

				// move string index to requested color
				string += 1 + 1;	// leave separator

				// set selected foreground color
				console_terminal.color_background = lib_color( lib_string_to_integer( string, 10 ) );

				// sequence length
				sequence_length += lib_string_length_scope_digit( string ) + 1;
			}

			// update sequence length
			sequence_length += 2 + 1;

			// done
			break;
		}
	}

	// return parsed sequence length
	return sequence_length;
}
