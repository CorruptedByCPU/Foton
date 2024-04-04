/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_menu( void ) {
	// open menu file
	FILE *file = fopen( (uint8_t *) "/system/etc/menu" );

	// menu file doesn't exist?
	if( ! file ) {
		// execute console application
		std_exec( (uint8_t *) "console", 7, EMPTY );

		// done
		return;
	}

	// allocate area for file content
	uint8_t *menu = (uint8_t *) calloc( file -> byte + 1 );
	if( ! menu ) {
		// cannot allocate enough memory
		fclose( file );

		// done
		return;
	}

	// load file content into memory
	fread( file, menu, file -> byte );

	// close menu file, we doesn't care anymore
	fclose( file );




	// find widest menu entry, and calculate height of menu window
	uint64_t width_pixel = 0;
	uint64_t height_pixel = 0;

	// until end of menu file
	uint8_t *search = menu; while( *search ) {
		// line length
		uint64_t line_length = lib_string_length_line( search );

		// entry width in characters
		uint64_t width_current = lib_font_length_string( LIB_FONT_FAMILY_ROBOTO, search, lib_string_word( search, line_length ) );

		// wider than others?
		if( width_current > width_pixel ) width_pixel = width_current;

		// menu entry
		height_pixel += LIB_FONT_HEIGHT_pixel;

		// move to next line
		search += line_length + 1;
	}

	// prepare empty menu window
	interface_menu = lib_interface_create( width_pixel + (LIB_INTERFACE_BORDER_pixel << STD_SHIFT_2), height_pixel + LIB_INTERFACE_BORDER_pixel, (uint8_t *) "Menu" );

	log( "%u x %u\n", width_pixel, height_pixel );

	// create menu object
	wm_object_menu = wm_object_create( -LIB_INTERFACE_SHADOW_length, wm_object_taskbar -> y - (interface_menu -> height - LIB_INTERFACE_SHADOW_length), interface_menu -> width, interface_menu -> height );

	// mark it as own
	wm_object_menu -> pid = wm_pid;

	// uint32_t *menu_pixel = (uint32_t *) ((uintptr_t) wm_object_menu -> descriptor + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR ));
	// for( uint16_t y = 0; y < wm_object_menu -> height; y++ )
	// 	for( uint16_t x = 0; x < wm_object_menu -> width; x++ )
	// 		menu_pixel[ (y * wm_object_menu -> width) + x ] = STD_COLOR_RED;

	// update interface window descriptor
	interface_menu -> descriptor = wm_object_menu -> descriptor;

	// set shadow length
	interface_menu -> descriptor -> offset = LIB_INTERFACE_SHADOW_length;

	// prepare shadow of window
	lib_interface_shadow( interface_menu );

	// clear window content
	lib_interface_clear( interface_menu );

	// show window name in header if set
	lib_interface_name( interface_menu );

	// debug
	wm_object_menu -> descriptor -> flags |= STD_WINDOW_FLAG_unstable | STD_WINDOW_FLAG_fixed_xy | STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;

	// release file content
	free( menu );
}
