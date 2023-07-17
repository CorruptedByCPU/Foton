/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

void lib_terminal( struct LIB_TERMINAL_STRUCTURE *terminal ) {
	// convert terminal height and width to characters
	terminal -> width_char = (terminal -> width / LIB_FONT_WIDTH_pixel) - 1;
	terminal -> height_char = (terminal -> height / LIB_FONT_HEIGHT_pixel) - 1;

	// set scanline in characters
	terminal -> scanline_char = terminal -> width / terminal -> width_char;
	terminal -> scanline_line = terminal -> scanline_pixel * LIB_FONT_HEIGHT_pixel;

	// set cursor initial position
	terminal -> cursor_x = EMPTY;
	terminal -> cursor_y = EMPTY;

	// set cursor pointer at the beginning of terminal data
	terminal -> pointer = terminal -> base_address;

	// cursor locked by default
	terminal -> cursor_lock = LOCK;

	// turn on cursor
	lib_terminal_cursor_enable( terminal );

	// clear terminal
	lib_terminal_drain( terminal );
}

void lib_terminal_char( struct LIB_TERMINAL_STRUCTURE *terminal, const char ascii ) {
	// disable cursor, no CPU power waste
	lib_terminal_cursor_disable( terminal );

	// printable character?
	if( ascii >= 0x20 && ascii <= 0x7F ) {
		// clear space under new character
		if( terminal -> flags & LIB_TERMINAL_FLAG_clean ) lib_terminal_char_drain( terminal );

		// display the character
		lib_font( LIB_FONT_FAMILY_ROBOTO_MONO, &ascii, 1, terminal -> color_foreground, terminal -> pointer, terminal -> scanline_pixel, LIB_FONT_ALIGN_left );

		// set the new cursor position
		terminal -> cursor_x++;
	} else {
		// special character NEW LINE?
		if( ascii == 0x0A ) {
			// set the cursor position from the beginning of the new line
			terminal -> cursor_x = EMPTY;
			terminal -> cursor_y++;
		}

		// special character ENTER?
		if( ascii == 0x0D ) {
			// set the cursor position from the beginning of the current line
			terminal -> cursor_x = EMPTY;
		}

		// special character BACKSPACE?
		if( ascii == 0x08 ) {
			// the cursor is at the beginning of the current line?
			if( terminal -> cursor_x )
				// move the cursor back one position
				terminal -> cursor_x--;
			else if( terminal -> cursor_y ) {
				// place the cursor at the end of the previous line
				terminal -> cursor_y--;
				terminal -> cursor_x = terminal -> width_char;
			}
		}

		// special character TAB?
		if( ascii == 0x0B ) {
			// calculate tabulator position
			terminal -> cursor_x += (LIB_TERMINAL_TAB_length - (terminal -> cursor_x % LIB_TERMINAL_TAB_length));
		}
	}

	// update cursor properties
	lib_terminal_cursor_set( terminal );

	// the character displayed was BACKSPACE?
	if( ascii == 0x08 ) lib_terminal_char_drain( terminal );

	// turn on the cursor, no CPU power waste
	lib_terminal_cursor_enable( terminal );
}

void lib_terminal_char_drain( struct LIB_TERMINAL_STRUCTURE *terminal ) {
	// clear the entire line with the default background color
	for( uint16_t y = 0; y < LIB_FONT_HEIGHT_pixel; y++ ) {
		for( uint16_t x = 0; x < LIB_FONT_WIDTH_pixel; x++ )
			terminal -> pointer[ (terminal -> scanline_pixel * y) + x ] = terminal -> color_background - (terminal -> alpha << 24);
	}
}

void lib_terminal_cursor_disable( struct LIB_TERMINAL_STRUCTURE *terminal ) {
	// lock cursor
	terminal -> cursor_lock++;

	// if cursor was not locked, hide him
	if( terminal -> cursor_lock == 1 ) lib_terminal_cursor_switch( terminal );
}

void lib_terminal_cursor_enable( struct LIB_TERMINAL_STRUCTURE *terminal ) {
	// if cursor is not locked, ignore
	if( terminal -> cursor_lock == 0 ) return;

	// unlock cursor
	terminal -> cursor_lock--;

	// if cursor is unlocked, show him
	if( terminal -> cursor_lock == 0 ) lib_terminal_cursor_switch( terminal );
}

void lib_terminal_cursor_set( struct LIB_TERMINAL_STRUCTURE *terminal ) {
	// cursor outside the space to the right?
	if( terminal -> cursor_x > terminal -> width_char ) {
		// place the cursor to the beginning of a new line
		terminal -> cursor_x = EMPTY;
		terminal -> cursor_y++;
	}

	// if cursor is out of vertical space
	if( terminal -> cursor_y > terminal -> height_char ) {
		// correct cursor position vertically
		terminal -> cursor_y--;

		// position the pointer to the cursor position
		terminal -> pointer = terminal -> base_address + ((terminal -> cursor_y * (terminal -> scanline_pixel * LIB_FONT_HEIGHT_pixel)) + (terminal -> cursor_x * terminal -> scanline_char));

		// scroll the contents of the terminal space one line up
		lib_terminal_scroll_up( terminal );
	} else
		// position the pointer to the cursor position
		terminal -> pointer = terminal -> base_address + ((terminal -> cursor_y * (terminal -> scanline_pixel * LIB_FONT_HEIGHT_pixel)) + (terminal -> cursor_x * terminal -> scanline_char));
}

void lib_terminal_cursor_switch( struct LIB_TERMINAL_STRUCTURE *terminal ) {
	// set the pointer to the cursor position
	for( uint64_t y = 0; y < LIB_FONT_HEIGHT_pixel; y++ )
		terminal -> pointer[ terminal -> scanline_pixel * y ] = (terminal -> pointer[ terminal -> scanline_pixel * y ] & STD_COLOR_mask) | (~terminal -> pointer[ terminal -> scanline_pixel * y ] & ~STD_COLOR_mask);
}

void lib_terminal_drain( struct LIB_TERMINAL_STRUCTURE *terminal ) {
	// turn of cursor
	lib_terminal_cursor_disable( terminal );

	// set pointer to first pixel of video memory area
	uint32_t *pixel = terminal -> base_address;

	// change all pixels color to DARK
	for( uint64_t y = 0; y < terminal -> height; y++ ) {
		for( uint64_t x = 0; x < terminal -> width; x++ )
			pixel[ x ] = STD_COLOR_BLACK_light;

		// next line of pixels on framebuffer
		pixel = (uint32_t *) ((uint64_t) pixel + (terminal -> scanline_pixel << STD_VIDEO_DEPTH_shift));
	}

	// fill terminal area with solid color
	for( uint64_t y = 0; y < terminal -> height; y++ ) {
		for( uint64_t x = 0; x < terminal -> width; x++ )
			terminal -> base_address[ (terminal -> scanline_pixel * y) + x ] = terminal -> color_background - (terminal -> alpha << 24);
	}

	// set new cursor position
	terminal -> cursor_x = EMPTY;
	terminal -> cursor_y = EMPTY;
	terminal -> pointer = terminal -> base_address;

	// turn on cursor
	lib_terminal_cursor_enable( terminal );
}

void lib_terminal_drain_line( struct LIB_TERMINAL_STRUCTURE *terminal ) {
	// position the pointer to the cursor position at line beginning
	uint32_t *line_pointer = terminal -> base_address + ((terminal -> cursor_y * (terminal -> scanline_pixel * LIB_FONT_HEIGHT_pixel)));

	// clear the entire line with the default background color
	for( uint16_t y = 0; y < LIB_FONT_HEIGHT_pixel; y++ )
		for( uint16_t x = 0; x < terminal -> width; x++ )
			line_pointer[ (terminal -> scanline_pixel * y) + x ] = terminal -> color_background - (terminal -> alpha << 24);
}

void lib_terminal_scroll_up( struct LIB_TERMINAL_STRUCTURE *terminal ) {
	// number of pixels to be moved
	uint64_t count = terminal -> height_char * terminal -> scanline_line;

	// scroll all lines one by one (except the last one)
	for( uint64_t i = 0; i < count; i++ )
		terminal -> base_address[ i ] = terminal -> base_address[ i + terminal -> scanline_line ];

	// clear the current terminal line
	lib_terminal_drain_line( terminal );
}

void lib_terminal_string( struct LIB_TERMINAL_STRUCTURE *terminal, const char *string, uint64_t length ) {
	// disable cursor, no CPU power waste
	lib_terminal_cursor_disable( terminal );

	// show every character from string
	for( uint64_t i = 0; i < length; i++ ) lib_terminal_char( terminal, string[ i ] );

	// turn on the cursor, no CPU power waste
	lib_terminal_cursor_enable( terminal );
}

void lib_terminal_value( struct LIB_TERMINAL_STRUCTURE *terminal, uint64_t value, uint8_t base ) {
	// if the base of the value is outside the accepted range
	if( base < 2 || base > 36 ) return;	// end of operation

	// space for value decoding
	int8_t i = 0;
	uint8_t string[ 20 ] = { 0x30 };	// 8 byte value

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
	if( ! i ) i++;

	// display generated digits from value
	do { lib_terminal_char( terminal, string[ --i ] ); }
	while( i >= 0 );
}
