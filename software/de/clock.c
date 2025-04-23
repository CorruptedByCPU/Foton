/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint64_t de_clock( void ) {
	// infinite loop
	while( TRUE ) {
		// check current date and time
		uint64_t time = std_time();

		// it's different than previous?
		if( time == de_clock_sync ) { sleep( 512 ); continue; }	// no

		// preserve current date and time
		de_clock_sync = time;

		// properties of current time
		uint8_t hours = (uint8_t) (time >> 16);
		uint8_t minutes = (uint8_t) (time >> 8);
		uint8_t seconds = (uint8_t) (time);

		// clock template
		uint8_t clock_string[ 5 ] = "00 00";

		// fill clock area with default background color
		uint32_t *clock_pixel = (uint32_t *) ((uintptr_t) de_window_taskbar + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR )) + (de_window_taskbar -> width - DE_TASKBAR_CLOCK_pixel);
		for( uint16_t y = 0; y < de_window_taskbar -> height; y++ )
			for( uint16_t x = 0; x < DE_TASKBAR_CLOCK_pixel; x++ )
				clock_pixel[ (y * de_window_taskbar -> width) + x ] = DE_TASKBAR_BACKGROUND_default;

		// hour
		if( hours < 10 ) { clock_string[ 0 ] = STD_ASCII_SPACE; lib_integer_to_string( hours, 10, (uint8_t *) &clock_string[ 1 ] ); }
		else lib_integer_to_string( hours, 10, (uint8_t *) &clock_string );

		// minute
		if( minutes < 10 ) { clock_string[ 3 ] = STD_ASCII_DIGIT_0; lib_integer_to_string( minutes, 10, (uint8_t *) &clock_string[ 4 ] ); }
		else lib_integer_to_string( minutes, 10, (uint8_t *) &clock_string[ 3 ] );

		// show clock on taskbar
		lib_font( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) &clock_string, sizeof( clock_string ), STD_COLOR_WHITE, clock_pixel + ((((DE_TASKBAR_HEIGHT_pixel - LIB_FONT_HEIGHT_pixel) / 2) + TRUE) * de_window_taskbar -> width) + (DE_TASKBAR_CLOCK_pixel >> STD_SHIFT_2), de_window_taskbar -> width, LIB_FONT_ALIGN_center );

		// colon animation
		if( seconds % 2 ) lib_font( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) ":", TRUE, STD_COLOR_GRAY, clock_pixel + ((((DE_TASKBAR_HEIGHT_pixel - LIB_FONT_HEIGHT_pixel) / 2)) * de_window_taskbar -> width) + (DE_TASKBAR_CLOCK_pixel >> STD_SHIFT_2), de_window_taskbar -> width, LIB_FONT_ALIGN_center );
		else lib_font( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) ":", TRUE, STD_COLOR_WHITE, clock_pixel + ((((DE_TASKBAR_HEIGHT_pixel - LIB_FONT_HEIGHT_pixel) / 2)) * de_window_taskbar -> width) + (DE_TASKBAR_CLOCK_pixel >> STD_SHIFT_2), de_window_taskbar -> width, LIB_FONT_ALIGN_center );

		// update taskbar content on screen
		de_window_taskbar -> flags |= STD_WINDOW_FLAG_flush;
	}
}
