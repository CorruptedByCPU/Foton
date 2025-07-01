/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#include	"../library/color.h"
#include	"../library/window.h"
#include	"../library/ui.h"

#define	W	(275 + (LIB_UI_MARGIN_DEFAULT << STD_SHIFT_2))
#define	H	(256 + (LIB_UI_HEADER_HEIGHT + LIB_UI_MARGIN_DEFAULT))

uint64_t _main( uint64_t argc, uint8_t *argv[] ) {
	struct LIB_WINDOW_STRUCTURE *window = lib_window( 606, 3, W, H );

	struct LIB_UI_STRUCTURE *ui = lib_ui( window );

	ui -> icon = lib_image_scale( lib_ui_icon( (uint8_t *) "/var/share/media/icon/default/app/gcolor3.tga" ), 48, 48, 16, 16 );

	// add icon to window properties
	for( uint64_t i = 0; i < 16 * 16; i++ ) window -> icon[ i ] = ui -> icon[ i ];

	lib_ui_clean( ui );

	lib_window_name( ui -> window, (uint8_t *) "Palette" );

	lib_ui_add_control( ui, LIB_UI_ELEMENT_CONTROL_TYPE_close );
	lib_ui_add_control( ui, LIB_UI_ELEMENT_CONTROL_TYPE_min );

	lib_ui_flush( ui );

	uint32_t *pixel = (uint32_t *) ui -> window -> pixel + ((LIB_UI_HEADER_HEIGHT) * ui -> window -> current_width) + LIB_UI_MARGIN_DEFAULT;

	for( uint64_t y = 0; y < 256; y++ )
		for( uint64_t x = 0; x < 256; x++ )
			pixel[ (y * ui -> window -> current_width) + x ] = lib_color_from_hsv( 86.1f, x / (double) 256, (256 - y) / (double) 256 );

	for( uint64_t y = 0; y < 256; y++ ) {
		uint32_t color = lib_color_from_hsv( ((double) y / 255.0f) * 359.0f, 1.0f, 1.0f );
		for( uint64_t x = 0; x < 16; x++ )
			pixel[ (y * ui -> window -> current_width) + x + 259 ] = color;
	}

	window -> flags |= LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_icon | LIB_WINDOW_FLAG_flush;

	while( TRUE ) {
		lib_ui_event( ui );

		sleep( TRUE );
	}

	return EMPTY;
}
