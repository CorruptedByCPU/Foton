/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

static uint32_t kernel_terminal_blend( uint32_t background, uint32_t foreground ) {
	return ((((((255 - ((foreground & 0xFF000000) >> 24)) * (background & 0x00FF00FF)) + (((foreground & 0xFF000000) >> 24) * (foreground & 0x00FF00FF))) >> 8) & 0x00FF00FF) | ((((255 - ((foreground & 0xFF000000) >> 24)) * ((background & (0xFF000000 | 0x0000FF00)) >> 8)) + (((foreground & 0xFF000000) >> 24) * (0x01000000 | ((foreground & 0x0000FF00) >> 8)))) & (0xFF000000 | 0x0000FF00)));
}

void kernel_terminal_char( char character ) {
	// printable character?
	if( character < STD_ASCII_SPACE || character > STD_ASCII_TILDE ) {
		// select special behavior
		switch( character ) {
			case STD_ASCII_BACKSPACE: { if( kernel -> terminal.cursor_x ) kernel -> terminal.cursor_x--; else if( kernel -> terminal.cursor_y ) { kernel -> terminal.cursor_x = kernel -> terminal.width_char - 1; kernel -> terminal.cursor_y--; }; break; }
			case STD_ASCII_NEW_LINE: { kernel -> terminal.cursor_x = EMPTY; kernel -> terminal.cursor_y++; kernel_terminal_register( character ); break; }
			case STD_ASCII_RETURN: { kernel -> terminal.cursor_x = EMPTY; kernel_terminal_register( character ); break; }
			case STD_ASCII_TAB: { kernel -> terminal.cursor_x += kernel -> terminal.cursor_x % KERNEL_TERMINAL_TAB_LENGTH ? KERNEL_TERMINAL_TAB_LENGTH - (kernel -> terminal.cursor_x % KERNEL_TERMINAL_TAB_LENGTH) : KERNEL_TERMINAL_TAB_LENGTH; kernel_terminal_register( character ); break; }
		}
	} else {
		// sweep away old character
		kernel_terminal_clean_character();

		// print
		kernel_terminal_register( character );

		// properties of font matrix
		uint8_t *font_matrix = (uint8_t *) lib_font_matrix + LIB_FONT_FAMILY_ROBOTO_MONO_offset;
		for( uint64_t i = 0; i < character - STD_ASCII_SPACE; i++ ) font_matrix += LIB_FONT_FAMILY_ROBOTO_MONO_KERNING;

		// for every pixel from character matrix
		for( uint8_t y = 0; y < LIB_FONT_MATRIX_height_pixel; y++ )
			for( uint16_t x = 0; x < LIB_FONT_FAMILY_ROBOTO_MONO_KERNING; x++ )
				// show him if is visible
				if( font_matrix[ (y * LIB_FONT_MATRIX_width_pixel) + x ] ) {
					// simplify below function a little
					uint32_t color_foreground_alpha = font_matrix[ (y * LIB_FONT_MATRIX_width_pixel) + x ] << 24;

					// shadow of pixel
					kernel -> terminal.pointer[ ((y + 1) * (kernel -> framebuffer_pitch_byte >> STD_VIDEO_DEPTH_shift)) + x ] = kernel_terminal_blend( kernel -> terminal.pointer[ ((y + 1) * (kernel -> framebuffer_pitch_byte >> STD_VIDEO_DEPTH_shift)) + x ], (STD_COLOR_BLACK & 0x00FFFFFF) | color_foreground_alpha );
				
					// pixel
					kernel -> terminal.pointer[ (y * (kernel -> framebuffer_pitch_byte >> STD_VIDEO_DEPTH_shift)) + x ] = kernel_terminal_blend( kernel -> terminal.pointer[ (y * (kernel -> framebuffer_pitch_byte >> STD_VIDEO_DEPTH_shift)) + x ], (kernel -> terminal.color_foreground & 0x00FFFFFF) | color_foreground_alpha );
				}

		// set the new cursor position
		kernel -> terminal.cursor_x++;
	}

	// update cursor properties
	kernel_terminal_cursor();
}

void kernel_terminal_clean( void ) {
	// remove all text from terminal
	for( uint64_t y = 0; y < kernel -> framebuffer_height_pixel; y++ ) for( uint64_t x = 0; x < kernel -> framebuffer_width_pixel; x++ ) kernel -> terminal.pixel[ (y * (kernel -> framebuffer_pitch_byte >> STD_VIDEO_DEPTH_shift)) + x ] = kernel -> terminal.color_background;
}

static void kernel_terminal_clean_character( void ) {
	// remove character from current cursor position
	for( uint64_t y = 0; y < LIB_FONT_HEIGHT_pixel; y++ ) for( uint64_t x = 0; x < LIB_FONT_WIDTH_pixel; x++ ) kernel -> terminal.pointer[ (y * (kernel -> framebuffer_pitch_byte >> STD_VIDEO_DEPTH_shift)) + x ] = kernel -> terminal.color_background;
}

void kernel_terminal_clean_line( uint64_t line ) {
	// calculate pointer of last line
	uint32_t *pixel = kernel -> terminal.pixel + (line * kernel -> terminal.scanline_pixel);

	// remove all text from terminal
	for( uint64_t y = 0; y < LIB_FONT_HEIGHT_pixel; y++ ) for( uint64_t x = 0; x < kernel -> framebuffer_width_pixel; x++ ) pixel[ (y * (kernel -> framebuffer_pitch_byte >> STD_VIDEO_DEPTH_shift)) + x ] = kernel -> terminal.color_background;
}

static void kernel_terminal_cursor( void ) {
	// cursor outside of terminal?
	if( kernel -> terminal.cursor_x > kernel -> terminal.width_char - 1 ) {
		// move cursor at beginning of new line
		kernel -> terminal.cursor_x = EMPTY;
		kernel -> terminal.cursor_y++;
	}

	// cursor outside of terminal?
	if( kernel -> terminal.cursor_y > kernel -> terminal.height_char - 1 ) {
		// correct cursor position
		kernel -> terminal.cursor_y--;

		// scroll the contents of the terminal space one line up
		kernel_terminal_scroll();
	}

	// set pointer at cursor position
	kernel -> terminal.pointer = kernel -> terminal.pixel + (kernel -> terminal.cursor_y * kernel -> terminal.scanline_pixel) + (kernel -> terminal.cursor_x * LIB_FONT_WIDTH_pixel);
}

void kernel_terminal_printf( const char *string, ... ) {
	// acquire exclusive access
	MACRO_LOCK( kernel -> terminal.lock );

	// properties of argument list
	va_list argv;

	// start of argument list
	va_start( argv, string );

	// for every character from string
	while( *string ) {
		// special character?
		if( *(string++) == STD_ASCII_PERCENT ) {
			// prefix value
			uint64_t pv = EMPTY;

			// is there a prefix?
			uint64_t p = lib_string_length_scope_digit( (uint8_t *) string );
			if( p ) {
				// yes, retrieve prefix value
				pv = lib_string_to_integer( (uint8_t *) string, STD_NUMBER_SYSTEM_decimal );
			
				// move pointer behind prefix (if even existed)
				string += p;
			}

			// select sequence type
			switch( *string ) {
				// binary
				case 'b': {
					// show
					kernel_terminal_value( va_arg( argv, uint64_t ), STD_NUMBER_SYSTEM_binary, pv, STD_ASCII_SPACE );

					// leave sequence type
					string++;

					// done
					continue;
				}

				// unsigned
				case 'd': {
					// signed value
					uint64_t value = va_arg( argv, uint64_t );

					// sign?
					if( value & STD_SIZE_QWORD_sign ) {
						// show minus before value
						kernel_terminal_char( STD_ASCII_MINUS );

						// convert to absolute
						value = ~value + 1;
					}

					// show
					kernel_terminal_value( value, STD_NUMBER_SYSTEM_decimal, pv, STD_ASCII_SPACE );

					// leave sequence type
					string++;

					// done
					continue;
				}

				// string?
				case 's': {
					// string properties
					uint8_t *substring = va_arg( argv, uint8_t * );

					// show
					for( uint64_t i = 0; i < lib_string_length( substring ); i++ ) kernel_terminal_char( substring[ i ] );

					// leave sequence type
					string++;

					// done
					continue;
				}

				// unsigned
				case 'u': {
					// show
					kernel_terminal_value( va_arg( argv, uint64_t ), STD_NUMBER_SYSTEM_decimal, pv, STD_ASCII_SPACE );

					// leave sequence type
					string++;

					// done
					continue;
				}
	
				// uppercase hexadecimal
				case 'X': {
					// show
					kernel_terminal_value( va_arg( argv, uint64_t ), STD_NUMBER_SYSTEM_hexadecimal, pv, STD_ASCII_DIGIT_0 );

					// leave sequence type
					string++;

					// done
					continue;
				}
			}
		}

		// no, show it
		kernel_terminal_char( *(string - 1) );
	}

	// end of arguemnt list
	va_end( argv );

	// sync
	for( uint64_t y = 0; y < kernel -> framebuffer_height_pixel; y++ )
	for( uint64_t x = 0; x < kernel -> framebuffer_width_pixel; x++ )
	kernel -> framebuffer_base_address[ (y * (kernel -> framebuffer_pitch_byte >> STD_VIDEO_DEPTH_shift)) + x ] = kernel -> terminal.pixel[ (y * (kernel -> framebuffer_pitch_byte >> STD_VIDEO_DEPTH_shift)) + x ] & ~STD_COLOR_mask;

	// release function
	MACRO_UNLOCK( kernel -> terminal.lock );
}

void kernel_terminal_register( char character ) {
	// // store character
	// kernel -> terminal.string[ kernel -> terminal.length++ ] = character;

	// // extend cache?
	// if( ! (kernel -> terminal.length & ~STD_PAGE_mask) ) {	// yea
	// 	// allocate new area
	// 	uint8_t *new = (uint8_t *) kernel_memory_alloc( (MACRO_PAGE_ALIGN_UP( kernel -> terminal.length ) >> STD_SHIFT_PAGE) + 1 );

	// 	// preserve old content
	// 	memcpy( new, kernel -> terminal.string, kernel -> terminal.length );

	// 	// release old area
	// 	kernel_memory_release( (uintptr_t) kernel -> terminal.string, MACRO_PAGE_ALIGN_UP( kernel -> terminal.length ) >> STD_SHIFT_PAGE );

	// 	// done
	// 	kernel -> terminal.string = new;
	// }
}

static void kernel_terminal_scroll( void ) {
	// copy all lines (except last) one position up
	for( uint64_t i = 0; i < (kernel -> terminal.height_char - 1) * kernel -> terminal.scanline_pixel; i++ )
		kernel -> terminal.pixel[ i ] = kernel -> terminal.pixel[ i + kernel -> terminal.scanline_pixel ];

	// clean last line
	kernel_terminal_clean_line( kernel -> terminal.height_char - 1 );
}

void kernel_terminal_value( uint64_t value, uint8_t base, uint8_t prefix, uint8_t character ) {
	// space for value decoding
	uint8_t i = 0;
	char string[ 64 ]; for( uint8_t k = 0; k < 64; k++ ) string[ k ] = character;

	// convert value to individual digits
	while( value ) {
		// first digit of the value
		uint8_t digit = value % base;

		// convert digit to ASCII
		if( digit < 10 ) string[ i++ ] = digit + 0x30;
		else string[ i++ ] = digit + 0x37;

		// truncate a digit from value
		value /= base;
	};

	// empty value?
	if( ! i ) { i++; string[ i - 1 ] = STD_ASCII_DIGIT_0; }

	// prefix wider than value?
	if( prefix > i ) i = prefix;

	// display generated digits from value
	while( i ) kernel_terminal_char( string[ --i ] );
}
