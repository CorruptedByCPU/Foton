/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/color.h"
	#include	"../library/font.h"
	#include	"../library/image.h"
	#include	"../library/integer.h"
	#include	"../library/window.h"
	#include	"../library/ui.h"
	//----------------------------------------------------------------------
	// static, structures, definitions
	//----------------------------------------------------------------------
	#include	"./palette/config.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"./palette/data.c"
	//----------------------------------------------------------------------
	// routines, procedures
	//----------------------------------------------------------------------
	#include	"./palette/init.c"
	#include	"./palette/ui.c"
	//----------------------------------------------------------------------

uint64_t _main( uint64_t argc, uint8_t *argv[] ) {
	palette_init();

	while( TRUE ) {
		sleep( TRUE );

		uint16_t key = lib_ui_event( ui );

		// exit?
		if( key == STD_ASCII_ESC ) return EMPTY;	// yes

		uint8_t sync = FALSE;

		if( freeze_spectrum == FALSE && (freeze_canvas || (ui -> window -> x >= LIB_UI_MARGIN_DEFAULT && ui -> window -> x < LIB_UI_MARGIN_DEFAULT + CANVAS_WIDTH && ui -> window -> y >= LIB_UI_HEADER_HEIGHT && ui -> window -> y < LIB_UI_HEADER_HEIGHT + CANVAS_HEIGHT)) ) {
			if( ui -> mouse.semaphore_left ) {
				freeze_canvas = TRUE;

				if( ui -> window -> absolute_x != canvas_x_axis_previous || ui -> window -> absolute_y != canvas_y_axis_previous) {
					int64_t x_new = (ui -> window -> absolute_x - ui -> window -> current_x) - LIB_UI_MARGIN_DEFAULT; if( x_new >= CANVAS_WIDTH ) x_new = CANVAS_WIDTH - 1; if( x_new < 0 ) x_new = 0;
					int64_t y_new = (ui -> window -> absolute_y - ui -> window -> current_y) - LIB_UI_HEADER_HEIGHT; if( y_new >= CANVAS_HEIGHT ) y_new = CANVAS_HEIGHT - 1; if( y_new < 0 ) y_new = 0;

					canvas_x_axis = x_new;
					canvas_y_axis = y_new;

					canvas_x_axis_previous = ui -> window -> absolute_x;
					canvas_y_axis_previous = ui -> window -> absolute_y;

					s = (double) x_new / (double) CANVAS_WIDTH;
					v = (CANVAS_HEIGHT - y_new) / (double) CANVAS_HEIGHT;

					sync = TRUE;
				}
			} else freeze_canvas = FALSE;
		}

		if( freeze_canvas == FALSE && (freeze_spectrum || (ui -> window -> x >= LIB_UI_MARGIN_DEFAULT + CANVAS_WIDTH + LIB_UI_PADDING_DEFAULT && ui -> window -> x < LIB_UI_MARGIN_DEFAULT + CANVAS_WIDTH + LIB_UI_PADDING_DEFAULT + SPECTRUM_WIDTH && ui -> window -> y >= LIB_UI_HEADER_HEIGHT && ui -> window -> y < LIB_UI_HEADER_HEIGHT + CANVAS_HEIGHT)) ) {
			if( ui -> mouse.semaphore_left ) {
				freeze_spectrum = TRUE;
				
				if( ui -> window -> absolute_y != spectrum_y_axis_previous ) {
					int64_t y_new = (ui -> window -> absolute_y - ui -> window -> current_y) - LIB_UI_HEADER_HEIGHT; if( y_new >= CANVAS_HEIGHT ) y_new = CANVAS_HEIGHT - 1; if( y_new < 0 ) y_new = 0;

					spectrum_y_axis = y_new;

					spectrum_y_axis_previous = ui -> window -> absolute_y;
	
					h = ((double) y_new / (double) CANVAS_HEIGHT) * 359.0f;

					sync = TRUE;
				}
			} else freeze_spectrum = FALSE;
		}

		if( sync )
			// update interface
			palette_ui();
	}

	return EMPTY;
}
