/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

int64_t wm_clock( void ) {
	// infinite loop
	while( TRUE ) {
		// check current date and time
		uint64_t time = std_time();

		// it's different than previous?
		if( time == wm_taskbar_clock_time ) { std_sleep( 512 ); continue; }	// no

		// preserve current date and time
		wm_taskbar_clock_time = time;

		// properties of current time
		uint8_t hours = (uint8_t) (time >> 16);
		uint8_t minutes = (uint8_t) (time >> 8);
		uint8_t seconds = (uint8_t) (time);

		// clock template
		uint8_t clock_string[ 5 ] = "00:00";

		// hour
		if( hours < 10 ) { clock_string[ 0 ] = STD_ASCII_SPACE; lib_integer_to_string( hours, 10, (uint8_t *) &clock_string[ 1 ] ); }
		else lib_integer_to_string( hours, 10, (uint8_t *) &clock_string );

		// colon animation
		if( seconds % 2 ) clock_string[ 2 ] = STD_ASCII_SPACE;

		// minute
		if( minutes < 10 ) { clock_string[ 3 ] = STD_ASCII_DIGIT_0; lib_integer_to_string( minutes, 10, (uint8_t *) &clock_string[ 4 ] ); }
		else lib_integer_to_string( minutes, 10, (uint8_t *) &clock_string[ 3 ] );

		// fill clock area with default background color
		uint32_t *clock_pixel = (uint32_t *) ((uintptr_t) wm_object_taskbar -> descriptor + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR )) + (wm_object_taskbar -> width - WM_OBJECT_TASKBAR_CLOCK_pixel);
		for( uint16_t y = 0; y < wm_object_taskbar -> height; y++ )
			for( uint16_t x = 0; x < WM_OBJECT_TASKBAR_CLOCK_pixel; x++ )
				clock_pixel[ (y * wm_object_taskbar -> width) + x ] = WM_TASKBAR_BG_default;

		// show clock on taskbar
		lib_font( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) &clock_string, sizeof( clock_string ), 0xFFFFFFFF, clock_pixel + (((WM_OBJECT_TASKBAR_HEIGHT_pixel - LIB_FONT_HEIGHT_pixel) / 2) * wm_object_taskbar -> width) + (50 >> STD_SHIFT_2), wm_object_taskbar -> width, LIB_FONT_ALIGN_center );

		// update taskbar content on screen
		wm_object_taskbar -> descriptor -> flags |= STD_WINDOW_FLAG_flush;
	}
}
