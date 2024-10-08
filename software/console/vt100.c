/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t console_vt100( uint8_t *string, uint64_t length ) {
	// start of sequence?
	if( *(string++) != '\e' ) return EMPTY;	// no

	// select sequence type
	switch( *string ) {
		// CSI
		case '[': {
			// move pointer into sequence
			string++;

			// we support at least 8 arguments
			uint8_t arg_length[ 8 ] = { EMPTY };
			uint8_t arg_value[ 8 ] = { EMPTY };

			// retrieve all arguments from sequence
			for( uint8_t i = 0; i < 8; i++ ) {
				// argument
				arg_length[ i ]	= lib_string_length_scope_digit( string );
				arg_value[ i ] = lib_string_to_integer( string, 10 );

				// end of argument list?
				if( ! arg_length[ i ] ) break;	// yes

				// move pointer over argument
				string += arg_length[ i ];

				// next argument exist?
				if( *string != STD_ASCII_SEMICOLON ) break;	// no

				// move pointer to next argument
				string++;
			}

			// choose sequence type
			switch( *string ) {
				// move cursor up
				case 'A': {
					// if steps are not specified
					if( ! arg_value[ 0 ] ) { arg_value[ 0 ] = 1; arg_length[ 0 ] = 0; }	// by default 1 step

					// move cursor N steps up, if possible
					if( console_terminal -> cursor_y < arg_value[ 0 ] ) console_terminal -> cursor_y = 0;
					else console_terminal -> cursor_y -= arg_value[ 0 ];
					
					// update cursor position inside terminal
					lib_terminal_cursor_set( console_terminal );

					// return sequence length
					return 2 + arg_length[ 0 ] + 1;
				}

				// move cursor down
				case 'B': {
					// if steps are not specified
					if( ! arg_value[ 0 ] ) { arg_value[ 0 ] = 1; arg_length[ 0 ] = 0; }	// by default 1 step

					// cursor behind text area?
					if( console_terminal -> cursor_y + arg_value[ 0 ] < console_terminal -> height_char ) console_terminal -> cursor_y += arg_value[ 0 ];
					else	// leave it at last position
						console_terminal -> cursor_y = console_terminal -> height_char - 1;
					
					// update cursor position inside terminal
					lib_terminal_cursor_set( console_terminal );

					// return sequence length
					return 2 + arg_length[ 0 ] + 1;
				}

				// move cursor forward
				case 'C': {
					// if steps are not specified
					if( ! arg_value[ 0 ] ) { arg_value[ 0 ] = 1; arg_length[ 0 ] = 0; }	// by default 1 step

					// cursor behind text area?
					if( console_terminal -> cursor_x + arg_value[ 0 ] < console_terminal -> width_char )
						// move cursor N steps forward
						console_terminal -> cursor_x += arg_value[ 0 ];
					else {
						// cursor behind text area?
						if( console_terminal -> cursor_y + 1 < console_terminal -> height_char ) {
							// move cursor one line below
							console_terminal -> cursor_y++;

							// and at right position
							console_terminal -> cursor_x += arg_value[ 0 ];
							console_terminal -> cursor_x -= console_terminal -> width_char;
						} else
							// leave cursor at end of current line
							console_terminal -> cursor_x = console_terminal -> width_char - 1;
					}
					
					// update cursor position inside terminal
					lib_terminal_cursor_set( console_terminal );

					// return sequence length
					return 2 + arg_length[ 0 ] + 1;
				}

				// move cursor backward
				case 'D': {
					// if steps are not specified
					if( ! arg_value[ 0 ] ) { arg_value[ 0 ] = 1; arg_length[ 0 ] = 0; }	// by default 1 step

					// move cursor N steps back, if possible
					if( console_terminal -> cursor_x >= arg_value[ 0 ] ) console_terminal -> cursor_x -= arg_value[ 0 ];
					else {
						// can we move one line up?
						if( console_terminal -> cursor_y ) {
							// move cursor one line up
							console_terminal -> cursor_y--;

							// and at the end of that line
							console_terminal -> cursor_x = console_terminal -> width_char - 1;
						} else
							// leave cursor at beginning of current line
							console_terminal -> cursor_x = 0;
					}

					// update cursor position inside terminal
					lib_terminal_cursor_set( console_terminal );

					// return sequence length
					return 2 + arg_length[ 0 ] + 1;
				}

				// move cursor at beginning of next (N th) line
				case 'E': {
					// if steps are not specified
					if( ! arg_value[ 0 ] ) { arg_value[ 0 ] = 1; arg_length[ 0 ] = 0; }	// by default 1 step

					// move cursor N lines forward, if possible
					if( console_terminal -> cursor_y + arg_value[ 0 ] <= console_terminal -> height_char ) console_terminal -> cursor_y += arg_value[ 0 ];
					else console_terminal -> cursor_y = console_terminal -> height_char;

					// and at beggining of that line
					console_terminal -> cursor_x = 0;

					// update cursor position inside terminal
					lib_terminal_cursor_set( console_terminal );

					// return sequence length
					return 2 + arg_length[ 0 ] + 1;
				}

				// move cursor at beginning of previous (N th) line
				case 'F': {
					// if steps are not specified
					if( ! arg_value[ 0 ] ) { arg_value[ 0 ] = 1; arg_length[ 0 ] = 0; }	// by default 1 step

					// move cursor N lines backward, if possible
					if( console_terminal -> cursor_y - arg_value[ 0 ] >= 0 ) console_terminal -> cursor_y -= arg_value[ 0 ];
					else console_terminal -> cursor_y = 0;

					// and at beggining of that line
					console_terminal -> cursor_x = 0;

					// update cursor position inside terminal
					lib_terminal_cursor_set( console_terminal );

					// return sequence length
					return 2 + arg_length[ 0 ] + 1;
				}

				// move cursor to selected column
				case 'G': {
					// move cursor to selected column
					console_terminal -> cursor_x = arg_value[ 0 ];

					// cursor behind text area?
					if( console_terminal -> width_char < console_terminal -> cursor_x )
						// leave it at last position
						console_terminal -> cursor_x = console_terminal -> width_char - 1;
					
					// update cursor position inside terminal
					lib_terminal_cursor_set( console_terminal );

					// return sequence length
					return 2 + arg_length[ 0 ] + 1;
				}

				// set cursor at
				case 'H': {
					// set new cursor position
					console_terminal -> cursor_x = arg_value[ 0 ];
					console_terminal -> cursor_y = arg_value[ 1 ];

					// cursor behind text area?
					if( console_terminal -> width_char < console_terminal -> cursor_x )
						// leave it at last position
						console_terminal -> cursor_x = console_terminal -> width_char - 1;
					
					// cursor behind text area?
					if( console_terminal -> height_char < console_terminal -> cursor_y )
						// leave it at last position
						console_terminal -> cursor_y = console_terminal -> height_char - 1;
						
					// update cursor position inside terminal
					lib_terminal_cursor_set( console_terminal );

					// return sequence length
					return 2 + arg_length[ 0 ] + 1 + arg_length[ 1 ] + 1;
				}

				// clear screen or part of it
				case 'J': {
					// select behavior
					switch( arg_value[ 0 ] ) {
						case 2: {
							// clear whole screen and move cursor at beginning
							lib_terminal_drain( console_terminal );

							// return sequence length
							return 2 + arg_length[ 0 ] + 1;
						}
					}
				}

				// clear current line or part of it
				case 'K': {
					// select behavior
					switch( arg_value[ 0 ] ) {
						case 2: {
							// clear whole line
							lib_terminal_drain_line( console_terminal );

							// return sequence length
							return 2 + arg_length[ 0 ] + 1;
						}
					}
				}

				// disable cursor
				case 'N': {
					// disable cursor
					lib_terminal_cursor_disable( console_terminal );

					// return sequence length
					return 2 + 1;
				}

				// enable cursor
				case 'O': {
					// disable cursor
					lib_terminal_cursor_enable( console_terminal );

					// return sequence length
					return 2 + 1;
				}

				// cursor lock reset
				case 'P': {
					// unroll cursor lock
					if( console_terminal -> cursor_lock ) console_terminal -> cursor_lock = TRUE;

					// disable cursor
					lib_terminal_cursor_enable( console_terminal );

					// return sequence length
					return 2 + 1;
				}

				// scroll up
				case 'S': {
					// if steps are not specified
					if( ! arg_value[ 0 ] ) { arg_value[ 0 ] = 1; arg_length[ 0 ] = 0; }	// by default 1 step

					// scroll by N lines
					while( arg_value[ 0 ]-- ) lib_terminal_scroll_up( console_terminal );

					// return sequence length
					return 2 + arg_length[ 0 ] + 1;
				}

				// scroll down
				case 'T': {
					// if steps are not specified
					if( ! arg_value[ 0 ] ) { arg_value[ 0 ] = 1; arg_length[ 0 ] = 0; }	// by default 1 step

					// scroll by N lines
					while( arg_value[ 0 ]-- ) lib_terminal_scroll_down( console_terminal );

					// return sequence length
					return 2 + arg_length[ 0 ] + 1;
				}

				// Set Graphics Mode
				case 'm': {
					// select behavior
					switch( arg_value[ 0 ] ) {
						// reset background/foreground color to default
						case 0: {
							// set default foreground/background color of terminal
							console_terminal -> color_foreground = lib_color( 255 );
							console_terminal -> color_background = lib_color( 232 );

							// return sequence length
							return 2 + arg_length[ 0 ] + 1;
						}

						// set text foreground color
						case 38: {
							// type of color sequence
							switch( arg_value[ 1 ] ) {
								// RGB
								case 2: {
									// set selected foreground color
									console_terminal -> color_foreground = STD_COLOR_mask | arg_value[ 2 ] << 16 | arg_value[ 3 ] << 8 | arg_value[ 4 ];

									// return sequence length
									return 2 + arg_length[ 0 ] + arg_length[ 1 ] + arg_length[ 2 ] + arg_length[ 3 ] + arg_length[ 4 ] + 4 + 1;
								}

								// predefinied palette
								case 5: {
									// set selected foreground color
									console_terminal -> color_foreground = lib_color( arg_value[ 2 ] );

									// return sequence length
									return 2 + arg_length[ 0 ] + arg_length[ 1 ] + arg_length[ 2 ] + 2 + 1;
								}
							}

						}

						// set text background color
						case 48: {
							// type of color sequence
							switch( arg_value[ 1 ] ) {
								// RGB
								case 2: {
									// set selected background color
									console_terminal -> color_background = STD_COLOR_mask | arg_value[ 2 ] << 16 | arg_value[ 3 ] << 8 | arg_value[ 4 ];

									// return sequence length
									return 2 + arg_length[ 0 ] + arg_length[ 1 ] + arg_length[ 2 ] + arg_length[ 3 ] + arg_length[ 4 ] + 4 + 1;
								}

								// predefinied palette
								case 5: {
									// set selected background color
									console_terminal -> color_background = lib_color( arg_value[ 2 ] );

									// return sequence length
									return 2 + arg_length[ 0 ] + arg_length[ 1 ] + arg_length[ 2 ] + 2 + 1;
								}
							}

						}
					}
				}

				// preserve cursor position
				case 's': {
					// preserve current cursor position
					console_terminal -> cursor_x_preserved = console_terminal -> cursor_x;
					console_terminal -> cursor_y_preserved = console_terminal -> cursor_y;

					// return sequence length
					return 2 + 1;
				}

				// restore cursor position
				case 'u': {
					// restore previous cursor position
					console_terminal -> cursor_x = console_terminal -> cursor_x_preserved;
					console_terminal -> cursor_y = console_terminal -> cursor_y_preserved;

					// update cursor position inside terminal
					lib_terminal_cursor_set( console_terminal );

					// return sequence length
					return 2 + 1;
				}
			}

			// sequence unsupported
			break;
		}

		// Fe
		default: {
			// choose sequence type
			switch( *string ) {
				case 'X': {
					// move pointer into sequence
					string++;

					// check string length
					uint64_t string_length = EMPTY;
					for( uint64_t i = 0; i < length - 2; i++ ) if( string[ i ] == '\\' ) break; else string_length++;

					// no end of string?
					if( string_length == length - 2 ) break;	// ERROR

					// set name length
					console_interface -> name_length = string_length - 1;
					if( console_interface -> name_length > LIB_INTERFACE_NAME_limit ) console_interface -> name_length = LIB_INTERFACE_NAME_limit;

					// copy name
					for( uint64_t i = 0; i < console_interface -> name_length; i++ ) console_interface -> name[ i ] = string[ i ];

					// inform Window Manager of new window name
					if( ! console_the_master_of_puppets ) lib_interface_name( console_interface );

					// return sequence length
					return 2 + string_length + 1;
				}

				// String Terminator
				case '\\': {
					// ERROR, unexpected end of string
					break;
				}
			}

			// sequence unsupported
			break;
		}
	}

	// return parsed sequence length
	return EMPTY;	// undefinied sequence
}
