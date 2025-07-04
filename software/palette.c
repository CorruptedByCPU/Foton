/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

// a makeshift
// to be done

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/color.h"
	#include	"../library/font.h"
	#include	"../library/integer.h"
	#include	"../library/window.h"
	#include	"../library/ui.h"
	// //----------------------------------------------------------------------
	// // static, structures, definitions
	// //----------------------------------------------------------------------
	// #include	"./kuro/config.h"
	// #include	"./kuro/dir.h"
	// #include	"./kuro/icon.h"
	// //----------------------------------------------------------------------
	// // variables
	// //----------------------------------------------------------------------
	// #include	"./kuro/data.c"
	// //----------------------------------------------------------------------
	// // routines, procedures
	// //----------------------------------------------------------------------
	// #include	"./kuro/dir.c"
	// #include	"./kuro/icon.c"
	// #include	"./kuro/init.c"
	// //----------------------------------------------------------------------



#define	WINDOW_WIDTH	(LIB_UI_MARGIN_DEFAULT + CANVAS_W + LIB_UI_PADDING_DEFAULT + SPECTRUM_WIDTH + LIB_UI_PADDING_DEFAULT + SAMPLE_W + LIB_UI_MARGIN_DEFAULT)
#define	WINDOW_HEIGHT	(LIB_UI_HEADER_HEIGHT + CANVAS_H + LIB_UI_MARGIN_DEFAULT)

#define	CANVAS_W	256
#define	CANVAS_H	256

#define	SPECTRUM_WIDTH	CANVAS_W / 10

#define	SAMPLE_W	108
#define	SAMPLE_H	50

struct LIB_WINDOW_STRUCTURE *window;
struct LIB_UI_STRUCTURE *ui;

double h = 88.3f;
double s = 0.506f;
double v = 0.69f;

uint64_t ir;
uint64_t ig;
uint64_t ib;
uint64_t ih;
uint64_t is;
uint64_t iv;
uint64_t irgb;

uint64_t x_canvas_current;
uint64_t y_canvas_current;
uint64_t y_spectrum_current;

uint64_t x_canvas_old;
uint64_t y_canvas_old;
uint64_t y_spectrum_old;

uint8_t hold_canvas = FALSE;
uint8_t hold_spectrum = FALSE;

uint8_t input_tmp[ 9 ] = { EMPTY };

void canvas( void ) {
	uint32_t *pixel = (uint32_t *) ui -> window -> pixel + ((LIB_UI_HEADER_HEIGHT) * ui -> window -> current_width) + LIB_UI_MARGIN_DEFAULT;

	for( uint64_t y = 0; y < CANVAS_H; y++ )
		for( uint64_t x = 0; x < CANVAS_W; x++ )
			pixel[ (y * ui -> window -> current_width) + x ] = lib_color_hsv( h, x / (double) CANVAS_W, (CANVAS_H - y) / (double) CANVAS_H );

	for( uint64_t x = 0; x < CANVAS_W; x++ ) pixel[ (y_canvas_current * ui -> window -> current_width) + x ] = 0xFF000000 | ~pixel[ (y_canvas_current * ui -> window -> current_width) + x ];
	for( uint64_t y = 0; y < CANVAS_H; y++ ) pixel[ (y * ui -> window -> current_width) + x_canvas_current ] = 0xFF000000 | ~pixel[ (y * ui -> window -> current_width) + x_canvas_current ];

	for( uint64_t y = 0; y < CANVAS_H; y++ ) {
		uint32_t color = lib_color_hsv( ((double) y / (double) CANVAS_H) * 359.0f, 1.0f, 1.0f );
		for( uint64_t x = 0; x < SPECTRUM_WIDTH; x++ )
			pixel[ (y * ui -> window -> current_width) + x + LIB_UI_PADDING_DEFAULT + CANVAS_W ] = color;
	}

	for( uint64_t x = 0; x < SPECTRUM_WIDTH; x++ ) pixel[ (y_spectrum_current * ui -> window -> current_width) + x + LIB_UI_PADDING_DEFAULT + CANVAS_W ] = 0xFF000000 | ~pixel[ (y_spectrum_current * ui -> window -> current_width) + x + LIB_UI_PADDING_DEFAULT + CANVAS_W ];
}

void sample( void ) {
	uint32_t *pixel = (uint32_t *) ui -> window -> pixel + ((LIB_UI_HEADER_HEIGHT) * ui -> window -> current_width) + LIB_UI_MARGIN_DEFAULT + CANVAS_W + LIB_UI_PADDING_DEFAULT + SPECTRUM_WIDTH + LIB_UI_PADDING_DEFAULT;

	uint32_t color = lib_color_hsv( h, s, v );

	for( uint64_t y = 0; y < SAMPLE_H; y++ ) {
		for( uint64_t x = 0; x < SAMPLE_W; x++ )
			pixel[ (y * ui -> window -> current_width) + x ] = color;
	}
}

void update( void ) {
	uint64_t length;

	uint32_t color = lib_color_hsv( h, s, v );

	length = lib_integer_to_string( (uint8_t) (color >> 16), 10, (uint8_t *) &input_tmp );
	input_tmp[ length ] = STD_ASCII_TERMINATOR;
	lib_ui_update_input( ui, ir, (uint8_t *) &input_tmp );

	length = lib_integer_to_string( (uint8_t) (color >> 8), 10, (uint8_t *) &input_tmp );
	input_tmp[ length ] = STD_ASCII_TERMINATOR;
	lib_ui_update_input( ui, ig, (uint8_t *) &input_tmp );

	length = lib_integer_to_string( (uint8_t) color, 10, (uint8_t *) &input_tmp );
	input_tmp[ length ] = STD_ASCII_TERMINATOR;
	lib_ui_update_input( ui, ib, (uint8_t *) &input_tmp );

	length = lib_integer_to_string( (uint64_t) h, 10, (uint8_t *) &input_tmp );
	input_tmp[ length ] = STD_ASCII_TERMINATOR;
	lib_ui_update_input( ui, ih, (uint8_t *) &input_tmp );

	length = lib_integer_to_string( (uint64_t) (s * 100.0f), 10, (uint8_t *) &input_tmp );
	input_tmp[ length ] = STD_ASCII_TERMINATOR;
	lib_ui_update_input( ui, is, (uint8_t *) &input_tmp );

	length = lib_integer_to_string( (uint64_t) (v * 100.0f), 10, (uint8_t *) &input_tmp );
	input_tmp[ length ] = STD_ASCII_TERMINATOR;
	lib_ui_update_input( ui, iv, (uint8_t *) &input_tmp );

	lib_integer_to_string( color, 16, (uint8_t *) &input_tmp );
	input_tmp[ 8 ] = STD_ASCII_TERMINATOR;
	lib_ui_update_input( ui, irgb, (uint8_t *) &input_tmp[ 2 ] );

	lib_ui_flush( ui );
}

uint64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initial axis
	x_canvas_current = CANVAS_W - (s * CANVAS_W);
	y_canvas_current = CANVAS_H - (v * CANVAS_H);
	y_spectrum_current = (h / 360.0f) * CANVAS_H;

	window = lib_window( 606, 3, WINDOW_WIDTH, WINDOW_HEIGHT );

	ui = lib_ui( window );

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

	ir = lib_ui_add_input( ui, x, y, 32, (uint8_t *) "0", LIB_UI_ELEMENT_FLAG_disabled, EMPTY );
	y += LIB_FONT_HEIGHT_pixel + LIB_UI_PADDING_DEFAULT;
	ig = lib_ui_add_input( ui, x, y, 32, (uint8_t *) "0", LIB_UI_ELEMENT_FLAG_disabled, EMPTY );
	y += LIB_FONT_HEIGHT_pixel + LIB_UI_PADDING_DEFAULT;
	ib = lib_ui_add_input( ui, x, y, 32, (uint8_t *) "0", LIB_UI_ELEMENT_FLAG_disabled, EMPTY );

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

	ih = lib_ui_add_input( ui, x, y, 32, (uint8_t *) "88", LIB_UI_ELEMENT_FLAG_disabled, EMPTY );
	y += LIB_FONT_HEIGHT_pixel + LIB_UI_PADDING_DEFAULT;
	is = lib_ui_add_input( ui, x, y, 32, (uint8_t *) "50", LIB_UI_ELEMENT_FLAG_disabled, EMPTY );
	y += LIB_FONT_HEIGHT_pixel + LIB_UI_PADDING_DEFAULT;
	iv = lib_ui_add_input( ui, x, y, 32, (uint8_t *) "69", LIB_UI_ELEMENT_FLAG_disabled, EMPTY );
	y += LIB_FONT_HEIGHT_pixel + LIB_UI_PADDING_DEFAULT;

	x = LIB_UI_MARGIN_DEFAULT + CANVAS_W + LIB_UI_PADDING_DEFAULT + SPECTRUM_WIDTH + LIB_UI_PADDING_DEFAULT + ((WINDOW_WIDTH - (LIB_UI_MARGIN_DEFAULT + CANVAS_W + LIB_UI_PADDING_DEFAULT + SPECTRUM_WIDTH + LIB_UI_PADDING_DEFAULT)) >> STD_SHIFT_2) - lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) "#", 1 ) - 32;
	y = WINDOW_HEIGHT - LIB_UI_MARGIN_DEFAULT - LIB_UI_ELEMENT_INPUT_height - ((WINDOW_HEIGHT - y) >> STD_SHIFT_2) + (LIB_UI_ELEMENT_INPUT_height >> STD_SHIFT_2);

	lib_ui_add_label( ui, x, y, lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) "#", 1 ), (uint8_t *) "#", EMPTY, EMPTY );
	x += lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) "#", 1 ) + LIB_UI_PADDING_DEFAULT;
	irgb = lib_ui_add_input( ui, x, y, 64, (uint8_t *) "86B057", LIB_UI_ELEMENT_FLAG_disabled, EMPTY );

	lib_ui_flush( ui );

	canvas();
	sample();

	window -> flags |= LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_icon | LIB_WINDOW_FLAG_flush;

	while( TRUE ) {
		sleep( TRUE );

		lib_ui_event( ui );

		// recieve key
		uint16_t key = getkey();

		// exit?
		if( key == STD_ASCII_ESC ) return EMPTY;	// yes

		uint8_t sync = FALSE;

		if( hold_spectrum == FALSE && (hold_canvas || (ui -> window -> x >= LIB_UI_MARGIN_DEFAULT && ui -> window -> x < LIB_UI_MARGIN_DEFAULT + CANVAS_W && ui -> window -> y >= LIB_UI_HEADER_HEIGHT && ui -> window -> y < LIB_UI_HEADER_HEIGHT + CANVAS_H)) ) {
			if( ui -> mouse.semaphore_left ) {
				hold_canvas = TRUE;

				if( ui -> window -> absolute_x != x_canvas_old || ui -> window -> absolute_y != y_canvas_old) {
					int64_t x_new = (ui -> window -> absolute_x - ui -> window -> current_x) - LIB_UI_MARGIN_DEFAULT; if( x_new >= CANVAS_W ) x_new = CANVAS_W - 1; if( x_new < 0 ) x_new = 0;
					int64_t y_new = (ui -> window -> absolute_y - ui -> window -> current_y) - LIB_UI_HEADER_HEIGHT; if( y_new >= CANVAS_H ) y_new = CANVAS_H - 1; if( y_new < 0 ) y_new = 0;

					x_canvas_current = x_new;
					y_canvas_current = y_new;

					x_canvas_old = ui -> window -> absolute_x;
					y_canvas_old = ui -> window -> absolute_y;

					s = (double) x_new / (double) CANVAS_W;
					v = (CANVAS_H - y_new) / (double) CANVAS_H;

					sync = TRUE;
				}
			} else hold_canvas = FALSE;
		}

		if( hold_canvas == FALSE && (hold_spectrum || (ui -> window -> x >= LIB_UI_MARGIN_DEFAULT + CANVAS_W + LIB_UI_PADDING_DEFAULT && ui -> window -> x < LIB_UI_MARGIN_DEFAULT + CANVAS_W + LIB_UI_PADDING_DEFAULT + SPECTRUM_WIDTH && ui -> window -> y >= LIB_UI_HEADER_HEIGHT && ui -> window -> y < LIB_UI_HEADER_HEIGHT + CANVAS_H)) ) {
			if( ui -> mouse.semaphore_left ) {
				hold_spectrum = TRUE;
				
				if( ui -> window -> absolute_y != y_spectrum_old ) {
					int64_t y_new = (ui -> window -> absolute_y - ui -> window -> current_y) - LIB_UI_HEADER_HEIGHT; if( y_new >= CANVAS_H ) y_new = CANVAS_H - 1; if( y_new < 0 ) y_new = 0;

					y_spectrum_current = y_new;

					y_spectrum_old = ui -> window -> absolute_y;
	
					h = ((double) y_new / (double) CANVAS_H) * 359.0f;

					sync = TRUE;
				}
			} else hold_spectrum = FALSE;
		}

		if( sync ) {
			update();

			canvas();
			sample();

			window -> flags |= LIB_WINDOW_FLAG_flush;
			while( window -> flags & LIB_WINDOW_FLAG_flush );
		}
	}

	return EMPTY;
}
