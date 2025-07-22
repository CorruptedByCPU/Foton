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
	// create window and UI
	palette_init();

	// infinite loop
	while( TRUE ) {
		// check for events
		uint16_t key = lib_ui_event( ui );

		// exit?
		if( key == STD_ASCII_ESC ) return EMPTY;	// yes

		// by default no redraw required
		uint8_t sync = FALSE;

		// don't move cross if dragging inside/around spectrum
		if( freeze_spectrum == FALSE && (freeze_canvas || (ui -> window -> x >= LIB_UI_MARGIN_DEFAULT && ui -> window -> x < LIB_UI_MARGIN_DEFAULT + CANVAS_WIDTH && ui -> window -> y >= LIB_UI_HEADER_HEIGHT && ui -> window -> y < LIB_UI_HEADER_HEIGHT + CANVAS_HEIGHT)) ) {
			// start canvas cross movement?
			if( ui -> mouse.semaphore_left ) {
				// yes
				freeze_canvas = TRUE;

				// if there is any change in cursor location
				if( ui -> window -> absolute_x != canvas_x_axis_previous || ui -> window -> absolute_y != canvas_y_axis_previous) {
					// calculate new cross location
					int64_t x_new = (ui -> window -> absolute_x - ui -> window -> current_x) - LIB_UI_MARGIN_DEFAULT; if( x_new >= CANVAS_WIDTH ) x_new = CANVAS_WIDTH - 1; if( x_new < 0 ) x_new = 0;
					int64_t y_new = (ui -> window -> absolute_y - ui -> window -> current_y) - LIB_UI_HEADER_HEIGHT; if( y_new >= CANVAS_HEIGHT ) y_new = CANVAS_HEIGHT - 1; if( y_new < 0 ) y_new = 0;

					// set
					canvas_x_axis = x_new;
					canvas_y_axis = y_new;

					// remember current absolute cursor location
					canvas_x_axis_previous = ui -> window -> absolute_x;
					canvas_y_axis_previous = ui -> window -> absolute_y;

					// calculate gradient for canvas as Saturation and Value
					s = (double) x_new / (double) CANVAS_WIDTH;
					v = (CANVAS_HEIGHT - y_new) / (double) CANVAS_HEIGHT;

					// redraw of canvas required
					sync = TRUE;
				}
			// release canvas cross
			} else freeze_canvas = FALSE;
		}

		// don't move cross if dragging inside/around canvas
		if( freeze_canvas == FALSE && (freeze_spectrum || (ui -> window -> x >= LIB_UI_MARGIN_DEFAULT + CANVAS_WIDTH + LIB_UI_PADDING_DEFAULT && ui -> window -> x < LIB_UI_MARGIN_DEFAULT + CANVAS_WIDTH + LIB_UI_PADDING_DEFAULT + SPECTRUM_WIDTH && ui -> window -> y >= LIB_UI_HEADER_HEIGHT && ui -> window -> y < LIB_UI_HEADER_HEIGHT + CANVAS_HEIGHT)) ) {
			// start spectrum lever movement?
			if( ui -> mouse.semaphore_left ) {
				// yes
				freeze_spectrum = TRUE;
				
				// if there is any change in cursor location
				if( ui -> window -> absolute_y != spectrum_y_axis_previous ) {
					// calculate new lever location
					int64_t y_new = (ui -> window -> absolute_y - ui -> window -> current_y) - LIB_UI_HEADER_HEIGHT; if( y_new >= CANVAS_HEIGHT ) y_new = CANVAS_HEIGHT - 1; if( y_new < 0 ) y_new = 0;

					// set
					spectrum_y_axis = y_new;

					// remember current absolute cursor location
					spectrum_y_axis_previous = ui -> window -> absolute_y;
	
					// calculate gradient for canvas as Hue
					h = ((double) y_new / (double) CANVAS_HEIGHT) * 359.0f;

					// redraw of canvas required
					sync = TRUE;
				}
			// release spectrum lever
			} else freeze_spectrum = FALSE;
		}

		// update interface if required
		if( sync ) palette_ui();
	}

	// close application
	return EMPTY;
}
