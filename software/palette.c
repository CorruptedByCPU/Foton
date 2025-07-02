/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#include	"../library/color.h"
#include	"../library/font.h"
#include	"../library/window.h"
#include	"../library/ui.h"

#define	SPECTRUM_WIDTH	8

#define	WINDOW_WIDTH	(LIB_UI_MARGIN_DEFAULT + CANVAS_W + LIB_UI_PADDING_DEFAULT + SPECTRUM_WIDTH + LIB_UI_PADDING_DEFAULT + SAMPLE_W + LIB_UI_MARGIN_DEFAULT)
#define	WINDOW_HEIGHT	(LIB_UI_HEADER_HEIGHT + CANVAS_H + LIB_UI_MARGIN_DEFAULT)

#define	CANVAS_W	156
#define	CANVAS_H	156

#define	SAMPLE_W	108
#define	SAMPLE_H	50

uint64_t _main( uint64_t argc, uint8_t *argv[] ) {
	struct LIB_WINDOW_STRUCTURE *window = lib_window( 606, 3, WINDOW_WIDTH, WINDOW_HEIGHT );

	struct LIB_UI_STRUCTURE *ui = lib_ui( window );

	ui -> icon = lib_image_scale( lib_ui_icon( (uint8_t *) "/var/share/media/icon/default/app/gcolor3.tga" ), 48, 48, 16, 16 );

	// add icon to window properties
	for( uint64_t i = 0; i < 16 * 16; i++ ) window -> icon[ i ] = ui -> icon[ i ];

	lib_ui_clean( ui );

	lib_window_name( ui -> window, (uint8_t *) "Palette" );

	lib_ui_add_control( ui, LIB_UI_ELEMENT_CONTROL_TYPE_close );
	lib_ui_add_control( ui, LIB_UI_ELEMENT_CONTROL_TYPE_min );

	// column 0
	uint64_t x = LIB_UI_MARGIN_DEFAULT + CANVAS_W + LIB_UI_PADDING_DEFAULT + SPECTRUM_WIDTH + LIB_UI_PADDING_DEFAULT;
	uint64_t y = LIB_UI_HEADER_HEIGHT + SAMPLE_H + LIB_UI_PADDING_DEFAULT;

	uint8_t red[] = "R:";
	lib_ui_add_label( ui, x, y, lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) &red, sizeof( red ) - 1 ), (uint8_t *) &red, EMPTY, EMPTY );
	y += LIB_FONT_HEIGHT_pixel + LIB_UI_PADDING_DEFAULT;
	uint8_t green[] = "G:";
	lib_ui_add_label( ui, x, y, lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) &green, sizeof( green ) - 1 ), (uint8_t *) &green, EMPTY, EMPTY );
	y += LIB_FONT_HEIGHT_pixel + LIB_UI_PADDING_DEFAULT;
	uint8_t blue[] = "B:";
	lib_ui_add_label( ui, x, y, lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) &blue, sizeof( blue ) - 1 ), (uint8_t *) &blue, EMPTY, EMPTY );

	x += lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) &red, sizeof( red ) - 1 ) + LIB_UI_PADDING_DEFAULT;
	y = LIB_UI_HEADER_HEIGHT + SAMPLE_H + LIB_UI_PADDING_DEFAULT;

	lib_ui_add_input( ui, x, y, 32, (uint8_t *) "0", LIB_UI_ELEMENT_FLAG_disabled, LIB_FONT_FLAG_ALIGN_center );
	y += LIB_FONT_HEIGHT_pixel + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_input( ui, x, y, 32, (uint8_t *) "0", LIB_UI_ELEMENT_FLAG_disabled, LIB_FONT_FLAG_ALIGN_center );
	y += LIB_FONT_HEIGHT_pixel + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_input( ui, x, y, 32, (uint8_t *) "0", LIB_UI_ELEMENT_FLAG_disabled, LIB_FONT_FLAG_ALIGN_center );

	// column 1
	x += 32 + LIB_UI_PADDING_DEFAULT;
	y = LIB_UI_HEADER_HEIGHT + SAMPLE_H + LIB_UI_PADDING_DEFAULT;

	uint8_t hue[] = "H:";
	lib_ui_add_label( ui, x, y, lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) &hue, sizeof( hue ) - 1 ), (uint8_t *) &hue, EMPTY, EMPTY );
	y += LIB_FONT_HEIGHT_pixel + LIB_UI_PADDING_DEFAULT;
	uint8_t saturation[] = "S:";
	lib_ui_add_label( ui, x, y, lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) &saturation, sizeof( saturation ) - 1 ), (uint8_t *) &saturation, EMPTY, EMPTY );
	y += LIB_FONT_HEIGHT_pixel + LIB_UI_PADDING_DEFAULT;
	uint8_t brightness[] = "V:";
	lib_ui_add_label( ui, x, y, lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) &brightness, sizeof( brightness ) - 1 ), (uint8_t *) &brightness, EMPTY, EMPTY );

	x += lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) &red, sizeof( red ) - 1 ) + LIB_UI_PADDING_DEFAULT;
	y = LIB_UI_HEADER_HEIGHT + SAMPLE_H + LIB_UI_PADDING_DEFAULT;

	lib_ui_add_input( ui, x, y, 32, (uint8_t *) "0", LIB_UI_ELEMENT_FLAG_disabled, LIB_FONT_FLAG_ALIGN_center );
	y += LIB_FONT_HEIGHT_pixel + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_input( ui, x, y, 32, (uint8_t *) "0", LIB_UI_ELEMENT_FLAG_disabled, LIB_FONT_FLAG_ALIGN_center );
	y += LIB_FONT_HEIGHT_pixel + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_input( ui, x, y, 32, (uint8_t *) "0", LIB_UI_ELEMENT_FLAG_disabled, LIB_FONT_FLAG_ALIGN_center );
	y += LIB_FONT_HEIGHT_pixel + LIB_UI_PADDING_DEFAULT;

	x = LIB_UI_MARGIN_DEFAULT + CANVAS_W + LIB_UI_PADDING_DEFAULT + SPECTRUM_WIDTH + LIB_UI_PADDING_DEFAULT + ((WINDOW_WIDTH - (LIB_UI_MARGIN_DEFAULT + CANVAS_W + LIB_UI_PADDING_DEFAULT + SPECTRUM_WIDTH + LIB_UI_PADDING_DEFAULT)) >> STD_SHIFT_2) - lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) "#", 1 ) - 32;
	y = WINDOW_HEIGHT - LIB_UI_MARGIN_DEFAULT - LIB_UI_ELEMENT_INPUT_height - ((WINDOW_HEIGHT - y) >> STD_SHIFT_2) + (LIB_UI_ELEMENT_INPUT_height >> STD_SHIFT_2);

	lib_ui_add_label( ui, x, y, lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) "#", 1 ), (uint8_t *) "#", EMPTY, EMPTY );
	x += lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) "#", 1 ) + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_input( ui, x, y, 64, (uint8_t *) "86B057", LIB_UI_ELEMENT_FLAG_disabled, LIB_FONT_FLAG_ALIGN_center );

	lib_ui_flush( ui );

	uint32_t *pixel = (uint32_t *) ui -> window -> pixel + ((LIB_UI_HEADER_HEIGHT) * ui -> window -> current_width) + LIB_UI_MARGIN_DEFAULT;

	for( uint64_t y = 0; y < CANVAS_H; y++ )
		for( uint64_t x = 0; x < CANVAS_W; x++ )
			pixel[ (y * ui -> window -> current_width) + x ] = lib_color_hsv( 88.3f, x / (double) CANVAS_W, (CANVAS_H - y) / (double) CANVAS_H );

	for( uint64_t y = 0; y < CANVAS_H; y++ ) {
		uint32_t color = lib_color_hsv( ((double) y / 256.0f) * 359.0f, 1.0f, 1.0f );
		for( uint64_t x = 0; x < SPECTRUM_WIDTH; x++ )
			pixel[ (y * ui -> window -> current_width) + x + LIB_UI_PADDING_DEFAULT + CANVAS_W ] = color;
	}

	pixel += CANVAS_W + LIB_UI_PADDING_DEFAULT + SPECTRUM_WIDTH + LIB_UI_PADDING_DEFAULT;

	for( uint64_t y = 0; y < SAMPLE_H; y++ ) {
		for( uint64_t x = 0; x < SAMPLE_W; x++ )
			pixel[ (y * ui -> window -> current_width) + x ] = lib_color_hsv( 88.3f, 50.6f / 100.0f, 69.0f / 100.0f );
	}

	window -> flags |= LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_icon | LIB_WINDOW_FLAG_flush;

	while( TRUE ) {
		lib_ui_event( ui );

		sleep( TRUE );
	}

	return EMPTY;
}
