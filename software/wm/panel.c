/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_panel( void ) {
	// check clock status
	wm_panel_clock();

	// nothing to do?
	if( ! wm -> panel_semaphore ) return;	// done

	// count amount of object to show
	for( uint64_t i = 0; i < wm -> list_limit; i++ )
		// it's not our object? and visible? (except without name)
		if( wm -> list[ i ] -> pid != wm -> pid && wm -> list[ i ] -> descriptor -> flags & LIB_WINDOW_FLAG_visible && wm -> list[ i ] -> descriptor -> name_length )
			// insert on panel list
			wm_panel_insert( wm -> list[ i ] );

	// properties of task list area
	uint32_t *pixel = (uint32_t *) ((uintptr_t) wm -> panel -> descriptor + sizeof( struct LIB_WINDOW_STRUCTURE ));

	// clean'up panel with default color
	for( uint16_t y = 0; y < wm -> panel -> height; y++ ) for( uint16_t x = WM_PANEL_HEIGHT_pixel; x < wm -> panel -> width - WM_PANEL_CLOCK_WIDTH_pixel; x++ ) pixel[ (y * wm -> panel -> width) + x ] = WM_PANEL_COLOR_default;

	// nothing to draw?
	if( ! wm -> list_limit_panel ) {
		// update panel content on screen
		wm -> panel -> descriptor -> flags |= LIB_WINDOW_FLAG_flush;

		// done
		return;
	}

	// move pointer to first entry of panel task list
	pixel += WM_PANEL_HEIGHT_pixel;

	// first entry width in pixels
	wm -> panel_entry_width = (wm -> panel -> width - (WM_PANEL_HEIGHT_pixel + WM_PANEL_CLOCK_WIDTH_pixel)) / wm -> list_limit_panel;

	// if entry wider than allowed
	if( wm -> panel_entry_width > WM_PANEL_ENTRY_WIDTH_limit ) wm -> panel_entry_width = WM_PANEL_ENTRY_WIDTH_limit;	// limit it

	// first entry location
	uint16_t x = 0;

	// show
	for( uint64_t i = 0; i < wm -> list_limit_panel; i++ ) {
		// select default background color for entry
		uint32_t color = 0xFFFFFFFF;
		// invisible?
		if( ! (wm -> list_panel[ i ] -> descriptor -> flags & LIB_WINDOW_FLAG_visible) ) color = 0xFF808080;

		// mark active entry
		if( wm -> list_panel[ i ] == wm -> active ) for( uint16_t y = WM_PANEL_HEIGHT_pixel - 2; y < WM_PANEL_HEIGHT_pixel; y++ ) for( uint16_t x = 0; x < wm -> panel_entry_width; x++ ) pixel[ (y * wm -> panel -> width) + x ] = WM_PANEL_COLOR_active;

		// show entry name
		lib_font( LIB_FONT_FAMILY_ROBOTO, wm -> list_panel[ i ] -> descriptor -> name, wm -> list_panel[ i ] -> descriptor -> name_length, color, pixel + (4 * wm -> panel -> width) + 4, wm -> panel -> width, LIB_FONT_FLAG_ALIGN_left );

		// next entry location
		pixel += wm -> panel_entry_width;
	}

	// panel parsed
	wm -> panel_semaphore = FALSE;

	// update panel content on screen
	wm -> panel -> descriptor -> flags |= LIB_WINDOW_FLAG_flush;
}

void wm_panel_clock( void ) {
	// check current date and time
	uint64_t time = std_time();

	// it's different than previous?
	if( time == wm -> panel_clock_state ) return;	// no

	// preserve current date and time
	wm -> panel_clock_state = time;

	// properties of current time
	uint8_t hours = (uint8_t) (time >> 16);
	uint8_t minutes = (uint8_t) (time >> 8);
	uint8_t seconds = (uint8_t) (time);

	// clock template
	uint8_t clock_string[ 5 ] = "00 00";

	// fill clock area with default background color
	uint32_t *panel_pixel = (uint32_t *) ((uintptr_t) wm -> panel -> descriptor + sizeof( struct LIB_WINDOW_STRUCTURE ));
	uint32_t *clock_pixel = (uint32_t *) ((uintptr_t) wm -> panel -> descriptor + sizeof( struct LIB_WINDOW_STRUCTURE )) + (wm -> panel -> width - WM_PANEL_CLOCK_WIDTH_pixel);
	for( uint16_t y = 0; y < wm -> panel -> height; y++ )
		for( uint16_t x = 0; x < WM_PANEL_CLOCK_WIDTH_pixel; x++ )
			clock_pixel[ (y * wm -> panel -> width) + x ] = WM_PANEL_COLOR_default;

	// hour
	if( hours < 10 ) { clock_string[ 0 ] = STD_ASCII_SPACE; lib_integer_to_string( hours, 10, (uint8_t *) &clock_string[ 1 ] ); }
	else lib_integer_to_string( hours, 10, (uint8_t *) &clock_string );

	// minute
	if( minutes < 10 ) { clock_string[ 3 ] = STD_ASCII_DIGIT_0; lib_integer_to_string( minutes, 10, (uint8_t *) &clock_string[ 4 ] ); }
	else lib_integer_to_string( minutes, 10, (uint8_t *) &clock_string[ 3 ] );

	// show clock on panel
	lib_font( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) &clock_string, sizeof( clock_string ), STD_COLOR_WHITE, clock_pixel + ((((WM_PANEL_HEIGHT_pixel - LIB_FONT_HEIGHT_pixel) / 2) + TRUE) * wm -> panel -> width) + (WM_PANEL_CLOCK_WIDTH_pixel >> STD_SHIFT_2), wm -> panel -> width, LIB_FONT_FLAG_ALIGN_center );

	// colon animation
	if( seconds % 2 ) lib_font( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) ":", TRUE, STD_COLOR_WHITE - 0x00808080, clock_pixel + ((((WM_PANEL_HEIGHT_pixel - LIB_FONT_HEIGHT_pixel) / 2)) * wm -> panel -> width) + (WM_PANEL_CLOCK_WIDTH_pixel >> STD_SHIFT_2), wm -> panel -> width, LIB_FONT_FLAG_ALIGN_center );
	else lib_font( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) ":", TRUE, STD_COLOR_WHITE, clock_pixel + ((((WM_PANEL_HEIGHT_pixel - LIB_FONT_HEIGHT_pixel) / 2)) * wm -> panel -> width) + (WM_PANEL_CLOCK_WIDTH_pixel >> STD_SHIFT_2), wm -> panel -> width, LIB_FONT_FLAG_ALIGN_center );

	// update panel content on screen
	wm -> panel -> descriptor -> flags |= LIB_WINDOW_FLAG_flush;
}

void wm_panel_insert( struct WM_STRUCTURE_OBJECT *object ) {
	// check if object already exist on list
	for( uint64_t i = 0; i < wm -> list_limit; i++ ) if( wm -> list_panel[ i ] == object ) return;	// yes, done

	// insert object pointer
	wm -> list_panel[ wm -> list_limit_panel++ ] = object;
}

void wm_panel_remove( struct WM_STRUCTURE_OBJECT *object ) {
	// find object on panel list
	for( uint16_t i = 0; i < wm -> list_limit_panel; i++ ) {
		// object located?
		if( wm -> list_panel[ i ] != object ) continue;	// no

		// remove object from list
		for( uint16_t k = i; k < wm -> list_limit_panel; k++ ) wm -> list_panel[ k ] = wm -> list_panel[ k + 1 ];

		// clear last entry
		wm -> list_panel[ --wm -> list_limit_panel ] = EMPTY;

		// refresh panel content
		wm -> panel_semaphore = TRUE;

		// done
		return;
	}
}
