/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#include	"../library/ui.h"

void lib_ui_fill_rectangle( uint32_t *pixel, uint64_t scanline, uint8_t r, uint64_t width, uint64_t height, uint32_t color ) {
	for( uint64_t y = 0; y < height; y++ )
		for( uint64_t x = 0; x < width; x++ ) {
			// no round corners?
			if( ! r ) { pixel[ (y * scanline) + x ] = color; continue; }

			// inner content of rectangle?
			if( (x >= r && x < width - r) || (y >= r && y < height - r) ) { pixel[ (y * scanline) + x ] = color; continue; }

			// check if x,y is inside circle
			int64_t dx, dy;

			// left-top corner
			dx = x - r; dy = y - r;
			if( x < r && y < r && ((dx * dx) + (dy * dy) < (r * r)) ) { pixel[ (y * scanline) + x ] = color; continue; }

			// right-top corner
			dx = x - (width - r - 1); dy = y - r;
			if( x >= width - r && y < r && ((dx * dx) + (dy * dy) < (r * r)) ) { pixel[ (y * scanline) + x ] = color; continue; }

			// right-botom corner
			dx = x - r; dy = y - (height - r - 1);
			if( x < r && y >= height - r && ((dx * dx) + (dy * dy) < (r * r)) ) { pixel[ (y * scanline) + x ] = color; continue; }
			
			// left-bottom corner
			dx = x - (width - r - 1); dy = y - (height - r - 1);
			if( x >= width - r && y >= height - r && ((dx * dx) + (dy * dy) < (r * r)) ) { pixel[ (y * scanline) + x ] = color; continue; }
		}
}

struct LIB_UI_STRUCTURE *lib_ui( struct LIB_WINDOW_STRUCTURE *window ) {
	// initialize structure
	struct LIB_UI_STRUCTURE *ui = calloc( sizeof( struct LIB_UI_STRUCTURE ) );

	// preserve properties of already existing window
	ui -> window = window;

	// no elements by default
	ui -> limit = ui -> limit_button = ui -> limit_checkbox = ui -> limit_input = ui -> limit_label = ui -> limit_radio = EMPTY;

	// prepare area for ui elements
	ui -> button = (struct LIB_UI_STRUCTURE_ELEMENT_BUTTON **) malloc( ui -> limit_button );
	ui -> checkbox = (struct LIB_UI_STRUCTURE_ELEMENT_CHECKBOX **) malloc( ui -> limit_checkbox );
	ui -> input = (struct LIB_UI_STRUCTURE_ELEMENT_INPUT **) malloc( ui -> limit_input );
	ui -> label = (struct LIB_UI_STRUCTURE_ELEMENT_LABEL **) malloc( ui -> limit_label );
	ui -> radio = (struct LIB_UI_STRUCTURE_ELEMENT_RADIO **) malloc( ui -> limit_radio );

	// return ui properties
	return ui;
}

static void lib_ui_add( struct LIB_UI_STRUCTURE_ELEMENT *element, uint16_t x, uint16_t y, uint16_t width, uint8_t *name ) {
	element -> x = x;
	element -> y = y;
	element -> width = width;
	element -> name = (uint8_t *) malloc( lib_string_length( name ) );
	for( uint64_t i = 0; i <= lib_string_length( name ); i++ ) element -> name[ i ] = name[ i ];
}

uint64_t lib_ui_add_button( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint8_t *name, uint16_t height ) {
	ui -> button = (struct LIB_UI_STRUCTURE_ELEMENT_BUTTON **) realloc( ui -> button, sizeof( struct LIB_UI_STRUCTURE_ELEMENT_BUTTON ) * (ui -> limit_button + TRUE) );
	ui -> button[ ui -> limit_button ] = (struct LIB_UI_STRUCTURE_ELEMENT_BUTTON *) malloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_BUTTON ) );

	lib_ui_add( (struct LIB_UI_STRUCTURE_ELEMENT *) ui -> button[ ui -> limit_button ], x, y, width, name );

	ui -> button[ ui -> limit_button ] -> standard.height = height;

	if( ui -> limit_button >= ui -> limit ) ui -> limit++;

	return ui -> limit_button++;
}

uint64_t lib_ui_add_checkbox( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint8_t *name ) {
	ui -> checkbox = (struct LIB_UI_STRUCTURE_ELEMENT_CHECKBOX **) realloc( ui -> checkbox, sizeof( struct LIB_UI_STRUCTURE_ELEMENT_CHECKBOX ) * (ui -> limit_checkbox + TRUE) );
	ui -> checkbox[ ui -> limit_checkbox ] = (struct LIB_UI_STRUCTURE_ELEMENT_CHECKBOX *) malloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_CHECKBOX ) );

	lib_ui_add( (struct LIB_UI_STRUCTURE_ELEMENT *) ui -> checkbox[ ui -> limit_checkbox ], x, y, width, name );

	ui -> checkbox[ ui -> limit_checkbox ] -> standard.height = LIB_UI_CHECKBOX_HEIGHT;
	
	ui -> checkbox[ ui -> limit_checkbox ] -> set = FALSE;

	if( ui -> limit_checkbox >= ui -> limit ) ui -> limit++;

	return ui -> limit_checkbox++;
}

uint64_t lib_ui_add_input( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint8_t *name ) {
	ui -> input = (struct LIB_UI_STRUCTURE_ELEMENT_INPUT **) realloc( ui -> input, sizeof( struct LIB_UI_STRUCTURE_ELEMENT_INPUT ) * (ui -> limit_input + TRUE) );
	ui -> input[ ui -> limit_input ] = (struct LIB_UI_STRUCTURE_ELEMENT_INPUT *) malloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_INPUT ) );

	lib_ui_add( (struct LIB_UI_STRUCTURE_ELEMENT *) ui -> input[ ui -> limit_input ], x, y, width, name );

	ui -> input[ ui -> limit_input ] -> standard.height = LIB_UI_INPUT_HEIGHT;

	ui -> input[ ui -> limit_input ] -> offset = EMPTY;
	ui -> input[ ui -> limit_input ] -> index = EMPTY;

	if( ui -> limit_input >= ui -> limit ) ui -> limit++;

	return ui -> limit_input++;
}

uint64_t lib_ui_add_radio( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint8_t *name, uint8_t group ) {
	ui -> radio = (struct LIB_UI_STRUCTURE_ELEMENT_RADIO **) realloc( ui -> radio, sizeof( struct LIB_UI_STRUCTURE_ELEMENT_RADIO ) * (ui -> limit_radio + TRUE) );
	ui -> radio[ ui -> limit_radio ] = (struct LIB_UI_STRUCTURE_ELEMENT_RADIO *) malloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_RADIO ) );

	lib_ui_add( (struct LIB_UI_STRUCTURE_ELEMENT *) ui -> radio[ ui -> limit_radio ], x, y, width, name );

	ui -> radio[ ui -> limit_radio ] -> standard.height = LIB_FONT_HEIGHT_pixel;

	ui -> radio[ ui -> limit_radio ] -> set = FALSE;
	ui -> radio[ ui -> limit_radio ] -> group = group;

	if( ui -> limit_radio >= ui -> limit ) ui -> limit++;

	return ui -> limit_radio++;
}

uint64_t lib_ui_add_label( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint8_t *name ) {
	ui -> label = (struct LIB_UI_STRUCTURE_ELEMENT_LABEL **) realloc( ui -> label, sizeof( struct LIB_UI_STRUCTURE_ELEMENT_LABEL ) * (ui -> limit_label + TRUE) );
	ui -> label[ ui -> limit_label ] = (struct LIB_UI_STRUCTURE_ELEMENT_LABEL *) malloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_LABEL ) );

	lib_ui_add( (struct LIB_UI_STRUCTURE_ELEMENT *) ui -> label[ ui -> limit_label ], x, y, width, name );

	ui -> label[ ui -> limit_label ] -> standard.height = LIB_FONT_HEIGHT_pixel;

	if( ui -> limit_label >= ui -> limit ) ui -> limit++;

	return ui -> limit_label++;
}

void lib_ui_clean( struct LIB_UI_STRUCTURE *ui ) {
	lib_ui_fill_rectangle( ui -> window -> pixel, ui -> window -> width, LIB_UI_RADIUS_DEFAULT, ui -> window -> width, ui -> window -> height, LIB_UI_COLOR_BACKGROUND_DEFAULT );
}

void lib_ui_event( struct LIB_UI_STRUCTURE *ui ) {
	uint8_t flush = FALSE;
	uint8_t flush_element = FALSE;

	// incomming message
	uint8_t ipc_data[ STD_IPC_SIZE_byte ] = { EMPTY };

	// message properties
	struct STD_STRUCTURE_IPC_MOUSE *mouse = (struct STD_STRUCTURE_IPC_MOUSE *) &ipc_data;

	// receive pending messages
	std_ipc_receive_by_type( (uint8_t *) &ipc_data, STD_IPC_TYPE_mouse );

	for( uint64_t i = 0; i < ui -> limit; i++ ) {
		struct LIB_UI_STRUCTURE_ELEMENT *element;

		if( i < ui -> limit_button ) {
			flush_element = FALSE;

			element = (struct LIB_UI_STRUCTURE_ELEMENT *) ui -> button[ i ];

			if( ui -> window -> x < element -> x || ui -> window -> x > (element -> x + element -> width) || ui -> window -> y < element -> y || (ui -> window -> y > element -> y + element -> height) ) {
				if( element -> flag & LIB_UI_FLAG_hover ) {
					element -> flag &= ~LIB_UI_FLAG_hover;
					
					flush_element = TRUE;
				}
			} else {
				if( ! (element -> flag & LIB_UI_FLAG_hover) ) {
					element -> flag |= LIB_UI_FLAG_hover;
				
					flush_element = TRUE;
				}
			}

			if( flush_element ) { lib_ui_show_button( ui, i, EMPTY ); flush = TRUE; }
		}

		if( i < ui -> limit_checkbox ) {
			flush_element = FALSE;

			element = (struct LIB_UI_STRUCTURE_ELEMENT *) ui -> checkbox[ i ];

			if( ui -> window -> x < element -> x || ui -> window -> x > (element -> x + element -> width) || ui -> window -> y < element -> y || (ui -> window -> y > element -> y + element -> height) ) {
				if( element -> flag & LIB_UI_FLAG_hover ) {
					element -> flag &= ~LIB_UI_FLAG_hover;
					
					flush_element = TRUE;
				}
			} else {
				if( mouse -> button == (uint8_t) ~STD_IPC_MOUSE_BUTTON_left ) {
					element -> flag ^= LIB_UI_FLAG_set;

					flush_element = TRUE;
				}

				if( ! (element -> flag & LIB_UI_FLAG_hover) ) {
					element -> flag |= LIB_UI_FLAG_hover;
					
					flush_element = TRUE;
				}
			}

			if( flush_element ) { lib_ui_show_checkbox( ui, i, EMPTY ); flush = TRUE; }
		}

		if( i < ui -> limit_input ) {
			flush_element = FALSE;

			element = (struct LIB_UI_STRUCTURE_ELEMENT *) ui -> input[ i ];

			if( ui -> window -> x < element -> x || ui -> window -> x > (element -> x + element -> width) || ui -> window -> y < element -> y || (ui -> window -> y > element -> y + element -> height) ) {
				if( element -> flag & LIB_UI_FLAG_hover ) {
					element -> flag &= ~LIB_UI_FLAG_hover;
					
					flush_element = TRUE;
				}
			} else {
				if( ! (element -> flag & LIB_UI_FLAG_hover) ) {
					element -> flag |= LIB_UI_FLAG_hover;
					
					flush_element = TRUE;
				}
			}

			if( flush_element ) { lib_ui_show_input( ui, i, EMPTY ); flush = TRUE; }
		}

		if( i < ui -> limit_radio ) {
			flush_element = FALSE;

			element = (struct LIB_UI_STRUCTURE_ELEMENT *) ui -> radio[ i ];

			if( ui -> window -> x < element -> x || ui -> window -> x > (element -> x + element -> width) || ui -> window -> y < element -> y || (ui -> window -> y > element -> y + element -> height) ) {
				if( element -> flag & LIB_UI_FLAG_hover ) {
					element -> flag &= ~LIB_UI_FLAG_hover;
					
					flush_element = TRUE;
				}
			} else {
				if( mouse -> button == (uint8_t) ~STD_IPC_MOUSE_BUTTON_left ) {
					struct LIB_UI_STRUCTURE_ELEMENT_RADIO *radio = (struct LIB_UI_STRUCTURE_ELEMENT_RADIO *) element;
					
					// unset all elements of same group
					for( uint64_t j = 0; j < ui -> limit_radio; j++ )
						if( ui -> radio[ j ] -> group == radio -> group ) {
							ui -> radio[ j ] -> standard.flag &= ~LIB_UI_FLAG_set;
							lib_ui_show_radio( ui, j, EMPTY );
						}

					element -> flag ^= LIB_UI_FLAG_set;

					flush_element = TRUE;
				}

				if( ! (element -> flag & LIB_UI_FLAG_hover) ) {
					element -> flag |= LIB_UI_FLAG_hover;
					
					flush_element = TRUE;
				}
			}

			if( flush_element ) { lib_ui_show_radio( ui, i, EMPTY ); flush = TRUE; }
		}
	}

	// for( uint64_t i = 0; i < ui -> limit_button; i++ ) {
	// 	if( ui -> window -> x < ui -> button[ i ] -> standard.x || ui -> window -> x > (ui -> button[ i ] -> standard.x + ui -> button[ i ] -> standard.width) || ui -> window -> y < ui -> button[ i ] -> standard.y || (ui -> window -> y > ui -> button[ i ] -> standard.y + ui -> button[ i ] -> standard.height) ) {
	// 		if( ! (ui -> button[ i ] -> standard.flag & LIB_UI_FLAG_hover) ) continue;
	// 		ui -> button[ i ] -> standard.flag &= ~LIB_UI_FLAG_hover;
	// 		lib_ui_show_button( ui, i, EMPTY );
	// 		flush = TRUE;
	// 		continue;
	// 	}

	// 	if( ui -> button[ i ] -> standard.flag & LIB_UI_FLAG_hover ) continue;

	// 	ui -> button[ i ] -> standard.flag |= LIB_UI_FLAG_hover;
	// 	lib_ui_show_button( ui, i, EMPTY );
	// 	flush = TRUE;
	// }

	// for( uint64_t i = 0; i < ui -> limit_checkbox; i++ ) {
	// 	if( ui -> window -> x < ui -> checkbox[ i ] -> standard.x || ui -> window -> x > (ui -> checkbox[ i ] -> standard.x + ui -> checkbox[ i ] -> standard.width) || ui -> window -> y < ui -> checkbox[ i ] -> standard.y || (ui -> window -> y > ui -> checkbox[ i ] -> standard.y + ui -> checkbox[ i ] -> standard.height) ) {
	// 		if( ! (ui -> checkbox[ i ] -> standard.flag & LIB_UI_FLAG_hover) ) continue;
	// 		ui -> checkbox[ i ] -> standard.flag &= ~LIB_UI_FLAG_hover;
	// 		lib_ui_show_checkbox( ui, i, EMPTY );
	// 		flush = TRUE;
	// 		continue;
	// 	}

	// 	if( ui -> checkbox[ i ] -> standard.flag & LIB_UI_FLAG_hover ) continue;

	// 	ui -> checkbox[ i ] -> standard.flag |= LIB_UI_FLAG_hover;
	// 	lib_ui_show_checkbox( ui, i, EMPTY );
	// 	flush = TRUE;
	// }

	// for( uint64_t i = 0; i < ui -> limit_input; i++ ) {
	// 	if( ui -> window -> x < ui -> input[ i ] -> standard.x || ui -> window -> x > (ui -> input[ i ] -> standard.x + ui -> input[ i ] -> standard.width) || ui -> window -> y < ui -> input[ i ] -> standard.y || (ui -> window -> y > ui -> input[ i ] -> standard.y + ui -> input[ i ] -> standard.height) ) {
	// 		if( ! (ui -> input[ i ] -> standard.flag & LIB_UI_FLAG_hover) ) continue;
	// 		ui -> input[ i ] -> standard.flag &= ~LIB_UI_FLAG_hover;
	// 		lib_ui_show_input( ui, i, EMPTY );
	// 		flush = TRUE;
	// 		continue;
	// 	}

	// 	if( ui -> input[ i ] -> standard.flag & LIB_UI_FLAG_hover ) continue;

	// 	ui -> input[ i ] -> standard.flag |= LIB_UI_FLAG_hover;
	// 	lib_ui_show_input( ui, i, EMPTY );
	// 	flush = TRUE;
	// }

	// for( uint64_t i = 0; i < ui -> limit_radio; i++ ) {
	// 	if( ui -> window -> x < ui -> radio[ i ] -> standard.x || ui -> window -> x > (ui -> radio[ i ] -> standard.x + ui -> radio[ i ] -> standard.width) || ui -> window -> y < ui -> radio[ i ] -> standard.y || (ui -> window -> y > ui -> radio[ i ] -> standard.y + ui -> radio[ i ] -> standard.height) ) {
	// 		if( ! (ui -> radio[ i ] -> standard.flag & LIB_UI_FLAG_hover) ) continue;
	// 		ui -> radio[ i ] -> standard.flag &= ~LIB_UI_FLAG_hover;
	// 		lib_ui_show_radio( ui, i, EMPTY );
	// 		flush = TRUE;
	// 		continue;
	// 	}

	// 	if( ui -> radio[ i ] -> standard.flag & LIB_UI_FLAG_hover ) continue;

	// 	ui -> radio[ i ] -> standard.flag |= LIB_UI_FLAG_hover;
	// 	lib_ui_show_radio( ui, i, EMPTY );
	// 	flush = TRUE;
	// }

	if( flush ) ui -> window -> flags |= LIB_WINDOW_FLAG_flush;
}

void lib_ui_show_button( struct LIB_UI_STRUCTURE *ui, uint64_t id, uint8_t flag ) {
	// update flag state
	ui -> button[ id ] -> standard.flag ^= flag;

	// set pointer location of element inside window
	uint32_t *pixel = ui -> window -> pixel + (ui -> button[ id ] -> standard.y * ui -> window -> width) + ui -> button[ id ] -> standard.x;

	uint32_t color = LIB_UI_COLOR_BACKGROUND_BUTTON;
	if( ui -> button[ id ] -> standard.flag & LIB_UI_FLAG_hover ) color += LIB_UI_COLOR_INCREASE;
	if( ui -> button[ id ] -> standard.flag & LIB_UI_FLAG_disabled ) color = LIB_UI_COLOR_BACKGROUND_BUTTON_DISABLED;

	lib_ui_fill_rectangle( pixel, ui -> window -> width, LIB_UI_RADIUS_DEFAULT, ui -> button[ id ] -> standard.width, ui -> button[ id ] -> standard.height, color );

	if( ui -> button[ id ] -> standard.height > LIB_FONT_HEIGHT_pixel ) pixel += ((ui -> button[ id ] -> standard.height - LIB_FONT_HEIGHT_pixel) >> 1) * ui -> window -> width;

	lib_font( LIB_FONT_FAMILY_ROBOTO, ui -> button[ id ] -> standard.name, lib_string_length( ui -> button[ id ] -> standard.name ), 0xFF000000, pixel + (ui -> button[ id ] -> standard.width >> 1), ui -> window -> width, LIB_FONT_ALIGN_center );
}

void lib_ui_show_checkbox( struct LIB_UI_STRUCTURE *ui, uint64_t id, uint8_t flag ) {
	// update flag state
	ui -> checkbox[ id ] -> standard.flag ^= flag;

	// set pointer location of element inside window
	uint32_t *pixel = ui -> window -> pixel + (ui -> checkbox[ id ] -> standard.y * ui -> window -> width) + ui -> checkbox[ id ] -> standard.x;

	uint32_t color = LIB_UI_COLOR_BACKGROUND_CHECKBOX;
	if( ui -> checkbox[ id ] -> standard.flag & LIB_UI_FLAG_hover ) color += LIB_UI_COLOR_INCREASE;

	// clean area
	lib_ui_fill_rectangle( pixel, ui -> window -> width, 0, ui -> checkbox[ id ] -> standard.width, ui -> checkbox[ id ] -> standard.height, LIB_UI_COLOR_BACKGROUND_DEFAULT );

	// show checkbox
	lib_ui_fill_rectangle( pixel, ui -> window -> width, LIB_UI_RADIUS_DEFAULT, ui -> checkbox[ id ] -> standard.height, ui -> checkbox[ id ] -> standard.height, color );

	if( ui -> checkbox[ id ] -> standard.flag & LIB_UI_FLAG_set ) {
		color = LIB_UI_COLOR_CHECKBOX_SELECTED;
		if( ui -> checkbox[ id ] -> standard.flag & LIB_UI_FLAG_hover ) color += LIB_UI_COLOR_INCREASE;
		
		lib_ui_fill_rectangle( pixel + ui -> window -> width + TRUE, ui -> window -> width, LIB_UI_RADIUS_DEFAULT, ui -> checkbox[ id ] -> standard.height - 2, ui -> checkbox[ id ] -> standard.height - 2, color );
	}

	// show description
	lib_font( LIB_FONT_FAMILY_ROBOTO, ui -> checkbox[ id ] -> standard.name, lib_string_length( ui -> checkbox[ id ] -> standard.name ), LIB_UI_COLOR_DEFAULT, pixel + ui -> checkbox[ id ] -> standard.height + 4, ui -> window -> width, LIB_FONT_ALIGN_left );
}

void lib_ui_show_input( struct LIB_UI_STRUCTURE *ui, uint64_t id, uint8_t flag ) {
	// update flag state
	ui -> input[ id ] -> standard.flag ^= flag;

	// set pointer location of element inside window
	uint32_t *pixel = ui -> window -> pixel + (ui -> input[ id ] -> standard.y * ui -> window -> width) + ui -> input[ id ] -> standard.x;

	uint32_t color_background = LIB_UI_COLOR_BACKGROUND_INPUT;
	if( ui -> input[ id ] -> standard.flag & LIB_UI_FLAG_hover ) color_background += LIB_UI_COLOR_INCREASE_LIGHT;
	if( ui -> input[ id ] -> standard.flag & LIB_UI_FLAG_disabled ) color_background = LIB_UI_COLOR_BACKGROUND_INPUT_DISABLED;

	// border
	lib_ui_fill_rectangle( pixel, ui -> window -> width, LIB_UI_RADIUS_DEFAULT, ui -> input[ id ] -> standard.width, ui -> input[ id ] -> standard.height, color_background + LIB_UI_COLOR_INCREASE_LIGHT );
	// inner
	lib_ui_fill_rectangle( pixel + (ui -> window -> width) + TRUE, ui -> window -> width, LIB_UI_RADIUS_DEFAULT, ui -> input[ id ] -> standard.width - 2, ui -> input[ id ] -> standard.height - 2, color_background + LIB_UI_COLOR_INCREASE_LIGHT );

	if( ui -> input[ id ] -> standard.height > LIB_FONT_HEIGHT_pixel ) pixel += ((ui -> input[ id ] -> standard.height - LIB_FONT_HEIGHT_pixel) >> 1) * ui -> window -> width;

	uint32_t color_foreground = LIB_UI_COLOR_INPUT;
	if( ui -> input[ id ] -> standard.flag & LIB_UI_FLAG_disabled ) color_foreground = LIB_UI_COLOR_INPUT_DISABLED;

	lib_font( LIB_FONT_FAMILY_ROBOTO, ui -> input[ id ] -> standard.name, lib_string_length( ui -> input[ id ] -> standard.name ), color_foreground, pixel + LIB_UI_PADDING_DEFAULT, ui -> window -> width, LIB_FONT_ALIGN_left );
}

void lib_ui_show_label( struct LIB_UI_STRUCTURE *ui, uint64_t id, uint8_t flag ) {
	// set pointer location of element inside window
	uint32_t *pixel = ui -> window -> pixel + (ui -> label[ id ] -> standard.y * ui -> window -> width) + ui -> label[ id ] -> standard.x;

	if( ui -> label[ id ] -> standard.height > LIB_FONT_HEIGHT_pixel ) pixel += ((ui -> label[ id ] -> standard.height - LIB_FONT_HEIGHT_pixel) >> 1) * ui -> window -> width;

	// show description
	lib_font( LIB_FONT_FAMILY_ROBOTO, ui -> label[ id ] -> standard.name, lib_string_length( ui -> label[ id ] -> standard.name ), LIB_UI_COLOR_DEFAULT, pixel, ui -> window -> width, LIB_FONT_ALIGN_left );
}

void lib_ui_show_radio( struct LIB_UI_STRUCTURE *ui, uint64_t id, uint8_t flag ) {
	// set pointer location of element inside window
	uint32_t *pixel = ui -> window -> pixel + (ui -> radio[ id ] -> standard.y * ui -> window -> width) + ui -> radio[ id ] -> standard.x;

	uint32_t color = LIB_UI_COLOR_BACKGROUND_RADIO;
	if( ui -> radio[ id ] -> standard.flag & LIB_UI_FLAG_hover ) color += LIB_UI_COLOR_INCREASE;

	// clean area
	lib_ui_fill_rectangle( pixel, ui -> window -> width, 0, ui -> radio[ id ] -> standard.width, ui -> radio[ id ] -> standard.height, LIB_UI_COLOR_BACKGROUND_DEFAULT );

	// show radio
	lib_ui_fill_rectangle( pixel, ui -> window -> width, ui -> radio[ id ] -> standard.height >> STD_SHIFT_2, ui -> radio[ id ] -> standard.height, ui -> radio[ id ] -> standard.height, color );

	if( ui -> radio[ id ] -> standard.flag & LIB_UI_FLAG_set ) {
		color = LIB_UI_COLOR_RADIO_SELECTED;
		if( ui -> radio[ id ] -> standard.flag & LIB_UI_FLAG_hover ) color += LIB_UI_COLOR_INCREASE;
		
		lib_ui_fill_rectangle( pixel + ui -> window -> width + TRUE, ui -> window -> width, ui -> radio[ id ] -> standard.height >> STD_SHIFT_2, ui -> radio[ id ] -> standard.height - 2, ui -> radio[ id ] -> standard.height - 2, color );
	}

	// show description
	lib_font( LIB_FONT_FAMILY_ROBOTO, ui -> radio[ id ] -> standard.name, lib_string_length( ui -> radio[ id ] -> standard.name ), LIB_UI_COLOR_DEFAULT, pixel + ui -> radio[ id ] -> standard.height + 4, ui -> window -> width, LIB_FONT_ALIGN_left );
}
