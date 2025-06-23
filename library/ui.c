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

	// set minimal dimension of window
	ui -> window -> width_minimal = ui -> window -> current_width;
	ui -> window -> height_minimal = ui -> window -> current_height;

	// no elements by default
	ui -> limit = ui -> limit_button = ui -> limit_checkbox = ui -> limit_control = ui -> limit_input = ui -> limit_label = ui -> limit_radio = ui -> limit_table = ui -> limit_textarea = EMPTY;

	// prepare area for element list
	ui -> element = (struct LIB_UI_STRUCTURE_ELEMENT **) malloc( ui -> limit );

	// prepare area for all ui elements
	ui -> button = (struct LIB_UI_STRUCTURE_ELEMENT_BUTTON **) malloc( ui -> limit_button );
	ui -> checkbox = (struct LIB_UI_STRUCTURE_ELEMENT_CHECKBOX **) malloc( ui -> limit_checkbox );
	ui -> control = (struct LIB_UI_STRUCTURE_ELEMENT_CONTROL **) malloc( ui -> limit_control );
	ui -> input = (struct LIB_UI_STRUCTURE_ELEMENT_INPUT **) malloc( ui -> limit_input );
	ui -> label = (struct LIB_UI_STRUCTURE_ELEMENT_LABEL **) malloc( ui -> limit_label );
	ui -> radio = (struct LIB_UI_STRUCTURE_ELEMENT_RADIO **) malloc( ui -> limit_radio );
	ui -> table = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE **) malloc( ui -> limit_table );
	ui -> textarea = (struct LIB_UI_STRUCTURE_ELEMENT_TEXTAREA **) malloc( ui -> limit_textarea );

	// clean up window area
	lib_ui_clean( ui );

	// return ui properties
	return ui;
}

static void lib_ui_add( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT *element, uint16_t x, uint16_t y, uint16_t width, uint8_t type, uint8_t *name ) {
	element -> x			= x;
	element -> y			= y;
	element -> width		= width;
	element -> type			= type;
	if( name ) element -> name	= (uint8_t *) calloc( lib_string_length( name ) + TRUE ); for( uint64_t i = 0; i < lib_string_length( name ); i++ ) element -> name[ i ] = name[ i ];

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

	ui -> control[ ui -> limit_control ] -> standard.x = LIB_UI_HEADER_HEIGHT * (ui -> limit_control + TRUE);
	ui -> control[ ui -> limit_control ] -> standard.y = EMPTY;
	ui -> control[ ui -> limit_control ] -> standard.width = LIB_UI_HEADER_HEIGHT;
	ui -> control[ ui -> limit_control ] -> standard.height = LIB_UI_HEADER_HEIGHT;
	ui -> control[ ui -> limit_control ] -> standard.type = CONTROL;

	ui -> control[ ui -> limit_control ] -> type = type;

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

uint64_t lib_ui_add_table( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t flag, struct LIB_UI_STRUCTURE_ELEMENT_TABLE_HEADER *header, struct LIB_UI_STRUCTURE_ELEMENT_TABLE_ROW *row, uint64_t c, uint64_t r ) {
	ui -> table = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE **) realloc( ui -> table, sizeof( struct LIB_UI_STRUCTURE_ELEMENT_TABLE ) * (ui -> limit_table + TRUE) );
	ui -> table[ ui -> limit_table ] = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE *) malloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_TABLE ) );

	ui -> table[ ui -> limit_table ] -> standard.x		= x;
	ui -> table[ ui -> limit_table ] -> standard.y		= y;
	ui -> table[ ui -> limit_table ] -> standard.width	= width;
	ui -> table[ ui -> limit_table ] -> standard.height	= height;
	ui -> table[ ui -> limit_table ] -> standard.type	= TABLE;
	ui -> table[ ui -> limit_table ] -> standard.flag	|= flag;

	ui -> table[ ui -> limit_table ] -> limit_column	= c;
	ui -> table[ ui -> limit_table ] -> limit_row		= r;
	ui -> table[ ui -> limit_table ] -> header		= header;
	ui -> table[ ui -> limit_table ] -> row			= row;
	ui -> table[ ui -> limit_table ] -> pixel		= (uint32_t *) malloc( FALSE );

	lib_ui_list_insert( ui, (struct LIB_UI_STRUCTURE_ELEMENT *) ui -> table[ ui -> limit_table ] );

	return ui -> limit_table++;
}

uint64_t lib_ui_add_textarea( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint64_t height, uint8_t flag, uint8_t *string ) {
	ui -> textarea = (struct LIB_UI_STRUCTURE_ELEMENT_TEXTAREA **) realloc( ui -> textarea, sizeof( struct LIB_UI_STRUCTURE_ELEMENT_TEXTAREA ) * (ui -> limit_textarea + TRUE) );
	ui -> textarea[ ui -> limit_textarea ] = (struct LIB_UI_STRUCTURE_ELEMENT_TEXTAREA *) malloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_TEXTAREA ) );

	struct LIB_UI_STRUCTURE_ELEMENT_TEXTAREA *textarea = ui -> textarea[ ui -> limit_textarea ];

	// lib_ui_add( ui, (struct LIB_UI_STRUCTURE_ELEMENT *) textarea, x, y, width, TEXTAREA, EMPTY );

	textarea -> standard.x		= x;
	textarea -> standard.y		= y;
	textarea -> standard.width	= width;
	textarea -> standard.height	= height;
	textarea -> standard.type	= TEXTAREA;
	textarea -> standard.flag	|= LIB_UI_ELEMENT_FLAG_active;

	textarea -> string		= string;

	lib_ui_list_insert( ui, (struct LIB_UI_STRUCTURE_ELEMENT *) textarea );

	return ui -> limit_textarea++;
}

void lib_ui_clean( struct LIB_UI_STRUCTURE *ui ) {
	lib_ui_fill_rectangle( ui -> window -> pixel, ui -> window -> current_width, LIB_UI_RADIUS_DEFAULT, ui -> window -> current_width, ui -> window -> current_height, LIB_UI_COLOR_BACKGROUND_DEFAULT );
}

void lib_ui_update_table( struct LIB_UI_STRUCTURE *ui, uint64_t id, struct LIB_UI_STRUCTURE_ELEMENT_TABLE_ROW *row, uint64_t r ) {
	ui -> table[ id ] -> limit_row = r;
	ui -> table[ id ] -> row = row;

	ui -> table[ id ] -> offset_x = EMPTY;
	ui -> table[ id ] -> offset_y = EMPTY;
}

void lib_ui_event( struct LIB_UI_STRUCTURE *ui ) {
	uint8_t sync = FALSE;

	struct LIB_WINDOW_STRUCTURE *new = EMPTY;
	if( (new = lib_window_event( ui -> window )) ) {
		ui -> window = new;

		lib_ui_flush( ui );

		ui -> window -> flags |= LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_flush;
	}

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
			switch( ui -> element[ i ] -> type ) {
				default: {
					if( ui -> element[ i ] -> flag & LIB_UI_ELEMENT_FLAG_hover ) {
						ui -> element[ i ] -> flag ^= LIB_UI_ELEMENT_FLAG_hover;
				
						flush_element = TRUE;
					}
				}
			}
		} else {
			if( mouse -> button == STD_IPC_MOUSE_BUTTON_left ) {
				switch( ui -> element[ i ] -> type ) {
					case TABLE: {
						struct LIB_UI_STRUCTURE_ELEMENT_TABLE *table = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE *) ui -> element[ i ];

						if( ui -> window -> y - table -> standard.y < LIB_UI_ELEMENT_TABLE_height ) {
							// sort by column
						} else {
							uint64_t r = (((ui -> window -> y + table -> offset_y) - table -> standard.y) / LIB_UI_ELEMENT_TABLE_height) - TRUE;

							uint16_t table_height = table -> standard.height; if( table -> standard.height == (uint16_t) STD_MAX_unsigned ) table_height = ui -> window -> current_height - table -> standard.y - LIB_UI_MARGIN_DEFAULT;

							if( ! ((r + 1) * LIB_UI_ELEMENT_TABLE_height - table -> offset_y < (table_height - LIB_UI_ELEMENT_TABLE_height)) ) table -> offset_y = ((r + 1) * LIB_UI_ELEMENT_TABLE_height) - (table_height - LIB_UI_ELEMENT_TABLE_height);
							else if( r * LIB_UI_ELEMENT_TABLE_height < table -> offset_y ) table -> offset_y = r * LIB_UI_ELEMENT_TABLE_height;

							if( std_microtime() - ui -> microtime < LIB_UI_LATENCY_microtime )
								if( table -> row[ r ].flag & LIB_UI_ELEMENT_FLAG_set ) {
									table -> row[ r ].flag |= LIB_UI_ELEMENT_FLAG_event;
								}

							if( ! ui -> keyboard.semaphore_ctrl_left ) for( uint64_t y = 0; y < table -> limit_row; y++ ) table -> row[ y ].flag &= ~LIB_UI_ELEMENT_FLAG_set;

							table -> row[ r ].flag ^= LIB_UI_ELEMENT_FLAG_set;
						}

						flush_element = TRUE;

						break;
					}

					default: {
						ui -> element[ ui -> element_active ] -> flag &= ~LIB_UI_ELEMENT_FLAG_active;

						lib_ui_show_element( ui, ui -> element[ ui -> element_active ] );
					
						for( uint64_t j = 0; j < ui -> limit; j++ ) if( ui -> element[ i ] == ui -> element[ j ] ) ui -> element_active = j;

						ui -> element[ ui -> element_active ] -> flag |= LIB_UI_ELEMENT_FLAG_active;

						*sync = TRUE;
					}
				}

				ui -> microtime = std_microtime();
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

			switch( ui -> element[ i ] -> type ) {
				case TABLE: {
					struct LIB_UI_STRUCTURE_ELEMENT_TABLE *table = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE *) ui -> element[ i ];

					uint64_t r = (((ui -> window -> y + table -> offset_y) - table -> standard.y) / LIB_UI_ELEMENT_TABLE_height) - TRUE;

					if( ! (table -> row[ r ].flag & LIB_UI_ELEMENT_FLAG_hover) ) {
						for( uint64_t y = 0; y < table -> limit_row; y++ ) table -> row[ y ].flag &= ~LIB_UI_ELEMENT_FLAG_hover;

						table -> row[ r ].flag |= LIB_UI_ELEMENT_FLAG_hover;

						flush_element = TRUE;
					}

					if( ui -> window -> z ) {
						uint16_t table_height = table -> standard.height; if( table -> standard.height == (uint16_t) STD_MAX_unsigned ) table_height = ui -> window -> current_height - table -> standard.y - LIB_UI_MARGIN_DEFAULT;

						if( ui -> window -> z > 0 ) {
							if( (table -> offset_y + table_height + (ui -> window -> z * LIB_UI_ELEMENT_TABLE_height)) < (table -> limit_row * LIB_UI_ELEMENT_TABLE_height) )
								table -> offset_y += ui -> window -> z * LIB_UI_ELEMENT_TABLE_height;
							else if( (table -> limit_row * LIB_UI_ELEMENT_TABLE_height) > table_height )
								table -> offset_y = (table -> limit_row * LIB_UI_ELEMENT_TABLE_height) - table_height;
						} else {
							ui -> window -> z = ~ui -> window -> z + TRUE;
							if( table -> offset_y > (ui -> window -> z * LIB_UI_ELEMENT_TABLE_height) )
								table -> offset_y -= ui -> window -> z * LIB_UI_ELEMENT_TABLE_height;
							else table -> offset_y = EMPTY;
						}
						
						flush_element = TRUE;
					}

					break;
				}


				default: {
					if( ! (ui -> element[ i ] -> flag & LIB_UI_ELEMENT_FLAG_hover) ) {
						ui -> element[ i ] -> flag ^= LIB_UI_ELEMENT_FLAG_hover;

						flush_element = TRUE;
					}
				}
			}
		}

		if( flush_element ) { lib_ui_show_element( ui, ui -> element[ i ] ); *sync = TRUE; }
	}

	for( uint64_t i = 0; i < ui -> limit_control; i++ ) {
		uint8_t flush_element = FALSE;

		if( ui -> window -> x < (ui -> window -> current_width - ui -> control[ i ] -> standard.x) || ui -> window -> x > ((ui -> window -> current_width - ui -> control[ i ] -> standard.x) + ui -> control[ i ] -> standard.width) || ui -> window -> y < ui -> control[ i ] -> standard.y || (ui -> window -> y > ui -> control[ i ] -> standard.y + ui -> control[ i ] -> standard.height) ) {
			if( ui -> control[ i ] -> standard.flag & LIB_UI_ELEMENT_FLAG_hover ) {
				ui -> control[ i ] -> standard.flag ^= LIB_UI_ELEMENT_FLAG_hover;
				
				flush_element = TRUE;
			}
		} else {
			if( ui -> control[ i ] -> type == LIB_UI_ELEMENT_CONTROL_TYPE_min && mouse -> button & ~STD_IPC_MOUSE_BUTTON_left ) ui -> window -> flags |= LIB_WINDOW_FLAG_hide;

			if( ui -> control[ i ] -> type == LIB_UI_ELEMENT_CONTROL_TYPE_max && mouse -> button & ~STD_IPC_MOUSE_BUTTON_left ) {
				if( ! (ui -> control[ i ] -> standard.flag & LIB_UI_ELEMENT_FLAG_set) ) {
					ui -> window -> old_x		= ui -> window -> current_x;
					ui -> window -> old_y		= ui -> window -> current_y;
					ui -> window -> old_width	= ui -> window -> current_width;
					ui -> window -> old_height	= ui -> window -> current_height;

					ui -> control[ i ] -> standard.flag |= LIB_UI_ELEMENT_FLAG_set;

					ui -> window -> flags |= LIB_WINDOW_FLAG_enlarge;
				} else {
					ui -> window -> new_x		= ui -> window -> old_x;
					ui -> window -> new_y		= ui -> window -> old_y;
					ui -> window -> new_width	= ui -> window -> old_width;
					ui -> window -> new_height	= ui -> window -> old_height;

					ui -> window -> old_x		= EMPTY;
					ui -> window -> old_y		= EMPTY;
					ui -> window -> old_width	= EMPTY;
					ui -> window -> old_height	= EMPTY;

					ui -> control[ i ] -> standard.flag &= ~LIB_UI_ELEMENT_FLAG_set;

					ui -> window -> flags |= LIB_WINDOW_FLAG_properties;
				}
			}

			if( ui -> control[ i ] -> type == LIB_UI_ELEMENT_CONTROL_TYPE_close && mouse -> button & ~STD_IPC_MOUSE_BUTTON_left ) exit();

			if( ! (ui -> control[ i ] -> standard.flag & LIB_UI_ELEMENT_FLAG_hover) ) {
				ui -> control[ i ] -> standard.flag ^= LIB_UI_ELEMENT_FLAG_hover;

				flush_element = TRUE;
			}
		}

		if( flush_element ) { lib_ui_show_control( ui, ui -> control[ i ] ); *sync = TRUE; }
	}
}

void lib_ui_flush( struct LIB_UI_STRUCTURE *ui ) {
	lib_ui_clean( ui );

	lib_ui_show_name( ui );

	for( uint64_t i = 0; i < ui -> limit; i++ )
		lib_ui_show_element( ui, ui -> element[ i ] );

	for( uint64_t i = 0; i < ui -> limit_label; i++ )
		lib_ui_show_label( ui, (struct LIB_UI_STRUCTURE_ELEMENT_LABEL *) ui -> label[ i ] );

	for( uint64_t i = 0; i < ui -> limit_control; i++ )
		lib_ui_show_control( ui, (struct LIB_UI_STRUCTURE_ELEMENT_CONTROL *) ui -> control[ i ] );
}

static void lib_ui_list_insert( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT *element ) {
	ui -> element = (struct LIB_UI_STRUCTURE_ELEMENT **) realloc( ui -> element, sizeof( struct LIB_UI_STRUCTURE_ELEMENT ) * (ui -> limit + TRUE) );

	ui -> element[ ui -> limit++ ] = element;
}

void lib_ui_show_button( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT_BUTTON *button ) {
	// set pointer location of element inside window
	uint32_t *pixel = ui -> window -> pixel + (button -> standard.y * ui -> window -> current_width) + button -> standard.x;

	uint32_t color_background = LIB_UI_COLOR_BACKGROUND_BUTTON;
	if( button -> standard.flag & LIB_UI_ELEMENT_FLAG_active || button -> standard.flag & LIB_UI_ELEMENT_FLAG_hover ) color_background += LIB_UI_COLOR_INCREASE;
	if( button -> standard.flag & LIB_UI_ELEMENT_FLAG_disabled ) color_background = LIB_UI_COLOR_BACKGROUND_BUTTON_DISABLED;

	lib_ui_fill_rectangle( pixel + ui -> window -> current_width, ui -> window -> current_width, LIB_UI_RADIUS_DEFAULT, button -> standard.width, button -> standard.height, LIB_UI_COLOR_BACKGROUND_SHADOW );
	lib_ui_fill_rectangle( pixel, ui -> window -> current_width, LIB_UI_RADIUS_DEFAULT, button -> standard.width, button -> standard.height, color_background );

	if( button -> standard.height > LIB_FONT_HEIGHT_pixel ) pixel += ((button -> standard.height - LIB_FONT_HEIGHT_pixel) >> 1) * ui -> window -> current_width;

	lib_font( LIB_FONT_FAMILY_ROBOTO, button -> standard.name, lib_string_length( button -> standard.name ), STD_COLOR_BLACK, pixel + (button -> standard.width >> 1), ui -> window -> current_width, LIB_FONT_FLAG_ALIGN_center );
}

void lib_ui_show_checkbox( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT_CHECKBOX *checkbox ) {
	// set pointer location of element inside window
	uint32_t *pixel = ui -> window -> pixel + (checkbox -> standard.y * ui -> window -> current_width) + checkbox -> standard.x;

	uint32_t color_background = LIB_UI_COLOR_BACKGROUND_CHECKBOX;
	if( checkbox -> standard.flag & LIB_UI_ELEMENT_FLAG_set ) color_background = LIB_UI_COLOR_CHECKBOX_SELECTED;
	if( checkbox -> standard.flag & LIB_UI_ELEMENT_FLAG_hover || checkbox -> standard.flag & LIB_UI_ELEMENT_FLAG_active ) color_background += LIB_UI_COLOR_INCREASE;

	// clean area
	lib_ui_fill_rectangle( pixel, ui -> window -> current_width, LIB_UI_RADIUS_DEFAULT, checkbox -> standard.width, checkbox -> standard.height, LIB_UI_COLOR_BACKGROUND_DEFAULT );

	// show checkbox
	lib_ui_fill_rectangle( pixel + ui -> window -> current_width, ui -> window -> current_width, LIB_UI_RADIUS_DEFAULT, checkbox -> standard.height, checkbox -> standard.height, LIB_UI_COLOR_BACKGROUND_SHADOW );
	lib_ui_fill_rectangle( pixel, ui -> window -> current_width, LIB_UI_RADIUS_DEFAULT, checkbox -> standard.height, checkbox -> standard.height, color_background );

	// show description
	lib_font( LIB_FONT_FAMILY_ROBOTO, checkbox -> standard.name, lib_string_length( checkbox -> standard.name ), LIB_UI_COLOR_DEFAULT, pixel + checkbox -> standard.height + LIB_UI_PADDING_DEFAULT, ui -> window -> current_width, LIB_FONT_FLAG_ALIGN_left );
}

void lib_ui_show_control( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT_CONTROL *control ) {
	// set pointer location of element inside window
	uint32_t *pixel = ui -> window -> pixel + ui -> window -> current_width - control -> standard.x;

	uint64_t color_foreground = STD_COLOR_WHITE;
	uint32_t color_background = LIB_UI_COLOR_BACKGROUND_CONTROL_DEFAULT;
	if( control -> standard.flag & LIB_UI_ELEMENT_FLAG_active || control -> standard.flag & LIB_UI_ELEMENT_FLAG_hover ) {
		color_foreground = STD_COLOR_WHITE;

		switch( control -> type ) {
			case LIB_UI_ELEMENT_CONTROL_TYPE_close: { color_foreground = STD_COLOR_BLACK; color_background = LIB_UI_COLOR_BACKGROUND_CONTROL_CLOSE; break; }
			default: { color_background = LIB_UI_COLOR_BACKGROUND_CONTROL_DEFAULT + LIB_UI_COLOR_INCREASE; break; }
		}
	}

	pixel += (ui -> window -> current_width * ((LIB_UI_HEADER_HEIGHT - LIB_UI_ELEMENT_LABEL_height) >> TRUE) + ((LIB_UI_HEADER_HEIGHT - LIB_UI_ELEMENT_LABEL_height) >> TRUE));
	lib_ui_fill_rectangle( pixel + ui -> window -> current_width, ui -> window -> current_width, LIB_UI_ELEMENT_LABEL_height >> TRUE, LIB_UI_ELEMENT_LABEL_height, LIB_UI_ELEMENT_LABEL_height, LIB_UI_COLOR_BACKGROUND_SHADOW );
	lib_ui_fill_rectangle( pixel, ui -> window -> current_width, LIB_UI_ELEMENT_LABEL_height >> TRUE, LIB_UI_ELEMENT_LABEL_height, LIB_UI_ELEMENT_LABEL_height, color_background );

	switch( control -> type ) {
		case LIB_UI_ELEMENT_CONTROL_TYPE_close: {
			for( uint64_t i = 6; i <= LIB_UI_ELEMENT_LABEL_height - 6; i++ ) {
				pixel[ (i * ui -> window -> current_width) + i ] = color_foreground;
				pixel[ (LIB_UI_ELEMENT_LABEL_height - i) + (i * ui -> window -> current_width) ] = color_foreground;
			}

			break;
		}

		case LIB_UI_ELEMENT_CONTROL_TYPE_max: {
			for( uint64_t i = 6; i <= LIB_UI_ELEMENT_LABEL_height - 6; i++ ) {
				if( control -> standard.flag & LIB_UI_ELEMENT_FLAG_set ) {
					pixel[ (i * ui -> window -> current_width) + 6 ] = color_foreground;
					pixel[ ((LIB_UI_ELEMENT_LABEL_height - 6) * ui -> window -> current_width) + i ] = color_foreground;
				} else {
					pixel[ (6 * ui -> window -> current_width) + i ] = color_foreground;
					pixel[ (i * ui -> window -> current_width) + (LIB_UI_ELEMENT_LABEL_height - 6) ] = color_foreground;
				}
			}

			break;
		}

		case LIB_UI_ELEMENT_CONTROL_TYPE_min: {
			for( uint64_t i = 6; i <= LIB_UI_ELEMENT_LABEL_height - 6; i++ ) pixel[ ((LIB_UI_ELEMENT_LABEL_height - 6) * ui -> window -> current_width) + i ] = color_foreground;
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
		case TABLE:	{ lib_ui_show_table( ui, (struct LIB_UI_STRUCTURE_ELEMENT_TABLE *) element ); break; }
		case TEXTAREA:	{ lib_ui_show_textarea( ui, (struct LIB_UI_STRUCTURE_ELEMENT_TEXTAREA *) element ); break; }
		default: {
			// nothing
		}
	}
}

void lib_ui_show_input( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT_INPUT *input ) {
	// set pointer location of element inside window
	uint32_t *pixel = ui -> window -> pixel + (input -> standard.y * ui -> window -> current_width) + input -> standard.x;

	uint32_t color_background = LIB_UI_COLOR_BACKGROUND_INPUT;
	if( input -> standard.flag & LIB_UI_ELEMENT_FLAG_active || input -> standard.flag & LIB_UI_ELEMENT_FLAG_hover ) color_background += LIB_UI_COLOR_INCREASE;
	if( input -> standard.flag & LIB_UI_ELEMENT_FLAG_disabled ) color_background = LIB_UI_COLOR_BACKGROUND_INPUT_DISABLED;

	// border
	lib_ui_fill_rectangle( pixel, ui -> window -> current_width, LIB_UI_RADIUS_DEFAULT, input -> standard.width, input -> standard.height, color_background + LIB_UI_COLOR_INCREASE_LITTLE );
	// inner
	lib_ui_fill_rectangle( pixel + (ui -> window -> current_width) + TRUE, ui -> window -> current_width, LIB_UI_RADIUS_DEFAULT, input -> standard.width - 2, input -> standard.height - 2, color_background + LIB_UI_COLOR_INCREASE_LITTLE );

	if( input -> standard.height > LIB_FONT_HEIGHT_pixel ) pixel += ((input -> standard.height - LIB_FONT_HEIGHT_pixel) >> 1) * ui -> window -> current_width;

	uint32_t color_foreground = LIB_UI_COLOR_INPUT;
	if( input -> standard.flag & LIB_UI_ELEMENT_FLAG_disabled ) color_foreground = LIB_UI_COLOR_INPUT_DISABLED;

	uint64_t name_length_max = lib_string_length( input -> standard.name ) - input -> offset;
	while( lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, input -> standard.name + input -> offset, name_length_max ) > input -> standard.width - LIB_UI_PADDING_DEFAULT ) { if( ! --name_length_max ) break; }

	if( name_length_max ) lib_font( LIB_FONT_FAMILY_ROBOTO_MONO, input -> standard.name + input -> offset, name_length_max, color_foreground, pixel + LIB_UI_PADDING_DEFAULT, ui -> window -> current_width, LIB_FONT_FLAG_ALIGN_left );

	if( ! (input -> standard.flag & LIB_UI_ELEMENT_FLAG_active) || input -> standard.flag & LIB_UI_ELEMENT_FLAG_disabled ) return;

	uint64_t x = lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, input -> standard.name + input -> offset, input -> index - input -> offset );
	for( uint64_t y = 0; y < LIB_FONT_HEIGHT_pixel; y++ )
		pixel[ (y * ui -> window -> current_width) + x + LIB_UI_PADDING_DEFAULT ] = STD_COLOR_WHITE;
}

void lib_ui_show_label( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT_LABEL *label ) {
	// set pointer location of element inside window
	uint32_t *pixel = ui -> window -> pixel + (label -> standard.y * ui -> window -> current_width) + label -> standard.x;

	if( label -> standard.height > LIB_FONT_HEIGHT_pixel ) pixel += ((label -> standard.height - LIB_FONT_HEIGHT_pixel) >> 1) * ui -> window -> current_width;

	// show description
	lib_font( LIB_FONT_FAMILY_ROBOTO, label -> standard.name, lib_string_length( label -> standard.name ), LIB_UI_COLOR_DEFAULT, pixel, ui -> window -> current_width, LIB_FONT_FLAG_ALIGN_left );
}

void lib_ui_show_name( struct LIB_UI_STRUCTURE *ui ) {
	// set pointer location of element inside window
	uint32_t *pixel = ui -> window -> pixel + (((LIB_UI_HEADER_HEIGHT - LIB_FONT_HEIGHT_pixel) >> TRUE) * ui -> window -> current_width) + LIB_UI_MARGIN_DEFAULT;

	// clean area
	lib_ui_fill_rectangle( pixel, ui -> window -> current_width, 0, ui -> window -> current_width - LIB_UI_MARGIN_DEFAULT - LIB_UI_PADDING_DEFAULT - (ui -> limit_control * LIB_UI_HEADER_HEIGHT), LIB_FONT_HEIGHT_pixel, LIB_UI_COLOR_BACKGROUND_DEFAULT );

	// limit name length to header width
	uint64_t limit = lib_ui_string( LIB_FONT_FAMILY_ROBOTO, ui -> window -> name, ui -> window -> name_length, ui -> window -> current_width - LIB_UI_MARGIN_DEFAULT - LIB_UI_PADDING_DEFAULT - (ui -> limit_control * LIB_UI_HEADER_HEIGHT) );

	// show description
	lib_font( LIB_FONT_FAMILY_ROBOTO, ui -> window -> name, limit, LIB_UI_COLOR_DEFAULT, pixel, ui -> window -> current_width, LIB_FONT_FLAG_ALIGN_left );

	ui -> window -> header_height	= LIB_UI_HEADER_HEIGHT;
	ui -> window -> header_width	= ui -> window -> current_width - (LIB_UI_HEADER_HEIGHT * (ui -> limit_control + TRUE));
}

void lib_ui_show_radio( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT_RADIO *radio ) {
	// set pointer location of element inside window
	uint32_t *pixel = ui -> window -> pixel + (radio -> standard.y * ui -> window -> current_width) + radio -> standard.x;

	uint32_t color_background = LIB_UI_COLOR_BACKGROUND_RADIO;
	if( radio -> standard.flag & LIB_UI_ELEMENT_FLAG_set ) color_background = LIB_UI_COLOR_RADIO_SELECTED;
	if( radio -> standard.flag & LIB_UI_ELEMENT_FLAG_active || radio -> standard.flag & LIB_UI_ELEMENT_FLAG_hover ) color_background += LIB_UI_COLOR_INCREASE;

	// clean area
	lib_ui_fill_rectangle( pixel, ui -> window -> current_width, radio -> standard.height >> STD_SHIFT_2, radio -> standard.width, radio -> standard.height, LIB_UI_COLOR_BACKGROUND_DEFAULT );

	// show radio
	lib_ui_fill_rectangle( pixel + ui -> window -> current_width, ui -> window -> current_width, radio -> standard.height >> STD_SHIFT_2, radio -> standard.height, radio -> standard.height, LIB_UI_COLOR_BACKGROUND_SHADOW );
	lib_ui_fill_rectangle( pixel, ui -> window -> current_width, radio -> standard.height >> STD_SHIFT_2, radio -> standard.height, radio -> standard.height, color_background );

	// show description
	lib_font( LIB_FONT_FAMILY_ROBOTO, radio -> standard.name, lib_string_length( radio -> standard.name ), LIB_UI_COLOR_DEFAULT, pixel + radio -> standard.height + LIB_UI_PADDING_DEFAULT, ui -> window -> current_width, LIB_FONT_FLAG_ALIGN_left );
}

void lib_ui_show_table( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT_TABLE *table ) {
	// set pointer location of element inside window
	uint32_t *pixel = ui -> window -> pixel + (table -> standard.y * ui -> window -> current_width) + table -> standard.x;

	uint32_t color_background = LIB_UI_COLOR_BACKGROUND_TABLE_ROW;
	if( table -> standard.flag & LIB_UI_ELEMENT_FLAG_disabled ) color_background = LIB_UI_COLOR_BACKGROUND_BUTTON_DISABLED;

	uint16_t table_width = table -> standard.width; if( table -> standard.width == (uint16_t) STD_MAX_unsigned ) table_width = ui -> window -> current_width - table -> standard.x - LIB_UI_MARGIN_DEFAULT;
	uint16_t table_height = table -> standard.height; if( table -> standard.height == (uint16_t) STD_MAX_unsigned ) table_height = ui -> window -> current_height - table -> standard.y - LIB_UI_MARGIN_DEFAULT;

	lib_ui_fill_rectangle( pixel, ui -> window -> current_width, EMPTY, table_width, table_height, LIB_UI_COLOR_BACKGROUND_DEFAULT );

	uint64_t content_width; for( uint64_t i = 0; i < table -> limit_column; i++ ) if( table -> header[ i ].width ) content_width += table -> header[ i ].width; else { content_width = table_width; break; }
	uint64_t content_height = (table -> limit_row + TRUE) * LIB_UI_ELEMENT_TABLE_height; if( content_height < table_height ) content_height = table_height;

	if( table -> pixel ) free( table -> pixel );
	table -> pixel = (uint32_t *) malloc( (content_width * content_height) << STD_VIDEO_DEPTH_shift );

	lib_ui_fill_rectangle( table -> pixel, content_width, EMPTY, content_width, LIB_UI_ELEMENT_TABLE_height, LIB_UI_COLOR_BACKGROUND_TABLE_HEADER );
	lib_ui_fill_rectangle( table -> pixel + (LIB_UI_ELEMENT_TABLE_height * content_width), content_width, EMPTY, content_width, content_height - LIB_UI_ELEMENT_TABLE_height, LIB_UI_COLOR_BACKGROUND_TABLE_ROW );

	for( uint64_t x = 0; x < table -> limit_column; x++ ) {
		if( ! table -> header[ x ].cell.name ) continue;

		uint64_t table_width_column = content_width / table -> limit_column;
		if( content_width != table_width ) table_width_column = table -> header[ x ].width;

		uint32_t *pixel_table_header = table -> pixel + (table_width_column * x);
		if( LIB_FONT_HEIGHT_pixel < LIB_UI_ELEMENT_TABLE_height ) pixel_table_header += ((LIB_UI_ELEMENT_TABLE_height - LIB_FONT_HEIGHT_pixel) >> TRUE) * content_width;

		if( table -> header[ x ].cell.flag & LIB_FONT_FLAG_ALIGN_right ) pixel_table_header += table_width_column - LIB_UI_PADDING_TABLE;
		else pixel_table_header += LIB_UI_PADDING_TABLE;

		uint64_t limit = lib_ui_string( LIB_FONT_FAMILY_ROBOTO, table -> header[ x ].cell.name, lib_string_length( table -> header[ x ].cell.name ), table_width_column );
		lib_font( LIB_FONT_FAMILY_ROBOTO, table -> header[ x ].cell.name, limit, LIB_UI_COLOR_DEFAULT, pixel_table_header, content_width, table -> header[ x ].cell.flag & LIB_FONT_FLAG_mask );
	}

	for( uint64_t y = 0; y < table -> limit_row; y++ ) {
		uint32_t *pixel_table_row = table -> pixel + (LIB_UI_ELEMENT_TABLE_height * table_width) + (LIB_UI_ELEMENT_TABLE_height * content_width * y);

		uint32_t color_foreground = STD_COLOR_WHITE;

		color_background = LIB_UI_COLOR_BACKGROUND_TABLE_ROW;
		if( table -> row[ y ].flag & LIB_UI_ELEMENT_FLAG_set ) { color_background = LIB_UI_COLOR_BACKGROUND_TABLE_ROW_SET; color_foreground = STD_COLOR_BLACK; }
		if( table -> row[ y ].flag & LIB_UI_ELEMENT_FLAG_hover ) color_background += LIB_UI_COLOR_INCREASE;
		lib_ui_fill_rectangle( pixel_table_row, content_width, EMPTY, content_width, LIB_UI_ELEMENT_TABLE_height, color_background );

		for( uint64_t x = 0; x < table -> limit_column; x++ ) {
			uint64_t table_width_column = content_width / table -> limit_column;
			if( content_width != table_width ) table_width_column = table -> header[ x ].width;

			uint32_t *pixel_table_cell = pixel_table_row + (((LIB_UI_ELEMENT_TABLE_height - LIB_FONT_HEIGHT_pixel) >> TRUE) * content_width) + (table_width_column * x);
			if( table -> row[ y ].cell[ x ].flag & LIB_FONT_FLAG_ALIGN_right ) pixel_table_cell += table_width_column - LIB_UI_PADDING_TABLE;
			else pixel_table_cell += LIB_UI_PADDING_TABLE;

			uint64_t offset = EMPTY;
			if( table -> row[ y ].cell[ x ].icon ) {
				offset = 16 + LIB_UI_PADDING_TABLE;

				for( uint64_t iy = 0; iy < 16; iy++ )
					for( uint64_t ix = 0; ix < 16; ix++ )
						pixel_table_cell[ (iy * content_width) + ix ] = lib_color_blend( pixel_table_cell[ (iy * content_width) + ix ], table -> row[ y ].cell[ x ].icon[ (iy * 16) + ix ] );
			}
			if( table -> row[ y ].cell[ x ].name ) {
				uint64_t limit = lib_ui_string( LIB_FONT_FAMILY_ROBOTO, table -> row[ y ].cell[ x ].name, lib_string_length( table -> row[ y ].cell[ x ].name ), table_width_column );
				lib_font( LIB_FONT_FAMILY_ROBOTO, table -> row[ y ].cell[ x ].name, limit, color_foreground, pixel_table_cell + offset, content_width, table -> row[ y ].cell[ x ].flag & LIB_FONT_FLAG_mask );
			}
		}
	}


	for( uint64_t y = 0; y < LIB_UI_ELEMENT_TABLE_height; y++ )
		for( uint64_t x = 0; x < table_width; x++ )
			pixel[ (y * ui -> window -> current_width) + x ] = table -> pixel[ (y * content_width) + x ];

	uint64_t y_limit = table_height;
	if( content_height < table_height ) y_limit = content_height;

	uint32_t *pixel_at_offset = (uint32_t *) table -> pixel + (table -> offset_y * content_width);
	for( uint64_t y = LIB_UI_ELEMENT_TABLE_height; y < y_limit; y++ )
		for( uint64_t x = 0; x < table_width; x++ )
			pixel[ (y * ui -> window -> current_width) + x ] = pixel_at_offset[ (y * content_width) + x ];
}

void lib_ui_show_textarea( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT_TEXTAREA *textarea ) {
	// in progress

// 	// set pointer location of element inside window
// 	uint32_t *pixel = ui -> window -> pixel + (textarea -> standard.y * ui -> window -> current_width) + textarea -> standard.x;

// 	uint16_t textarea_width = textarea -> standard.width; if( textarea -> standard.width == (uint16_t) STD_MAX_unsigned ) textarea_width = ui -> window -> current_width - textarea -> standard.x - LIB_UI_MARGIN_DEFAULT;
// 	uint16_t textarea_height = textarea -> standard.height; if( textarea -> standard.height == (uint16_t) STD_MAX_unsigned ) textarea_height = ui -> window -> current_height - textarea -> standard.y - LIB_UI_MARGIN_DEFAULT;

// 	lib_ui_fill_rectangle( pixel, ui -> window -> current_width, LIB_UI_RADIUS_DEFAULT, textarea_width, textarea_height, LIB_UI_COLOR_BACKGROUND_TEXTAREA );

// 	uint64_t content_width = EMPTY;

// 	uint8_t *line = textarea -> string; while( *line ) {
// 		uint64_t line_in_characters = lib_string_length_line( line );
// 		uint64_t line_in_pixels = lib_font_length_string( LIB_FONT_FAMILY_ROBOTO, line, line_in_characters );
// 		if( content_width < line_in_pixels ) content_width = line_in_pixels + (LIB_UI_PADDING_TEXTAREA << STD_SHIFT_2);

// 		line += line_in_characters + TRUE;
// 	}
	
// 	uint64_t string_lines = lib_string_count( textarea -> string, lib_string_length( textarea -> string ), '\n' ) + 1;
// 	uint64_t content_height = (LIB_FONT_HEIGHT_pixel * string_lines) + (LIB_UI_PADDING_TEXTAREA << STD_SHIFT_2); if( content_height < textarea_height ) content_height = textarea_height;

// 	if( textarea -> pixel ) free( textarea -> pixel );
// 	textarea -> pixel = (uint32_t *) malloc( (content_width * content_height) << STD_VIDEO_DEPTH_shift );

// 	lib_ui_fill_rectangle( textarea -> pixel, content_width, EMPTY, content_width, content_height, LIB_UI_COLOR_BACKGROUND_TEXTAREA );

// 	uint32_t *pixel_paragraph = (uint32_t *) textarea -> pixel + (LIB_UI_PADDING_TEXTAREA * content_width) + LIB_UI_PADDING_TEXTAREA;

// 	uint8_t *paragraph = textarea -> string; while( *paragraph != STD_ASCII_TERMINATOR ) {
// 		uint64_t line_in_characters = lib_string_length_line( paragraph );
// 		if( line_in_characters ) lib_font( LIB_FONT_FAMILY_ROBOTO, paragraph, line_in_characters, STD_COLOR_WHITE, pixel_paragraph, content_width, LIB_FONT_FLAG_ALIGN_left );

// 		pixel_paragraph += (LIB_FONT_HEIGHT_pixel * content_width );

// 		paragraph += line_in_characters;
// 		if( *paragraph == STD_ASCII_NEW_LINE ) paragraph++;
// 	}

// 	uint64_t y_limit = textarea_height;
// 	if( content_height < textarea_height ) y_limit = content_height;

// 	uint32_t *pixel_at_offset = (uint32_t *) textarea -> pixel + (textarea -> offset_y * content_width) + textarea -> offset_x;
// 	for( uint64_t y = 0; y < y_limit; y++ )
// 		for( uint64_t x = 0; x < textarea_width; x++ )
// 			pixel[ (y * ui -> window -> current_width) + x ] = pixel_at_offset[ (y * content_width) + x ];
}

static uint64_t lib_ui_string( uint8_t font_family, uint8_t *string, uint64_t limit, uint64_t width_pixel ) {
	// set max length of string
	while( lib_font_length_string( font_family, string, limit ) > width_pixel ) if( ! --limit ) break;

	// new string limit
	return limit;
}
