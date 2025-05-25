/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#include	"../library/input.h"
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
	ui -> limit = ui -> limit_button = ui -> limit_checkbox = ui -> limit_control = ui -> limit_input = ui -> limit_label = ui -> limit_radio = EMPTY;

	// prepare area for element list
	ui -> element = (struct LIB_UI_STRUCTURE_ELEMENT **) malloc( ui -> limit );

	// prepare area for ui elements
	ui -> button = (struct LIB_UI_STRUCTURE_ELEMENT_BUTTON **) malloc( ui -> limit_button );
	ui -> checkbox = (struct LIB_UI_STRUCTURE_ELEMENT_CHECKBOX **) malloc( ui -> limit_checkbox );
	ui -> control = (struct LIB_UI_STRUCTURE_ELEMENT_CONTROL **) malloc( ui -> limit_control );
	ui -> input = (struct LIB_UI_STRUCTURE_ELEMENT_INPUT **) malloc( ui -> limit_input );
	ui -> label = (struct LIB_UI_STRUCTURE_ELEMENT_LABEL **) malloc( ui -> limit_label );
	ui -> radio = (struct LIB_UI_STRUCTURE_ELEMENT_RADIO **) malloc( ui -> limit_radio );

	// return ui properties
	return ui;
}

static void lib_ui_add( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT *element, uint16_t x, uint16_t y, uint16_t width, uint8_t type, uint8_t *name ) {
	element -> x		= x;
	element -> y		= y;
	element -> width	= width;
	element -> type		= type;
	element -> name		= (uint8_t *) malloc( lib_string_length( name ) ); for( uint64_t i = 0; i <= lib_string_length( name ); i++ ) element -> name[ i ] = name[ i ];

	if( ! ui -> limit ) element -> flag |= LIB_UI_ELEMENT_FLAG_active;
}

uint64_t lib_ui_add_button( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint8_t *name, uint16_t height, uint8_t flag ) {
	ui -> button = (struct LIB_UI_STRUCTURE_ELEMENT_BUTTON **) realloc( ui -> button, sizeof( struct LIB_UI_STRUCTURE_ELEMENT_BUTTON ) * (ui -> limit_button + TRUE) );
	ui -> button[ ui -> limit_button ] = (struct LIB_UI_STRUCTURE_ELEMENT_BUTTON *) malloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_BUTTON ) );

	lib_ui_add( ui, (struct LIB_UI_STRUCTURE_ELEMENT *) ui -> button[ ui -> limit_button ], x, y, width, BUTTON, name );

	ui -> button[ ui -> limit_button ] -> standard.height	= height;
	ui -> button[ ui -> limit_button ] -> standard.flag	|= flag;

	lib_ui_list_insert( ui, (struct LIB_UI_STRUCTURE_ELEMENT *) ui -> button[ ui -> limit_button ] );

	return ui -> limit_button++;
}

uint64_t lib_ui_add_checkbox( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint8_t *name, uint8_t flag ) {
	ui -> checkbox = (struct LIB_UI_STRUCTURE_ELEMENT_CHECKBOX **) realloc( ui -> checkbox, sizeof( struct LIB_UI_STRUCTURE_ELEMENT_CHECKBOX ) * (ui -> limit_checkbox + TRUE) );
	ui -> checkbox[ ui -> limit_checkbox ] = (struct LIB_UI_STRUCTURE_ELEMENT_CHECKBOX *) malloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_CHECKBOX ) );

	lib_ui_add( ui, (struct LIB_UI_STRUCTURE_ELEMENT *) ui -> checkbox[ ui -> limit_checkbox ], x, y, width, CHECKBOX, name );

	ui -> checkbox[ ui -> limit_checkbox ] -> standard.height	= LIB_UI_ELEMENT_CHECKBOX_height;
	ui -> checkbox[ ui -> limit_checkbox ] -> standard.flag		|= flag;
	
	ui -> checkbox[ ui -> limit_checkbox ] -> set = FALSE;

	lib_ui_list_insert( ui, (struct LIB_UI_STRUCTURE_ELEMENT *) ui -> checkbox[ ui -> limit_checkbox ] );

	return ui -> limit_checkbox++;
}

uint64_t lib_ui_add_control( struct LIB_UI_STRUCTURE *ui, uint8_t type ) {
	ui -> control = (struct LIB_UI_STRUCTURE_ELEMENT_CONTROL **) realloc( ui -> control, sizeof( struct LIB_UI_STRUCTURE_ELEMENT_CONTROL ) * (ui -> limit_control + TRUE) );
	ui -> control[ ui -> limit_control ] = (struct LIB_UI_STRUCTURE_ELEMENT_CONTROL *) malloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_CONTROL ) );

	ui -> control[ ui -> limit_control ] -> standard.x = ui -> window -> width - (LIB_UI_HEADER_HEIGHT * (ui -> limit_control + TRUE));
	ui -> control[ ui -> limit_control ] -> standard.y = EMPTY;
	ui -> control[ ui -> limit_control ] -> standard.width = LIB_UI_HEADER_HEIGHT;
	ui -> control[ ui -> limit_control ] -> standard.height = LIB_UI_HEADER_HEIGHT;
	ui -> control[ ui -> limit_control ] -> standard.type = CONTROL;

	ui -> control[ ui -> limit_control ] -> type = type;

	lib_ui_list_insert( ui, (struct LIB_UI_STRUCTURE_ELEMENT *) ui -> control[ ui -> limit_control ] );

	return ui -> limit_control++;
}

uint64_t lib_ui_add_input( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint8_t *name, uint8_t flag ) {
	ui -> input = (struct LIB_UI_STRUCTURE_ELEMENT_INPUT **) realloc( ui -> input, sizeof( struct LIB_UI_STRUCTURE_ELEMENT_INPUT ) * (ui -> limit_input + TRUE) );
	ui -> input[ ui -> limit_input ] = (struct LIB_UI_STRUCTURE_ELEMENT_INPUT *) malloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_INPUT ) );

	struct LIB_UI_STRUCTURE_ELEMENT_INPUT *input = ui -> input[ ui -> limit_input ];

	lib_ui_add( ui, (struct LIB_UI_STRUCTURE_ELEMENT *) input, x, y, width, INPUT, name );

	input -> standard.height	= LIB_UI_ELEMENT_INPUT_height;
	input -> standard.flag		|= flag;

	if( lib_string_length( input -> standard.name ) < LIB_UI_ELEMENT_INPUT_length_max ) input -> standard.name = (uint8_t *) realloc( input -> standard.name, LIB_UI_ELEMENT_INPUT_length_max );
	else input -> standard.name[ LIB_UI_ELEMENT_INPUT_length_max ] = STD_ASCII_TERMINATOR;

	input -> offset = EMPTY;
	input -> index = EMPTY;

	lib_ui_list_insert( ui, (struct LIB_UI_STRUCTURE_ELEMENT *) input );

	return ui -> limit_input++;
}

uint64_t lib_ui_add_label( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint8_t *name ) {
	ui -> label = (struct LIB_UI_STRUCTURE_ELEMENT_LABEL **) realloc( ui -> label, sizeof( struct LIB_UI_STRUCTURE_ELEMENT_LABEL ) * (ui -> limit_label + TRUE) );
	ui -> label[ ui -> limit_label ] = (struct LIB_UI_STRUCTURE_ELEMENT_LABEL *) malloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_LABEL ) );

	lib_ui_add( ui, (struct LIB_UI_STRUCTURE_ELEMENT *) ui -> label[ ui -> limit_label ], x, y, width, LABEL, name );

	ui -> label[ ui -> limit_label ] -> standard.height = LIB_FONT_HEIGHT_pixel;

	return ui -> limit_label++;
}

uint64_t lib_ui_add_radio( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint8_t *name, uint8_t group, uint8_t flag ) {
	ui -> radio = (struct LIB_UI_STRUCTURE_ELEMENT_RADIO **) realloc( ui -> radio, sizeof( struct LIB_UI_STRUCTURE_ELEMENT_RADIO ) * (ui -> limit_radio + TRUE) );
	ui -> radio[ ui -> limit_radio ] = (struct LIB_UI_STRUCTURE_ELEMENT_RADIO *) malloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_RADIO ) );

	lib_ui_add( ui, (struct LIB_UI_STRUCTURE_ELEMENT *) ui -> radio[ ui -> limit_radio ], x, y, width, RADIO, name );

	ui -> radio[ ui -> limit_radio ] -> standard.height	= LIB_FONT_HEIGHT_pixel;
	ui -> radio[ ui -> limit_radio ] -> standard.flag	|= flag;

	ui -> radio[ ui -> limit_radio ] -> set = FALSE;
	ui -> radio[ ui -> limit_radio ] -> group = group;

	lib_ui_list_insert( ui, (struct LIB_UI_STRUCTURE_ELEMENT *) ui -> radio[ ui -> limit_radio ] );

	return ui -> limit_radio++;
}

void lib_ui_clean( struct LIB_UI_STRUCTURE *ui ) {
	lib_ui_fill_rectangle( ui -> window -> pixel, ui -> window -> width, LIB_UI_RADIUS_DEFAULT, ui -> window -> width, ui -> window -> height, LIB_UI_COLOR_BACKGROUND_DEFAULT );
}

void lib_ui_event( struct LIB_UI_STRUCTURE *ui ) {
	uint8_t sync = FALSE;

	lib_ui_event_mouse( ui, (uint8_t *) &sync );
	lib_ui_event_keyboard( ui, (uint8_t *) &sync );

	if( sync ) ui -> window -> flags |= LIB_WINDOW_FLAG_flush;
}

static void lib_ui_event_keyboard( struct LIB_UI_STRUCTURE *ui, uint8_t *sync ) {
	uint8_t ipc_data[ STD_IPC_SIZE_byte ] = { EMPTY };

	if( ! std_ipc_receive_by_type( (uint8_t *) &ipc_data, STD_IPC_TYPE_keyboard ) ) return;

	struct STD_STRUCTURE_IPC_KEYBOARD *keyboard = (struct STD_STRUCTURE_IPC_KEYBOARD *) &ipc_data;

	if( keyboard -> key == STD_KEY_ALT_LEFT ) ui -> keyboard.semaphore_alt_left = TRUE;
	if( keyboard -> key == (STD_KEY_ALT_LEFT | STD_KEY_RELEASE) ) ui -> keyboard.semaphore_alt_left = FALSE;

	if( keyboard -> key == STD_KEY_CTRL_LEFT ) ui -> keyboard.semaphore_ctrl_left = TRUE;
	if( keyboard -> key == (STD_KEY_CTRL_LEFT | STD_KEY_RELEASE) ) ui -> keyboard.semaphore_ctrl_left = FALSE;

	if( keyboard -> key == STD_KEY_SHIFT_LEFT ) ui -> keyboard.semaphore_shift = TRUE;
	if( keyboard -> key == (STD_KEY_SHIFT_LEFT | STD_KEY_RELEASE) ) ui -> keyboard.semaphore_shift = FALSE;

	if( keyboard -> key == STD_KEY_TAB ) {
		ui -> element[ ui -> element_active ] -> flag &= ~LIB_UI_ELEMENT_FLAG_active;

		lib_ui_show_element( ui, ui -> element[ ui -> element_active ] );
		
		if( ui -> keyboard.semaphore_shift ) {
			if( ! ui -> element_active ) ui -> element_active = ui -> limit - TRUE;
			else ui -> element_active--;
		} else ui -> element_active++;

		if( ui -> element_active == ui -> limit ) ui -> element_active = EMPTY;

		ui -> element[ ui -> element_active ] -> flag |= LIB_UI_ELEMENT_FLAG_active;

		lib_ui_show_element( ui, ui -> element[ ui -> element_active ] );

		*sync = TRUE;
	}

	if( keyboard -> key == STD_KEY_SPACE ) {
		if( ui -> element[ ui -> element_active ] -> type == RADIO ) {
			struct LIB_UI_STRUCTURE_ELEMENT_RADIO *radio = (struct LIB_UI_STRUCTURE_ELEMENT_RADIO *) ui -> element[ ui -> element_active ];
			for( uint64_t j = 0; j < ui -> limit_radio; j++ )
				if( ui -> radio[ j ] -> group == radio -> group ) {
					ui -> radio[ j ] -> standard.flag &= ~LIB_UI_ELEMENT_FLAG_set;
					lib_ui_show_element( ui, (struct LIB_UI_STRUCTURE_ELEMENT *) ui -> radio[ j ] );
				}
		}

		ui -> element[ ui -> element_active ] -> flag ^= LIB_UI_ELEMENT_FLAG_set;

		lib_ui_show_element( ui, ui -> element[ ui -> element_active ] );

		*sync = TRUE;
	}

	if( ui -> element[ ui -> element_active ] -> type == INPUT && ! (ui -> element[ ui -> element_active ] -> flag & LIB_UI_ELEMENT_FLAG_disabled) ) {
		struct LIB_UI_STRUCTURE_ELEMENT_INPUT *input = (struct LIB_UI_STRUCTURE_ELEMENT_INPUT *) ui -> element[ ui -> element_active ];

		input -> index = lib_input( input -> standard.name, LIB_UI_ELEMENT_INPUT_length_max, input -> index, keyboard -> key, ui -> keyboard.semaphore_ctrl_left );

		while( input -> offset > input -> index ) input -> offset--;
		uint64_t length = input -> index - input -> offset;
		while( lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, input -> standard.name + input -> offset, length-- ) > (input -> standard.width - LIB_UI_PADDING_DEFAULT) ) input -> offset++;

		lib_ui_show_input( ui, input );

		*sync = TRUE;
	}
}

static void lib_ui_event_mouse( struct LIB_UI_STRUCTURE *ui, uint8_t *sync ) {
	uint8_t flush_element = FALSE;

	// incomming message
	uint8_t ipc_data[ STD_IPC_SIZE_byte ] = { EMPTY };

	// message properties
	struct STD_STRUCTURE_IPC_MOUSE *mouse = (struct STD_STRUCTURE_IPC_MOUSE *) &ipc_data;

	// receive pending messages of mouse
	std_ipc_receive_by_type( (uint8_t *) &ipc_data, STD_IPC_TYPE_mouse );

	for( uint64_t i = 0; i < ui -> limit; i++ ) {
		uint8_t flush_element = FALSE;

		if( ui -> window -> x < ui -> element[ i ] -> x || ui -> window -> x > (ui -> element[ i ] -> x + ui -> element[ i ] -> width) || ui -> window -> y < ui -> element[ i ] -> y || (ui -> window -> y > ui -> element[ i ] -> y + ui -> element[ i ] -> height) ) {
			if( ui -> element[ i ] -> flag & LIB_UI_ELEMENT_FLAG_hover ) {
				ui -> element[ i ] -> flag ^= LIB_UI_ELEMENT_FLAG_hover;
				
				flush_element = TRUE;
			}
		} else {
			if( ui -> element[ i ] -> type == CONTROL ) {
				struct LIB_UI_STRUCTURE_ELEMENT_CONTROL *control = (struct LIB_UI_STRUCTURE_ELEMENT_CONTROL *) ui -> element[ i ];
				if( control -> type == LIB_UI_ELEMENT_CONTROL_TYPE_min && mouse -> button == STD_IPC_MOUSE_BUTTON_left ) ui -> window -> flags |= LIB_WINDOW_FLAG_hide;
				if( control -> type == LIB_UI_ELEMENT_CONTROL_TYPE_close && mouse -> button & ~STD_IPC_MOUSE_BUTTON_left ) exit();
			}

			if( mouse -> button == STD_IPC_MOUSE_BUTTON_left ) {
				ui -> element[ ui -> element_active ] -> flag &= ~LIB_UI_ELEMENT_FLAG_active;

				lib_ui_show_element( ui, ui -> element[ ui -> element_active ] );
				
				for( uint64_t j = 0; j < ui -> limit; j++ ) if( ui -> element[ i ] == ui -> element[ j ] ) ui -> element_active = j;

				*sync = TRUE;
			}

			if( mouse -> button == (uint8_t) ~STD_IPC_MOUSE_BUTTON_left ) {
				if( ui -> element[ i ] -> type == RADIO ) {
					struct LIB_UI_STRUCTURE_ELEMENT_RADIO *radio = (struct LIB_UI_STRUCTURE_ELEMENT_RADIO *) ui -> element[ i ];
					for( uint64_t j = 0; j < ui -> limit_radio; j++ )
						if( ui -> radio[ j ] -> group == radio -> group ) {
							ui -> radio[ j ] -> standard.flag &= ~LIB_UI_ELEMENT_FLAG_set;
							lib_ui_show_element( ui, (struct LIB_UI_STRUCTURE_ELEMENT *) ui -> radio[ j ] );
						}
				}

				ui -> element[ i ] -> flag ^= LIB_UI_ELEMENT_FLAG_set;

				flush_element = TRUE;
			}

			if( ! (ui -> element[ i ] -> flag & LIB_UI_ELEMENT_FLAG_hover) ) {
				ui -> element[ i ] -> flag ^= LIB_UI_ELEMENT_FLAG_hover;

				flush_element = TRUE;
			}
		}

		if( flush_element ) { lib_ui_show_element( ui, ui -> element[ i ] ); *sync = TRUE; }
	}
}

void lib_ui_flush( struct LIB_UI_STRUCTURE *ui ) {
	lib_ui_clean( ui );

	lib_ui_show_name( ui );

	for( uint64_t i = 0; i < ui -> limit; i++ )
		lib_ui_show_element( ui, ui -> element[ i ] );

	for( uint64_t i = 0; i < ui -> limit_label; i++ )
		lib_ui_show_label( ui, (struct LIB_UI_STRUCTURE_ELEMENT_LABEL *) ui -> label[ i ] );
}

static void lib_ui_list_insert( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT *element ) {
	ui -> element = (struct LIB_UI_STRUCTURE_ELEMENT **) realloc( ui -> element, sizeof( struct LIB_UI_STRUCTURE_ELEMENT ) * (ui -> limit + TRUE) );

	ui -> element[ ui -> limit++ ] = element;
}

void lib_ui_show_button( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT_BUTTON *button ) {
	// set pointer location of element inside window
	uint32_t *pixel = ui -> window -> pixel + (button -> standard.y * ui -> window -> width) + button -> standard.x;

	uint32_t color_background = LIB_UI_COLOR_BACKGROUND_BUTTON;
	if( button -> standard.flag & LIB_UI_ELEMENT_FLAG_active ) color_background += LIB_UI_COLOR_INCREASE;
	if( button -> standard.flag & LIB_UI_ELEMENT_FLAG_hover ) color_background += LIB_UI_COLOR_INCREASE;
	if( button -> standard.flag & LIB_UI_ELEMENT_FLAG_disabled ) color_background = LIB_UI_COLOR_BACKGROUND_BUTTON_DISABLED;

	lib_ui_fill_rectangle( pixel + ui -> window -> width, ui -> window -> width, LIB_UI_RADIUS_DEFAULT, button -> standard.width, button -> standard.height, LIB_UI_COLOR_BACKGROUND_SHADOW );
	lib_ui_fill_rectangle( pixel, ui -> window -> width, LIB_UI_RADIUS_DEFAULT, button -> standard.width, button -> standard.height, color_background );

	if( button -> standard.height > LIB_FONT_HEIGHT_pixel ) pixel += ((button -> standard.height - LIB_FONT_HEIGHT_pixel) >> 1) * ui -> window -> width;

	lib_font( LIB_FONT_FAMILY_ROBOTO, button -> standard.name, lib_string_length( button -> standard.name ), 0xFF000000, pixel + (button -> standard.width >> 1), ui -> window -> width, LIB_FONT_ALIGN_center );
}

void lib_ui_show_checkbox( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT_CHECKBOX *checkbox ) {
	// set pointer location of element inside window
	uint32_t *pixel = ui -> window -> pixel + (checkbox -> standard.y * ui -> window -> width) + checkbox -> standard.x;

	uint32_t color_background = LIB_UI_COLOR_BACKGROUND_CHECKBOX;
	if( checkbox -> standard.flag & LIB_UI_ELEMENT_FLAG_set ) color_background = LIB_UI_COLOR_CHECKBOX_SELECTED;
	if( checkbox -> standard.flag & LIB_UI_ELEMENT_FLAG_active ) color_background += LIB_UI_COLOR_INCREASE;
	if( checkbox -> standard.flag & LIB_UI_ELEMENT_FLAG_hover ) color_background += LIB_UI_COLOR_INCREASE;

	// clean area
	lib_ui_fill_rectangle( pixel, ui -> window -> width, LIB_UI_RADIUS_DEFAULT, checkbox -> standard.width, checkbox -> standard.height, LIB_UI_COLOR_BACKGROUND_DEFAULT );

	// show checkbox
	lib_ui_fill_rectangle( pixel + ui -> window -> width, ui -> window -> width, LIB_UI_RADIUS_DEFAULT, checkbox -> standard.height, checkbox -> standard.height, LIB_UI_COLOR_BACKGROUND_SHADOW );
	lib_ui_fill_rectangle( pixel, ui -> window -> width, LIB_UI_RADIUS_DEFAULT, checkbox -> standard.height, checkbox -> standard.height, color_background );

	// show description
	lib_font( LIB_FONT_FAMILY_ROBOTO, checkbox -> standard.name, lib_string_length( checkbox -> standard.name ), LIB_UI_COLOR_DEFAULT, pixel + checkbox -> standard.height + 4, ui -> window -> width, LIB_FONT_ALIGN_left );
}

void lib_ui_show_control( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT_CONTROL *control ) {
	// set pointer location of element inside window
	uint32_t *pixel = ui -> window -> pixel + control -> standard.x;

	uint64_t color_foreground = STD_COLOR_WHITE;
	uint32_t color_background = LIB_UI_COLOR_BACKGROUND_CONTROL_DEFAULT;
	if( control -> standard.flag & LIB_UI_ELEMENT_FLAG_active ) color_background += LIB_UI_COLOR_INCREASE;
	if( control -> standard.flag & LIB_UI_ELEMENT_FLAG_hover ) {
		color_foreground = STD_COLOR_WHITE;

		switch( control -> type ) {
			case LIB_UI_ELEMENT_CONTROL_TYPE_close: { color_foreground = STD_COLOR_BLACK; color_background = LIB_UI_COLOR_BACKGROUND_CONTROL_CLOSE; break; }
			default: { color_background = LIB_UI_COLOR_BACKGROUND_CONTROL_DEFAULT + LIB_UI_COLOR_INCREASE; break; }
		}
	}

	pixel += (ui -> window -> width * ((LIB_UI_HEADER_HEIGHT - LIB_UI_ELEMENT_LABEL_height) >> TRUE) + ((LIB_UI_HEADER_HEIGHT - LIB_UI_ELEMENT_LABEL_height) >> TRUE));
	lib_ui_fill_rectangle( pixel + ui -> window -> width, ui -> window -> width, LIB_UI_ELEMENT_LABEL_height >> TRUE, LIB_UI_ELEMENT_LABEL_height, LIB_UI_ELEMENT_LABEL_height, LIB_UI_COLOR_BACKGROUND_SHADOW );
	lib_ui_fill_rectangle( pixel, ui -> window -> width, LIB_UI_ELEMENT_LABEL_height >> TRUE, LIB_UI_ELEMENT_LABEL_height, LIB_UI_ELEMENT_LABEL_height, color_background );

	switch( control -> type ) {
		case LIB_UI_ELEMENT_CONTROL_TYPE_close: {
			for( uint64_t i = 6; i <= LIB_UI_ELEMENT_LABEL_height - 6; i++ ) {
				pixel[ (i * ui -> window -> width) + i ] = color_foreground;
				pixel[ (LIB_UI_ELEMENT_LABEL_height - i) + (i * ui -> window -> width) ] = color_foreground;
			}

			break;
		}

		case LIB_UI_ELEMENT_CONTROL_TYPE_max: {
			for( uint64_t i = 6; i <= LIB_UI_ELEMENT_LABEL_height - 6; i++ ) {
				pixel[ (6 * ui -> window -> width) + i ] = color_foreground;
				pixel[ (i * ui -> window -> width) + 6 ] = color_foreground;
				pixel[ (i * ui -> window -> width) + (LIB_UI_ELEMENT_LABEL_height - 6) ] = color_foreground;
				pixel[ ((LIB_UI_ELEMENT_LABEL_height - 6) * ui -> window -> width) + i ] = color_foreground;
			}

			break;
		}

		case LIB_UI_ELEMENT_CONTROL_TYPE_min: {
			for( uint64_t i = 6; i <= LIB_UI_ELEMENT_LABEL_height - 6; i++ ) pixel[ ((LIB_UI_ELEMENT_LABEL_height - 6) * ui -> window -> width) + i ] = color_foreground;
		}
	}
}

void lib_ui_show_element( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT *element ) {
	switch( element -> type ) {
		case BUTTON:	{ lib_ui_show_button( ui, (struct LIB_UI_STRUCTURE_ELEMENT_BUTTON *) element ); break; }
		case CHECKBOX:	{ lib_ui_show_checkbox( ui, (struct LIB_UI_STRUCTURE_ELEMENT_CHECKBOX *) element ); break; }
		case CONTROL:	{ lib_ui_show_control( ui, (struct LIB_UI_STRUCTURE_ELEMENT_CONTROL *) element ); break; }
		case INPUT:	{ lib_ui_show_input( ui, (struct LIB_UI_STRUCTURE_ELEMENT_INPUT *) element ); break; }
		case RADIO:	{ lib_ui_show_radio( ui, (struct LIB_UI_STRUCTURE_ELEMENT_RADIO *) element ); break; }
		default: {
			// nothing
		}
	}
}

void lib_ui_show_input( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT_INPUT *input ) {
	// set pointer location of element inside window
	uint32_t *pixel = ui -> window -> pixel + (input -> standard.y * ui -> window -> width) + input -> standard.x;

	uint32_t color_background = LIB_UI_COLOR_BACKGROUND_INPUT;
	if( input -> standard.flag & LIB_UI_ELEMENT_FLAG_active ) color_background += LIB_UI_COLOR_INCREASE;
	if( input -> standard.flag & LIB_UI_ELEMENT_FLAG_hover ) color_background += LIB_UI_COLOR_INCREASE_LIGHT;
	if( input -> standard.flag & LIB_UI_ELEMENT_FLAG_disabled ) color_background = LIB_UI_COLOR_BACKGROUND_INPUT_DISABLED;

	// border
	lib_ui_fill_rectangle( pixel, ui -> window -> width, LIB_UI_RADIUS_DEFAULT, input -> standard.width, input -> standard.height, color_background + LIB_UI_COLOR_INCREASE_LIGHT );
	// inner
	lib_ui_fill_rectangle( pixel + (ui -> window -> width) + TRUE, ui -> window -> width, LIB_UI_RADIUS_DEFAULT, input -> standard.width - 2, input -> standard.height - 2, color_background + LIB_UI_COLOR_INCREASE_LIGHT );

	if( input -> standard.height > LIB_FONT_HEIGHT_pixel ) pixel += ((input -> standard.height - LIB_FONT_HEIGHT_pixel) >> 1) * ui -> window -> width;

	uint32_t color_foreground = LIB_UI_COLOR_INPUT;
	if( input -> standard.flag & LIB_UI_ELEMENT_FLAG_disabled ) color_foreground = LIB_UI_COLOR_INPUT_DISABLED;

	uint64_t name_length_max = lib_string_length( input -> standard.name ) - input -> offset;
	while( lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, input -> standard.name + input -> offset, name_length_max ) > input -> standard.width - LIB_UI_PADDING_DEFAULT ) { if( ! --name_length_max ) break; }

	if( name_length_max ) lib_font( LIB_FONT_FAMILY_ROBOTO_MONO, input -> standard.name + input -> offset, name_length_max, color_foreground, pixel + LIB_UI_PADDING_DEFAULT, ui -> window -> width, LIB_FONT_ALIGN_left );

	if( ! (input -> standard.flag & LIB_UI_ELEMENT_FLAG_active) || input -> standard.flag & LIB_UI_ELEMENT_FLAG_disabled ) return;

	uint64_t x = lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, input -> standard.name + input -> offset, input -> index - input -> offset );
	for( uint64_t y = 0; y < LIB_FONT_HEIGHT_pixel; y++ )
		pixel[ (y * ui -> window -> width) + x + LIB_UI_PADDING_DEFAULT ] = STD_COLOR_WHITE;
}

void lib_ui_show_label( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT_LABEL *label ) {
	// set pointer location of element inside window
	uint32_t *pixel = ui -> window -> pixel + (label -> standard.y * ui -> window -> width) + label -> standard.x;

	if( label -> standard.height > LIB_FONT_HEIGHT_pixel ) pixel += ((label -> standard.height - LIB_FONT_HEIGHT_pixel) >> 1) * ui -> window -> width;

	// show description
	lib_font( LIB_FONT_FAMILY_ROBOTO, label -> standard.name, lib_string_length( label -> standard.name ), LIB_UI_COLOR_DEFAULT, pixel, ui -> window -> width, LIB_FONT_ALIGN_left );
}

void lib_ui_show_name( struct LIB_UI_STRUCTURE *ui ) {
	// set pointer location of element inside window
	uint32_t *pixel = ui -> window -> pixel + (((LIB_UI_HEADER_HEIGHT - LIB_FONT_HEIGHT_pixel) >> TRUE) * ui -> window -> width) + LIB_UI_PADDING_DEFAULT;

	// clean area
	lib_ui_fill_rectangle( pixel, ui -> window -> width, 0, ui -> window -> width - (LIB_UI_PADDING_DEFAULT << 1) - (ui -> limit_control * LIB_UI_HEADER_HEIGHT), LIB_FONT_HEIGHT_pixel, LIB_UI_COLOR_BACKGROUND_DEFAULT );

	// show description
	lib_font( LIB_FONT_FAMILY_ROBOTO, ui -> window -> name, ui -> window -> name_length, LIB_UI_COLOR_DEFAULT, pixel, ui -> window -> width, LIB_FONT_ALIGN_left );
}

void lib_ui_show_radio( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT_RADIO *radio ) {
	// set pointer location of element inside window
	uint32_t *pixel = ui -> window -> pixel + (radio -> standard.y * ui -> window -> width) + radio -> standard.x;

	uint32_t color_background = LIB_UI_COLOR_BACKGROUND_RADIO;
	if( radio -> standard.flag & LIB_UI_ELEMENT_FLAG_set ) color_background = LIB_UI_COLOR_RADIO_SELECTED;
	if( radio -> standard.flag & LIB_UI_ELEMENT_FLAG_active ) color_background += LIB_UI_COLOR_INCREASE;
	if( radio -> standard.flag & LIB_UI_ELEMENT_FLAG_hover ) color_background += LIB_UI_COLOR_INCREASE;

	// clean area
	lib_ui_fill_rectangle( pixel, ui -> window -> width, radio -> standard.height >> STD_SHIFT_2, radio -> standard.width, radio -> standard.height, LIB_UI_COLOR_BACKGROUND_DEFAULT );

	// show radio
	lib_ui_fill_rectangle( pixel + ui -> window -> width, ui -> window -> width, radio -> standard.height >> STD_SHIFT_2, radio -> standard.height, radio -> standard.height, LIB_UI_COLOR_BACKGROUND_SHADOW );
	lib_ui_fill_rectangle( pixel, ui -> window -> width, radio -> standard.height >> STD_SHIFT_2, radio -> standard.height, radio -> standard.height, color_background );

	// show description
	lib_font( LIB_FONT_FAMILY_ROBOTO, radio -> standard.name, lib_string_length( radio -> standard.name ), LIB_UI_COLOR_DEFAULT, pixel + radio -> standard.height + 4, ui -> window -> width, LIB_FONT_ALIGN_left );
}
