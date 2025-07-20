/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

// #include	"../library/color.h"
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

	// by default no icon provided
	ui -> icon = EMPTY;

	// no elements by default
	ui -> limit = ui -> limit_button = ui -> limit_checkbox = ui -> limit_control = ui -> limit_input = ui -> limit_label = ui -> limit_list = ui -> limit_radio = ui -> limit_table = ui -> limit_textarea = EMPTY;

	// prepare area for element list
	ui -> element = (struct LIB_UI_STRUCTURE_ELEMENT **) malloc( ui -> limit );

	// element selected by default
	ui -> element_active = EMPTY;

	// prepare area for all ui elements
	ui -> button = (struct LIB_UI_STRUCTURE_ELEMENT_BUTTON **) malloc( ui -> limit_button );
	ui -> checkbox = (struct LIB_UI_STRUCTURE_ELEMENT_CHECKBOX **) malloc( ui -> limit_checkbox );
	ui -> control = (struct LIB_UI_STRUCTURE_ELEMENT_CONTROL **) malloc( ui -> limit_control );
	ui -> input = (struct LIB_UI_STRUCTURE_ELEMENT_INPUT **) malloc( ui -> limit_input );
	ui -> label = (struct LIB_UI_STRUCTURE_ELEMENT_LABEL **) malloc( ui -> limit_label );
	ui -> list = (struct LIB_UI_STRUCTURE_ELEMENT_LIST **) malloc( ui -> limit_list );
	ui -> radio = (struct LIB_UI_STRUCTURE_ELEMENT_RADIO **) malloc( ui -> limit_radio );
	ui -> table = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE **) malloc( ui -> limit_table );
	ui -> textarea = (struct LIB_UI_STRUCTURE_ELEMENT_TEXTAREA **) malloc( ui -> limit_textarea );

	// clean up window area
	lib_ui_clean( ui );

	// return ui properties
	return ui;
}

uint64_t lib_ui_add_button( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint8_t *name, uint16_t height, uint8_t flag_ui ) {
	ui -> button = (struct LIB_UI_STRUCTURE_ELEMENT_BUTTON **) realloc( ui -> button, sizeof( struct LIB_UI_STRUCTURE_ELEMENT_BUTTON ) * (ui -> limit_button + TRUE) );
	ui -> button[ ui -> limit_button ] = (struct LIB_UI_STRUCTURE_ELEMENT_BUTTON *) malloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_BUTTON ) );

	ui -> button[ ui -> limit_button ] -> standard.x		= x;
	ui -> button[ ui -> limit_button ] -> standard.y		= y;
	ui -> button[ ui -> limit_button ] -> standard.width		= width;
	ui -> button[ ui -> limit_button ] -> standard.height		= height;
	ui -> button[ ui -> limit_button ] -> standard.flag		= flag_ui;
	ui -> button[ ui -> limit_button ] -> standard.type		= BUTTON;

	if( *name ) {
		ui -> button[ ui -> limit_button ] -> standard.name	= (uint8_t *) calloc( lib_string_length( name ) + TRUE );

		memcpy( ui -> button[ ui -> limit_button ] -> standard.name, name, lib_string_length( name ) );
	}

	lib_ui_list_insert( ui, (struct LIB_UI_STRUCTURE_ELEMENT *) ui -> button[ ui -> limit_button ] );

	return ui -> limit_button++;
}

uint64_t lib_ui_add_checkbox( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint8_t *name, uint8_t flag_ui ) {
	ui -> checkbox = (struct LIB_UI_STRUCTURE_ELEMENT_CHECKBOX **) realloc( ui -> checkbox, sizeof( struct LIB_UI_STRUCTURE_ELEMENT_CHECKBOX ) * (ui -> limit_checkbox + TRUE) );
	ui -> checkbox[ ui -> limit_checkbox ] = (struct LIB_UI_STRUCTURE_ELEMENT_CHECKBOX *) malloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_CHECKBOX ) );

	ui -> checkbox[ ui -> limit_checkbox ] -> standard.x		= x;
	ui -> checkbox[ ui -> limit_checkbox ] -> standard.y		= y;
	ui -> checkbox[ ui -> limit_checkbox ] -> standard.width	= width;
	ui -> checkbox[ ui -> limit_checkbox ] -> standard.height	= LIB_UI_ELEMENT_CHECKBOX_height;
	ui -> checkbox[ ui -> limit_checkbox ] -> standard.flag		= flag_ui;
	ui -> checkbox[ ui -> limit_checkbox ] -> standard.type		= CHECKBOX;

	if( *name ) {
		ui -> checkbox[ ui -> limit_checkbox ] -> standard.name	= (uint8_t *) calloc( lib_string_length( name ) + TRUE );

		memcpy( ui -> checkbox[ ui -> limit_checkbox ] -> standard.name, name, lib_string_length( name ) );
	}

	ui -> checkbox[ ui -> limit_checkbox ] -> set = FALSE;

	lib_ui_list_insert( ui, (struct LIB_UI_STRUCTURE_ELEMENT *) ui -> checkbox[ ui -> limit_checkbox ] );

	return ui -> limit_checkbox++;
}

uint64_t lib_ui_add_control( struct LIB_UI_STRUCTURE *ui, uint8_t type ) {
	ui -> control = (struct LIB_UI_STRUCTURE_ELEMENT_CONTROL **) realloc( ui -> control, sizeof( struct LIB_UI_STRUCTURE_ELEMENT_CONTROL ) * (ui -> limit_control + TRUE) );
	ui -> control[ ui -> limit_control ] = (struct LIB_UI_STRUCTURE_ELEMENT_CONTROL *) malloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_CONTROL ) );

	ui -> control[ ui -> limit_control ] -> standard.x	= LIB_UI_HEADER_HEIGHT * (ui -> limit_control + TRUE);
	ui -> control[ ui -> limit_control ] -> standard.y	= EMPTY;
	ui -> control[ ui -> limit_control ] -> standard.width	= LIB_UI_HEADER_HEIGHT;
	ui -> control[ ui -> limit_control ] -> standard.height	= LIB_UI_HEADER_HEIGHT;
	ui -> control[ ui -> limit_control ] -> standard.type	= CONTROL;

	ui -> control[ ui -> limit_control ] -> type		= type;

	return ui -> limit_control++;
}

uint64_t lib_ui_add_input( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint8_t *name, uint8_t flag_ui, uint8_t flag_font ) {
	ui -> input = (struct LIB_UI_STRUCTURE_ELEMENT_INPUT **) realloc( ui -> input, sizeof( struct LIB_UI_STRUCTURE_ELEMENT_INPUT ) * (ui -> limit_input + TRUE) );
	ui -> input[ ui -> limit_input ] = (struct LIB_UI_STRUCTURE_ELEMENT_INPUT *) malloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_INPUT ) );

	ui -> input[ ui -> limit_input ] -> standard.x			= x;
	ui -> input[ ui -> limit_input ] -> standard.y			= y;
	ui -> input[ ui -> limit_input ] -> standard.width		= width;
	ui -> input[ ui -> limit_input ] -> standard.height		= LIB_UI_ELEMENT_INPUT_height;
	ui -> input[ ui -> limit_input ] -> standard.flag		= flag_ui;
	ui -> input[ ui -> limit_input ] -> standard.type		= INPUT;

	if( *name ) {
		ui -> input[ ui -> limit_input ] -> standard.name	= (uint8_t *) calloc( LIB_UI_ELEMENT_INPUT_length_max );

		memcpy( ui -> input[ ui -> limit_input ] -> standard.name, name, lib_string_length( name ) );
	}

	ui -> input[ ui -> limit_input ] -> flag			= flag_font;
	ui -> input[ ui -> limit_input ] -> offset			= EMPTY;
	ui -> input[ ui -> limit_input ] -> index			= EMPTY;

	lib_ui_list_insert( ui, (struct LIB_UI_STRUCTURE_ELEMENT *) ui -> input[ ui -> limit_input ] );

	return ui -> limit_input++;
}

uint64_t lib_ui_add_label( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint8_t *name, uint8_t flag_ui, uint8_t flag_font ) {
	ui -> label = (struct LIB_UI_STRUCTURE_ELEMENT_LABEL **) realloc( ui -> label, sizeof( struct LIB_UI_STRUCTURE_ELEMENT_LABEL ) * (ui -> limit_label + TRUE) );
	ui -> label[ ui -> limit_label ] = (struct LIB_UI_STRUCTURE_ELEMENT_LABEL *) malloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_LABEL ) );

	ui -> label[ ui -> limit_label ] -> standard.x			= x;
	ui -> label[ ui -> limit_label ] -> standard.y			= y;
	ui -> label[ ui -> limit_label ] -> standard.width		= width;
	ui -> label[ ui -> limit_label ] -> standard.height		= LIB_UI_ELEMENT_LABEL_height;
	ui -> label[ ui -> limit_label ] -> standard.type		= LABEL;
	ui -> label[ ui -> limit_label ] -> standard.flag		= flag_ui;

	if( *name ) {
		ui -> label[ ui -> limit_label ] -> standard.name	= (uint8_t *) calloc( lib_string_length( name ) + TRUE );

		memcpy( ui -> label[ ui -> limit_label ] -> standard.name, name, lib_string_length( name ) );
	}

	ui -> label[ ui -> limit_label ] -> flag			= flag_font;

	return ui -> limit_label++;
}

uint64_t lib_ui_add_list( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint16_t height, struct LIB_UI_STRUCTURE_ELEMENT_LIST_ENTRY *entry, uint64_t limit ) {
	ui -> list = (struct LIB_UI_STRUCTURE_ELEMENT_LIST **) realloc( ui -> list, sizeof( struct LIB_UI_STRUCTURE_ELEMENT_LIST ) * (ui -> limit_list + TRUE) );
	ui -> list[ ui -> limit_list ] = (struct LIB_UI_STRUCTURE_ELEMENT_LIST *) malloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_LIST ) );

	ui -> list[ ui -> limit_list ] -> standard.x		= x;
	ui -> list[ ui -> limit_list ] -> standard.y		= y;
	ui -> list[ ui -> limit_list ] -> standard.width	= width;
	ui -> list[ ui -> limit_list ] -> standard.height	= height;
	ui -> list[ ui -> limit_list ] -> standard.type		= LIST;

	ui -> list[ ui -> limit_list ] -> limit_entry		= limit;
	ui -> list[ ui -> limit_list ] -> entry			= entry;

	lib_ui_list_insert( ui, (struct LIB_UI_STRUCTURE_ELEMENT *) ui -> list[ ui -> limit_list ] );

	return ui -> limit_list++;
}

uint64_t lib_ui_add_radio( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint8_t *name, uint8_t group, uint8_t flag_ui ) {
	ui -> radio = (struct LIB_UI_STRUCTURE_ELEMENT_RADIO **) realloc( ui -> radio, sizeof( struct LIB_UI_STRUCTURE_ELEMENT_RADIO ) * (ui -> limit_radio + TRUE) );
	ui -> radio[ ui -> limit_radio ] = (struct LIB_UI_STRUCTURE_ELEMENT_RADIO *) malloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_RADIO ) );

	ui -> radio[ ui -> limit_radio ] -> standard.x		= x;
	ui -> radio[ ui -> limit_radio ] -> standard.y		= y;
	ui -> radio[ ui -> limit_radio ] -> standard.width	= width;
	ui -> radio[ ui -> limit_radio ] -> standard.height	= LIB_FONT_HEIGHT_pixel;
	ui -> radio[ ui -> limit_radio ] -> standard.flag	= flag_ui;
	ui -> radio[ ui -> limit_radio ] -> standard.type	= RADIO;

	if( *name ) {
		ui -> radio[ ui -> limit_radio ] -> standard.name	= (uint8_t *) calloc( lib_string_length( name ) + TRUE );

		memcpy( ui -> radio[ ui -> limit_radio ] -> standard.name, name, lib_string_length( name ) );
	}

	ui -> radio[ ui -> limit_radio ] -> set = FALSE;
	ui -> radio[ ui -> limit_radio ] -> group = group;

	lib_ui_list_insert( ui, (struct LIB_UI_STRUCTURE_ELEMENT *) ui -> radio[ ui -> limit_radio ] );

	return ui -> limit_radio++;
}

uint64_t lib_ui_add_table( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint16_t height, struct LIB_UI_STRUCTURE_ELEMENT_TABLE_HEADER *header, struct LIB_UI_STRUCTURE_ELEMENT_TABLE_ROW *row, uint64_t c, uint64_t r ) {
	ui -> table = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE **) realloc( ui -> table, sizeof( struct LIB_UI_STRUCTURE_ELEMENT_TABLE ) * (ui -> limit_table + TRUE) );
	ui -> table[ ui -> limit_table ] = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE *) malloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_TABLE ) );

	ui -> table[ ui -> limit_table ] -> standard.x		= x;
	ui -> table[ ui -> limit_table ] -> standard.y		= y;
	ui -> table[ ui -> limit_table ] -> standard.width	= width;
	ui -> table[ ui -> limit_table ] -> standard.height	= height;
	ui -> table[ ui -> limit_table ] -> standard.type	= TABLE;

	ui -> table[ ui -> limit_table ] -> limit_column	= c;
	ui -> table[ ui -> limit_table ] -> limit_row		= r;
	ui -> table[ ui -> limit_table ] -> header		= header;
	ui -> table[ ui -> limit_table ] -> row			= row;
	ui -> table[ ui -> limit_table ] -> pixel		= (uint32_t *) malloc( FALSE );

	lib_ui_list_insert( ui, (struct LIB_UI_STRUCTURE_ELEMENT *) ui -> table[ ui -> limit_table ] );

	return ui -> limit_table++;
}

uint64_t lib_ui_add_textarea( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint64_t height, uint8_t flag_ui, uint8_t *string, uint8_t font ) {
	ui -> textarea = (struct LIB_UI_STRUCTURE_ELEMENT_TEXTAREA **) realloc( ui -> textarea, sizeof( struct LIB_UI_STRUCTURE_ELEMENT_TEXTAREA ) * (ui -> limit_textarea + TRUE) );
	ui -> textarea[ ui -> limit_textarea ] = (struct LIB_UI_STRUCTURE_ELEMENT_TEXTAREA *) malloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_TEXTAREA ) );

	ui -> textarea[ ui -> limit_textarea ] -> standard.x		= x;
	ui -> textarea[ ui -> limit_textarea ] -> standard.y		= y;
	ui -> textarea[ ui -> limit_textarea ] -> standard.width	= width;
	ui -> textarea[ ui -> limit_textarea ] -> standard.height	= height;
	ui -> textarea[ ui -> limit_textarea ] -> standard.type		= TEXTAREA;
	ui -> textarea[ ui -> limit_textarea ] -> standard.flag		= flag_ui;

	ui -> textarea[ ui -> limit_textarea ] -> string		= string;
	ui -> textarea[ ui -> limit_textarea ] -> length		= lib_string_length( string );

	// ui -> textarea[ ui -> limit_textarea ] -> count			= FALSE;
	// ui -> textarea[ ui -> limit_textarea ] -> line			= (struct LIB_UI_STRUCTURE_ELEMENT_TEXTAREA_LINE *) malloc( ui -> textarea[ ui -> limit_textarea ] -> count );

	// for( uint64_t i = 0; i < lib_string_length( string ); ) {
	// 	ui -> textarea[ ui -> limit_textarea ] -> line = (struct LIB_UI_STRUCTURE_ELEMENT_TEXTAREA_LINE *) realloc( ui -> textarea[ ui -> limit_textarea ] -> line, ui -> textarea[ ui -> limit_textarea ] -> count + 1 );

	// 	ui -> textarea[ ui -> limit_textarea ] -> line[ ui -> textarea[ ui -> limit_textarea ] -> count ].length = lib_string_length_line( string );
	// 	ui -> textarea[ ui -> limit_textarea ] -> line[ ui -> textarea[ ui -> limit_textarea ] -> count ].string = (uint8_t *) calloc( ui -> textarea[ ui -> limit_textarea ] -> line[ ui -> textarea[ ui -> limit_textarea ] -> count ].length + 1 );

	// 	for( uint64_t k = 0; k < ui -> textarea[ ui -> limit_textarea ] -> line[ ui -> textarea[ ui -> limit_textarea ] -> count ].length; k++ ) ui -> textarea[ ui -> limit_textarea ] -> line[ ui -> textarea[ ui -> limit_textarea ] -> count ].string[ k ] = ui -> textarea[ ui -> limit_textarea ] -> string[ k ];

	// 	i += ui -> textarea[ ui -> limit_textarea ] -> line[ ui -> textarea[ ui -> limit_textarea ] -> count ].length + 1;

	// 	ui -> textarea[ ui -> limit_textarea ] -> count++;
	// }

	ui -> textarea[ ui -> limit_textarea ] -> font			= font;

	ui -> textarea[ ui -> limit_textarea ] -> cursor_x		= EMPTY;
	ui -> textarea[ ui -> limit_textarea ] -> cursor_y		= EMPTY;

	lib_ui_list_insert( ui, (struct LIB_UI_STRUCTURE_ELEMENT *) ui -> textarea[ ui -> limit_textarea ] );

	return ui -> limit_textarea++;
}

void lib_ui_border( struct LIB_UI_STRUCTURE *ui ) {
	uint32_t color = LIB_UI_COLOR_BORDER_DEFAULT;
	// if( ui -> window_active ) color = LIB_UI_COLOR_BORDER_ACTIVE;

	for( uint64_t y = 0; y < ui -> window -> current_height; y++ )
		for( uint64_t x = 0; x < ui -> window -> current_width; x++ ) {
			if( !x || !y ) ui -> window -> pixel[ (y * ui -> window -> current_width) + x ] = color + LIB_UI_COLOR_INCREASE_LITTLE;
			if( x == ui -> window -> current_width - 1 || y == ui -> window -> current_height - 1 ) ui -> window -> pixel[ (y * ui -> window -> current_width) + x ] = color - LIB_UI_COLOR_INCREASE_LITTLE;
		}
}

void lib_ui_clean( struct LIB_UI_STRUCTURE *ui ) {
	lib_ui_fill_rectangle( ui -> window -> pixel, ui -> window -> current_width, EMPTY, ui -> window -> current_width, ui -> window -> current_height, LIB_UI_COLOR_BACKGROUND_DEFAULT );

	lib_ui_border( ui );
}

uint16_t lib_ui_event( struct LIB_UI_STRUCTURE *ui ) {
	uint8_t sync = FALSE;

	struct LIB_WINDOW_STRUCTURE *new = EMPTY;
	if( (new = lib_window_event( ui -> window )) ) {
		ui -> window = new;

		lib_ui_clean( ui );

		lib_ui_flush( ui );

		ui -> window -> flags |= LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_flush;
	}

	lib_ui_event_mouse( ui, (uint8_t *) &sync );
	uint16_t key = lib_ui_event_keyboard( ui, (uint8_t *) &sync );

	if( (ui -> window -> flags & LIB_WINDOW_FLAG_active) != ui -> window_active ) {
		ui -> window_active = ui -> window -> flags & TRUE;

		lib_ui_show_name( ui );

		sync = TRUE;
	}

	if( sync ) ui -> window -> flags |= LIB_WINDOW_FLAG_flush;

	return key;
}

static uint16_t lib_ui_event_keyboard( struct LIB_UI_STRUCTURE *ui, uint8_t *sync ) {
	if( ! ui -> limit ) return EMPTY;

	uint8_t ipc_data[ STD_IPC_SIZE_byte ] = { EMPTY };

	if( ! std_ipc_receive_by_type( (uint8_t *) &ipc_data, STD_IPC_TYPE_keyboard ) ) return EMPTY;

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

	// input
	if( ui -> element[ ui -> element_active ] -> type == INPUT && ! (ui -> element[ ui -> element_active ] -> flag & LIB_UI_ELEMENT_FLAG_disabled) ) {
		struct LIB_UI_STRUCTURE_ELEMENT_INPUT *input = (struct LIB_UI_STRUCTURE_ELEMENT_INPUT *) ui -> element[ ui -> element_active ];

		input -> index = lib_input( input -> standard.name, LIB_UI_ELEMENT_INPUT_length_max, input -> index, keyboard -> key, ui -> keyboard.semaphore_ctrl_left );

		while( input -> offset > input -> index ) input -> offset--;
		uint64_t length = input -> index - input -> offset;
		while( lib_font_length_string( LIB_FONT_FAMILY_ROBOTO, input -> standard.name + input -> offset, length-- ) > (input -> standard.width - LIB_UI_PADDING_DEFAULT) ) input -> offset++;

		lib_ui_show_input( ui, input );

		*sync = TRUE;
	}

	// textarea
	if( ui -> element[ ui -> element_active ] -> type == TEXTAREA ) {
		struct LIB_UI_STRUCTURE_ELEMENT_TEXTAREA *textarea = (struct LIB_UI_STRUCTURE_ELEMENT_TEXTAREA *) ui -> element[ ui -> element_active ];

		// keys expanding textarea
		if( keyboard -> key == STD_ASCII_RETURN || ! (keyboard -> key < STD_ASCII_SPACE || keyboard -> key > STD_ASCII_TILDE) )
			textarea -> string = (uint8_t *) realloc( textarea -> string, textarea -> length + TRUE );

		switch( keyboard -> key ) {
			case STD_KEY_ARROW_DOWN: {
				uint64_t x = lib_string_length_line( (uint8_t *) &textarea -> string[ textarea -> pointer_line ] );

				// there is New Line character after current line?
				if( textarea -> pointer_line + x < textarea -> length ) {	// yes
					// move pointer at beginning of next line
					textarea -> pointer_line += x + 1;

					// length of next line
					uint64_t y = lib_string_length_line( (uint8_t *) &textarea -> string[ textarea -> pointer_line ] );

					// move pointer at beginning of next
					textarea -> pointer = EMPTY;

					// default cursor position
					textarea -> cursor_x = EMPTY;
					textarea -> cursor_y += LIB_FONT_HEIGHT_pixel;

					uint64_t line_length_in_font = lib_font_length_string( textarea -> font, (uint8_t *) &textarea -> string[ textarea -> pointer_line ], y );

					// if old cursor position 
					if( ! ui -> keyboard.semaphore_ctrl_left && textarea -> cursor_x_old <= line_length_in_font ) {
						// find a character which is near to old cursor x position
						while( textarea -> pointer < y ) {
							uint64_t x_offset = lib_font_length_char( textarea -> font, textarea -> string[ textarea -> pointer_line + textarea -> pointer ] );

							if( textarea -> cursor_x + x_offset > textarea -> cursor_x_old ) break;

							textarea -> pointer++;
							textarea -> cursor_x += x_offset;
						}
					// or 
					} else {
						textarea -> pointer = y;
						textarea -> cursor_x = line_length_in_font;

						if( ui -> keyboard.semaphore_ctrl_left ) textarea -> cursor_x_old = textarea -> cursor_x;
					}
				} else {
					textarea -> cursor_x = lib_font_length_string( textarea -> font, (uint8_t *) &textarea -> string[ textarea -> pointer_line ], x );

					textarea -> pointer = x;
				}

				break;
			}

			case STD_KEY_ARROW_LEFT: {
				// current line
				if( textarea -> pointer ) {
					textarea -> pointer--;

					textarea -> cursor_x_old = textarea -> cursor_x -= lib_font_length_char( textarea -> font, textarea -> string[ textarea -> pointer_line + textarea -> pointer ] );
				// or previous
				} else if( textarea -> pointer_line ) {
					// move pointer behind New Line character
					textarea -> pointer_line--;

					uint64_t x = lib_string_length_line_backward( (uint8_t *) &textarea -> string[ textarea -> pointer_line ], textarea -> pointer_line );
					textarea -> cursor_x_old = textarea -> cursor_x = lib_font_length_string( textarea -> font, (uint8_t *) &textarea -> string[ textarea -> pointer_line - x ], x );
					textarea -> cursor_y -= LIB_FONT_HEIGHT_pixel;

					textarea -> pointer_line -= x;
					textarea -> pointer = x;
				}

				if( ui -> keyboard.semaphore_ctrl_left ) {
					// trim
					while( textarea -> string[ textarea -> pointer_line + textarea -> pointer - 1 ] <= STD_ASCII_SPACE && textarea -> string[ textarea -> pointer_line + textarea -> pointer - 1 ] >= STD_ASCII_DELETE ) textarea -> pointer--;

					// look for beginning of word
					while( textarea -> string[ textarea -> pointer_line + textarea -> pointer - 1 ] > STD_ASCII_SPACE && textarea -> string[ textarea -> pointer_line + textarea -> pointer - 1 ] < STD_ASCII_DELETE ) textarea -> pointer--;

					textarea -> cursor_x_old = textarea -> cursor_x = lib_font_length_string( textarea -> font, (uint8_t *) &textarea -> string[ textarea -> pointer_line ], textarea -> pointer );
				}

				break;
			}

			case STD_KEY_ARROW_RIGHT: {
				if( textarea -> pointer_line + textarea -> pointer < textarea -> length ) {
					// next line
					if( textarea -> string[ textarea -> pointer_line + textarea -> pointer ] == STD_ASCII_NEW_LINE ) {
						textarea -> cursor_x_old = textarea -> cursor_x = EMPTY;
						textarea -> cursor_y += LIB_FONT_HEIGHT_pixel;

						textarea -> pointer_line += textarea -> pointer + 1;
						textarea -> pointer = EMPTY;

						// exception
						textarea -> offset_x = EMPTY;
					// or current
					} else {
						textarea -> cursor_x_old = textarea -> cursor_x += lib_font_length_char( textarea -> font, textarea -> string[ textarea -> pointer_line + textarea -> pointer ] );
					
						textarea -> pointer++;
					}

					if( ui -> keyboard.semaphore_ctrl_left ) {
						// trim
						while( textarea -> string[ textarea -> pointer_line + textarea -> pointer ] <= STD_ASCII_SPACE && textarea -> string[ textarea -> pointer_line + textarea -> pointer ] >= STD_ASCII_DELETE ) textarea -> pointer++;

						// look for end of word
						while( textarea -> string[ textarea -> pointer_line + textarea -> pointer ] > STD_ASCII_SPACE && textarea -> string[ textarea -> pointer_line + textarea -> pointer ] < STD_ASCII_DELETE ) textarea -> pointer++;

						textarea -> cursor_x_old = textarea -> cursor_x = lib_font_length_string( textarea -> font, (uint8_t *) &textarea -> string[ textarea -> pointer_line ], textarea -> pointer );
					}
				}

				break;
			}

			case STD_KEY_ARROW_UP: {
				if( textarea -> pointer_line ) {
					// move pointer at end of previous line
					textarea -> pointer_line--;

					// length of previous line
					uint64_t x = lib_string_length_line_backward( (uint8_t *) &textarea -> string[ textarea -> pointer_line ], textarea -> pointer_line );

					// move pointer at beginning of previous line
					textarea -> pointer_line -= x;
					textarea -> pointer = EMPTY;

					// default cursor position
					textarea -> cursor_x = EMPTY;
					textarea -> cursor_y -= LIB_FONT_HEIGHT_pixel;
					
					uint64_t line_length_in_font = lib_font_length_string( textarea -> font, (uint8_t *) &textarea -> string[ textarea -> pointer_line ], x );

					// if old cursor position 
					if( ! ui -> keyboard.semaphore_ctrl_left && textarea -> cursor_x_old <= line_length_in_font ) {
						// find a character which is near to old cursor x position
						while( textarea -> pointer < x ) {
							uint64_t x_offset = lib_font_length_char( textarea -> font, textarea -> string[ textarea -> pointer_line + textarea -> pointer ] );

							if( textarea -> cursor_x + x_offset > textarea -> cursor_x_old ) break;

							textarea -> pointer++;
							textarea -> cursor_x += x_offset;
						}
					// or 
					} else {
						textarea -> pointer = x;
						textarea -> cursor_x = line_length_in_font;

						if( ui -> keyboard.semaphore_ctrl_left ) {
							textarea -> pointer = EMPTY;
							textarea -> cursor_x_old = textarea -> cursor_x = EMPTY;
						}
					}
				} else {
					textarea -> pointer = EMPTY;

					textarea -> cursor_x = EMPTY;
				}

				break;
			}

			case STD_KEY_BACKSPACE: {
				if( textarea -> pointer_line || textarea -> pointer ) {
					// current line
					if( textarea -> pointer ) {
						textarea -> pointer--;

						textarea -> cursor_x_old = textarea -> cursor_x -= lib_font_length_char( textarea -> font, textarea -> string[ textarea -> pointer_line + textarea -> pointer ] );
					// or previous
					} else if( textarea -> pointer_line ) {
						// move pointer behind New Line character
						textarea -> pointer_line--;

						uint64_t x = lib_string_length_line_backward( (uint8_t *) &textarea -> string[ textarea -> pointer_line ], textarea -> pointer_line );
						textarea -> cursor_x_old = textarea -> cursor_x = lib_font_length_string( textarea -> font, (uint8_t *) &textarea -> string[ textarea -> pointer_line - x ], x );
						textarea -> cursor_y -= LIB_FONT_HEIGHT_pixel;

						// new beginning of line
						textarea -> pointer_line -= x;
						textarea -> pointer = x;
					}

					// remove character from document
					for( uint64_t i = textarea -> pointer_line + textarea -> pointer; i < textarea -> length; i++ ) textarea -> string[ i ] = textarea -> string[ i + 1 ]; textarea -> string[ --textarea -> length ] = STD_ASCII_TERMINATOR;
				}

				break;
			}

			case STD_KEY_DELETE: {
				if( textarea -> pointer_line + textarea -> pointer < textarea -> length ) {
					// remove character from document
					for( uint64_t i = textarea -> pointer_line + textarea -> pointer; i < textarea -> length; i++ ) textarea -> string[ i ] = textarea -> string[ i + 1 ];

					textarea -> string[ --textarea -> length ] = STD_ASCII_TERMINATOR;
				}

				break;
			}

			case STD_KEY_END: {
				uint64_t x = lib_string_length_line( (uint8_t *) &textarea -> string[ textarea -> pointer_line ] );
				textarea -> cursor_x_old = textarea -> cursor_x = lib_font_length_string( textarea -> font, (uint8_t *) &textarea -> string[ textarea -> pointer_line ], x );

				textarea -> pointer = x;

				break;
			}

			case STD_KEY_ENTER: {
				if( textarea -> pointer_line + textarea -> pointer < textarea -> length ) for( uint64_t i = textarea -> length; i > textarea -> pointer_line + textarea -> pointer; i-- ) textarea -> string[ i ] = textarea -> string[ i - 1 ];

				textarea -> string[ textarea -> pointer_line + textarea -> pointer++ ] = STD_ASCII_NEW_LINE;

				textarea -> cursor_x_old = textarea -> cursor_x = EMPTY;
				textarea -> cursor_y += LIB_FONT_HEIGHT_pixel;

				textarea -> pointer_line += textarea -> pointer;
				textarea -> pointer = EMPTY;

				textarea -> length++;

				break;
			}

			case STD_KEY_HOME: {
				textarea -> cursor_x_old = textarea -> cursor_x = EMPTY;

				textarea -> pointer = EMPTY;

				break;
			}

			case STD_KEY_PAGE_UP: {
				// calculate amount of lines to move up
				uint64_t height_in_lines = ((textarea -> height_current - (LIB_UI_PADDING_TEXTAREA << STD_SHIFT_2)) / LIB_FONT_HEIGHT_pixel) - 1;
				
				// until beginning of document
				while( textarea -> pointer_line ) {
					// we are already in first line?
					if( ! textarea -> cursor_y ) break;	// yes

					// count each line
					if( textarea -> string[ --textarea -> pointer_line ] == STD_ASCII_NEW_LINE ) {
						// with moving cursor up
						textarea -> cursor_y -= LIB_FONT_HEIGHT_pixel;

						// amount of line moved?
						if( ! --height_in_lines ) break;	// yea
					}
				}
				
				// set cursor at beginning of line
				textarea -> pointer_line -= lib_string_length_line_backward( (uint8_t *) &textarea -> string[ textarea -> pointer_line ], textarea -> pointer_line );
				textarea -> pointer = EMPTY;

				// with cursor
				textarea -> cursor_x_old = textarea -> cursor_x = EMPTY;

				break;
			}

			case STD_KEY_PAGE_DOWN: {
				// calculate amount of lines to move down
				uint64_t height_in_lines = ((textarea -> height_current - (LIB_UI_PADDING_TEXTAREA << STD_SHIFT_2)) / LIB_FONT_HEIGHT_pixel) - 1;

				// until end of document
				while( ++textarea -> pointer_line <= textarea -> length ) {
					// count each line
					if( textarea -> string[ textarea -> pointer_line - 1 ] == STD_ASCII_NEW_LINE ) {
						// with moving cursor down
						textarea -> cursor_y += LIB_FONT_HEIGHT_pixel;

						// amount of line moved?
						if( ! --height_in_lines ) break;	// yea
					}
				}

				// set cursor at beginning of line
				textarea -> pointer_line -= lib_string_length_line_backward( (uint8_t *) &textarea -> string[ textarea -> pointer_line ], textarea -> pointer_line );
				textarea -> pointer = EMPTY;

				// with cursor
				textarea -> cursor_x_old = textarea -> cursor_x = EMPTY;

				// done
				break;
			}

			default: {
				if( ! ui -> keyboard.semaphore_alt_left && ! ui -> keyboard.semaphore_ctrl_left && keyboard -> key >= STD_ASCII_SPACE && keyboard -> key <= STD_ASCII_TILDE ) {
					if( textarea -> pointer_line + textarea -> pointer < textarea -> length ) for( uint64_t i = textarea -> length; i > textarea -> pointer_line + textarea -> pointer; i-- ) textarea -> string[ i ] = textarea -> string[ i - 1 ];
	
					textarea -> string[ textarea -> pointer_line + textarea -> pointer++ ] = keyboard -> key;

					textarea -> cursor_x_old = textarea -> cursor_x += lib_font_length_char( textarea -> font, keyboard -> key );

					textarea -> length++;
				}
			}
		}

		// ignore key release
		if( ! (keyboard -> key & STD_KEY_RELEASE) ) {
			lib_ui_show_textarea( ui, textarea );

			*sync = TRUE;
		}
	}

	return keyboard -> key;
}

static void lib_ui_event_mouse( struct LIB_UI_STRUCTURE *ui, uint8_t *sync ) {
	uint8_t flush_element = FALSE;

	// incomming message
	uint8_t ipc_data[ STD_IPC_SIZE_byte ] = { EMPTY };

	// message properties
	struct STD_STRUCTURE_IPC_MOUSE *mouse = (struct STD_STRUCTURE_IPC_MOUSE *) &ipc_data;

	// receive pending messages of mouse
	std_ipc_receive_by_type( (uint8_t *) &ipc_data, STD_IPC_TYPE_mouse );

	if( mouse -> button == STD_IPC_MOUSE_BUTTON_left ) ui -> mouse.semaphore_left = TRUE;
	if( mouse -> button == (uint8_t) ~STD_IPC_MOUSE_BUTTON_left ) ui -> mouse.semaphore_left = FALSE;
	if( mouse -> button == STD_IPC_MOUSE_BUTTON_right ) ui -> mouse.semaphore_right = TRUE;
	if( mouse -> button == (uint8_t) ~STD_IPC_MOUSE_BUTTON_right ) ui -> mouse.semaphore_right = FALSE;

	for( uint64_t i = 0; i < ui -> limit; i++ ) {
		uint8_t flush_element = FALSE;

		// outside of element?
		if( ui -> window -> x < ui -> element[ i ] -> x || ui -> window -> x > (ui -> element[ i ] -> x + ui -> element[ i ] -> width) || ui -> window -> y < ui -> element[ i ] -> y || (ui -> window -> y > ui -> element[ i ] -> y + ui -> element[ i ] -> height) ) {
			switch( ui -> element[ i ] -> type ) {
				case LIST: {
					struct LIB_UI_STRUCTURE_ELEMENT_LIST *list = (struct LIB_UI_STRUCTURE_ELEMENT_LIST *) ui -> element[ i ];

					for( uint64_t y = 0; y < list -> limit_entry; y++ ) {
						if( list -> entry[ y ].flag & LIB_UI_ELEMENT_FLAG_hover ) flush_element = TRUE;

						list -> entry[ y ].flag &= ~LIB_UI_ELEMENT_FLAG_hover;
					}

					break;
				}

				default: {
					if( ui -> element[ i ] -> flag & LIB_UI_ELEMENT_FLAG_hover ) {
						ui -> element[ i ] -> flag ^= LIB_UI_ELEMENT_FLAG_hover;
				
						flush_element = TRUE;
					}
				}
			}
		} else {
			// inside of element

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

					case TEXTAREA: {
						// // slider bottom?
						// if( ui -> window -> y >= table -> standard.y + textarea -> standard.height - LIB_UI_SLIDER_SIZE) {
						// }

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
				switch( ui -> element[ i ] -> type ) {
					case LIST: {
						struct LIB_UI_STRUCTURE_ELEMENT_LIST *list = (struct LIB_UI_STRUCTURE_ELEMENT_LIST *) ui -> element[ i ];

						uint64_t r = ((ui -> window -> y - list -> standard.y) / LIB_UI_ELEMENT_LIST_ENTRY_height);

						list -> entry[ r ].flag = LIB_UI_ELEMENT_FLAG_event;

						break;
					}

					case RADIO: {
						struct LIB_UI_STRUCTURE_ELEMENT_RADIO *radio = (struct LIB_UI_STRUCTURE_ELEMENT_RADIO *) ui -> element[ i ];

						for( uint64_t j = 0; j < ui -> limit_radio; j++ )
							if( ui -> radio[ j ] -> group == radio -> group ) {
								ui -> radio[ j ] -> standard.flag &= ~LIB_UI_ELEMENT_FLAG_set;
								lib_ui_show_element( ui, (struct LIB_UI_STRUCTURE_ELEMENT *) ui -> radio[ j ] );
							}

						break;
					}

					default: {
						// nothing
					}
				}

				ui -> element[ i ] -> flag ^= LIB_UI_ELEMENT_FLAG_set;

				flush_element = TRUE;
			}

			switch( ui -> element[ i ] -> type ) {
				case LIST: {
					struct LIB_UI_STRUCTURE_ELEMENT_LIST *list = (struct LIB_UI_STRUCTURE_ELEMENT_LIST *) ui -> element[ i ];

					uint64_t r = ((ui -> window -> y - list -> standard.y) / LIB_UI_ELEMENT_LIST_ENTRY_height);

					if( ! (list -> entry[ r ].flag & LIB_UI_ELEMENT_FLAG_hover) ) {
						for( uint64_t y = 0; y < list -> limit_entry; y++ ) list -> entry[ y ].flag &= ~LIB_UI_ELEMENT_FLAG_hover;

						list -> entry[ r ].flag |= LIB_UI_ELEMENT_FLAG_hover;

						flush_element = TRUE;
					}

					break;
				}

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
	lib_ui_show_name( ui );

	for( uint64_t i = 0; i < ui -> limit; i++ )
		lib_ui_show_element( ui, ui -> element[ i ] );

	for( uint64_t i = 0; i < ui -> limit_label; i++ )
		lib_ui_show_label( ui, (struct LIB_UI_STRUCTURE_ELEMENT_LABEL *) ui -> label[ i ] );

	for( uint64_t i = 0; i < ui -> limit_control; i++ )
		lib_ui_show_control( ui, (struct LIB_UI_STRUCTURE_ELEMENT_CONTROL *) ui -> control[ i ] );
}

uint32_t *lib_ui_icon( uint8_t *path ) {
	// file properties
	FILE *file = EMPTY;

	// file exist?
	if( (file = fopen( path, EMPTY )) ) {
		// assign area for file
		struct LIB_IMAGE_STRUCTURE_TGA *image = (struct LIB_IMAGE_STRUCTURE_TGA *) malloc( MACRO_PAGE_ALIGN_UP( file -> byte ) >> STD_SHIFT_PAGE );

		// load file content
		fread( file, (uint8_t *) image, file -> byte );

		// copy image content to cursor object
		uint32_t *icon = (uint32_t *) malloc( image -> width * image -> height * STD_VIDEO_DEPTH_byte );
		lib_image_tga_parse( (uint8_t *) image, icon, file -> byte );

		// release file content
		free( image );

		// close file
		fclose( file );

		// done
		return icon;
	}

	// cannot locate specified file
	return EMPTY;
}

static void lib_ui_list_insert( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT *element ) {
	ui -> element = (struct LIB_UI_STRUCTURE_ELEMENT **) realloc( ui -> element, sizeof( struct LIB_UI_STRUCTURE_ELEMENT ) * (ui -> limit + TRUE) );

	ui -> element[ ui -> limit++ ] = element;
}

FILE *lib_ui_read_file( struct LIB_UI_STRUCTURE *ui ) {
	uint32_t color;
	for( uint64_t y = 0; y < ui -> window -> current_height; y++ ) {
		for( uint64_t x = 0; x < ui -> window -> current_width; x++ ) {
			if( ((x + y) / 20 ) % 2 ) color = 0x80202020;
			else color = 0x80181818;
			ui -> window -> pixel[ (y * ui -> window -> current_width) + x ] = lib_color_blend( ui -> window -> pixel[ (y * ui -> window -> current_width) + x ], color );
		}
	}

	ui -> window -> flags |= LIB_WINDOW_FLAG_flush;

	struct LIB_WINDOW_STRUCTURE *window = lib_window( -1, -1, 267, 283 );
	struct LIB_UI_STRUCTURE *internal = lib_ui( window );
	internal -> icon = lib_image_scale( lib_ui_icon( (uint8_t *) "/var/share/media/icon/default/app/system-file-manager.tga" ), 48, 48, 16, 16 );
	for( uint64_t i = 0; i < 16 * 16; i++ ) window -> icon[ i ] = internal -> icon[ i ];
	lib_ui_clean( internal );
	lib_window_name( internal -> window, (uint8_t *) "Open File" );
	lib_ui_add_control( internal, LIB_UI_ELEMENT_CONTROL_TYPE_close );
	lib_ui_flush( internal );
	window -> flags |= LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_icon | LIB_WINDOW_FLAG_resizable | LIB_WINDOW_FLAG_flush;

	while( TRUE ) {
		sleep( TRUE );

		// uint16_t key = lib_ui_event( internal );
	}

	return EMPTY;
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
	// lib_ui_fill_rectangle( pixel + ui -> window -> current_width, ui -> window -> current_width, LIB_UI_ELEMENT_LABEL_height >> TRUE, LIB_UI_ELEMENT_LABEL_height, LIB_UI_ELEMENT_LABEL_height, LIB_UI_COLOR_BACKGROUND_SHADOW );
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
		case LIST:	{ lib_ui_show_list( ui, (struct LIB_UI_STRUCTURE_ELEMENT_LIST *) element ); break; }
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

	// default
	pixel += LIB_UI_PADDING_DEFAULT;

	// if( input -> flag & LIB_FONT_FLAG_ALIGN_center ) pixel += -LIB_UI_PADDING_DEFAULT + (input -> standard.width >> STD_SHIFT_2);

	if( name_length_max ) lib_font( LIB_FONT_FAMILY_ROBOTO_MONO, input -> standard.name + input -> offset, name_length_max, color_foreground, pixel, ui -> window -> current_width, input -> flag );

	if( ! (input -> standard.flag & LIB_UI_ELEMENT_FLAG_active) || input -> standard.flag & LIB_UI_ELEMENT_FLAG_disabled ) return;

	uint64_t x = lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, input -> standard.name + input -> offset, input -> index - input -> offset );
	for( uint64_t y = 0; y < LIB_FONT_HEIGHT_pixel; y++ )
		pixel[ (y * ui -> window -> current_width) + x ] = STD_COLOR_WHITE;
}

void lib_ui_show_label( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT_LABEL *label ) {
	// set pointer location of element inside window
	uint32_t *pixel = ui -> window -> pixel + (label -> standard.y * ui -> window -> current_width) + label -> standard.x;

	if( label -> standard.height > LIB_FONT_HEIGHT_pixel ) pixel += ((label -> standard.height - LIB_FONT_HEIGHT_pixel) >> 1) * ui -> window -> current_width;

	if( label -> flag & LIB_FONT_FLAG_ALIGN_center ) pixel += label -> standard.width >> STD_SHIFT_2;

	// show description
	lib_font( LIB_FONT_FAMILY_ROBOTO, label -> standard.name, lib_string_length( label -> standard.name ), LIB_UI_COLOR_DEFAULT, pixel, ui -> window -> current_width, label -> flag );
}

void lib_ui_show_list( struct LIB_UI_STRUCTURE *ui, struct LIB_UI_STRUCTURE_ELEMENT_LIST *list ) {
	// set pointer location of element inside window
	uint32_t *pixel = ui -> window -> pixel + (list -> standard.y * ui -> window -> current_width) + list -> standard.x;

	uint16_t list_width = list -> standard.width; if( list -> standard.width == (uint16_t) STD_MAX_unsigned ) list_width = ui -> window -> current_width - list -> standard.x - LIB_UI_MARGIN_DEFAULT;
	uint16_t list_height = list -> standard.height; if( list -> standard.height == (uint16_t) STD_MAX_unsigned ) list_height = ui -> window -> current_height - list -> standard.y - LIB_UI_MARGIN_DEFAULT;

	lib_ui_fill_rectangle( pixel, ui -> window -> current_width, EMPTY, list_width, list_height, LIB_UI_COLOR_BACKGROUND_LIST );

	for( uint64_t i = 0; i < list -> limit_entry; i++ ) {
		struct LIB_UI_STRUCTURE_ELEMENT_LIST_ENTRY *entry = (struct LIB_UI_STRUCTURE_ELEMENT_LIST_ENTRY *) &list -> entry[ i ];

		uint32_t color_foreground = STD_COLOR_WHITE;
		uint32_t color_background = LIB_UI_COLOR_BACKGROUND_LIST;
		if( entry -> flag & LIB_UI_ELEMENT_FLAG_active || entry -> flag & LIB_UI_ELEMENT_FLAG_hover ) color_background = LIB_UI_COLOR_BACKGROUND_LIST_HOVER;

		uint32_t *offset = pixel + (i * LIB_UI_ELEMENT_LIST_ENTRY_height * ui -> window -> current_width);

		// clean area
		lib_ui_fill_rectangle( offset - (LIB_UI_PADDING_DEFAULT >> STD_SHIFT_2), ui -> window -> current_width, LIB_UI_RADIUS_DEFAULT, list_width + LIB_UI_PADDING_DEFAULT, LIB_UI_ELEMENT_LIST_ENTRY_height, color_background );

		if( LIB_UI_ELEMENT_LIST_ENTRY_height > LIB_FONT_HEIGHT_pixel ) offset += ((LIB_UI_ELEMENT_LIST_ENTRY_height - LIB_FONT_HEIGHT_pixel) >> STD_SHIFT_2) * ui -> window -> current_width;

		if( entry -> icon ) {
			for( uint64_t y = 0; y < 16; y++ )
				for( uint64_t x = 0; x < 16; x++ )
					offset[ (y * ui -> window -> current_width) + x ] = lib_color_blend( offset[ (y * ui -> window -> current_width) + x ], entry -> icon[ (y * 16) + x ] );

			offset += 16 + LIB_UI_PADDING_DEFAULT;
		}

		lib_font( LIB_FONT_FAMILY_ROBOTO, entry -> name, lib_string_length( entry -> name ), color_foreground, offset, ui -> window -> current_width, EMPTY );

		if( entry -> shortcut ) {
			offset = pixel + (i * LIB_UI_ELEMENT_LIST_ENTRY_height * ui -> window -> current_width) + list_width;
			if( LIB_UI_ELEMENT_LIST_ENTRY_height > LIB_FONT_HEIGHT_pixel ) offset += ((LIB_UI_ELEMENT_LIST_ENTRY_height - LIB_FONT_HEIGHT_pixel) >> STD_SHIFT_2) * ui -> window -> current_width;
			
			lib_font( LIB_FONT_FAMILY_ROBOTO, entry -> shortcut, lib_string_length( entry -> shortcut ), STD_COLOR_GRAY, offset, ui -> window -> current_width, LIB_FONT_FLAG_ALIGN_right );
		}
	}
}

void lib_ui_show_name( struct LIB_UI_STRUCTURE *ui ) {
	uint64_t offset = EMPTY;

	// set pointer location of element inside window
	uint32_t *pixel = ui -> window -> pixel + (((LIB_UI_HEADER_HEIGHT - LIB_FONT_HEIGHT_pixel) >> TRUE) * ui -> window -> current_width) + LIB_UI_MARGIN_DEFAULT;

	// limit name length to header width
	uint64_t limit = lib_ui_string( LIB_FONT_FAMILY_ROBOTO, ui -> window -> name, ui -> window -> name_length, ui -> window -> current_width - LIB_UI_MARGIN_DEFAULT - LIB_UI_PADDING_DEFAULT - (ui -> limit_control * LIB_UI_HEADER_HEIGHT) - offset );

	if( ! limit ) return;

	// clean area
	lib_ui_fill_rectangle( pixel, ui -> window -> current_width, 0, ui -> window -> current_width - LIB_UI_MARGIN_DEFAULT - LIB_UI_PADDING_DEFAULT - (ui -> limit_control * LIB_UI_HEADER_HEIGHT), LIB_FONT_HEIGHT_pixel, LIB_UI_COLOR_BACKGROUND_DEFAULT );

	// icon exist?
	if( ui -> icon ) {
		offset = 16 + LIB_UI_PADDING_DEFAULT;

		for( uint64_t y = 0; y < 16; y++ )
			for( uint64_t x = 0; x < 16; x++ )
				pixel[ (y * ui -> window -> current_width) + x ] = lib_color_blend( pixel[ (y * ui -> window -> current_width) + x ], ui -> icon[ (y * 16) + x ] );
	}

	uint32_t color = LIB_UI_COLOR_INACTIVE;
	if( ui -> window -> flags & LIB_WINDOW_FLAG_active ) color = LIB_UI_COLOR_DEFAULT;

	// show description
	lib_font( LIB_FONT_FAMILY_ROBOTO, ui -> window -> name, limit, color, pixel + ui -> window -> current_width + offset, ui -> window -> current_width, LIB_FONT_FLAG_ALIGN_left );

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

	// lib_ui_fill_rectangle( table -> pixel, content_width, EMPTY, content_width, LIB_UI_ELEMENT_TABLE_height, LIB_UI_COLOR_BACKGROUND_TABLE_HEADER );
	lib_ui_fill_rectangle( table -> pixel, content_width, EMPTY, content_width, content_height, LIB_UI_COLOR_BACKGROUND_TABLE_ROW );

	for( uint64_t x = 0; x < table -> limit_column; x++ ) {
		if( ! table -> header[ x ].cell.name ) continue;

		uint64_t table_width_column = content_width / table -> limit_column;
		if( content_width != table_width ) table_width_column = table -> header[ x ].width;

		uint32_t *pixel_table_header = table -> pixel + (table_width_column * x);
		if( LIB_FONT_HEIGHT_pixel < LIB_UI_ELEMENT_TABLE_height ) pixel_table_header += ((LIB_UI_ELEMENT_TABLE_height - LIB_FONT_HEIGHT_pixel) >> TRUE) * content_width;

		if( table -> header[ x ].cell.flag & LIB_FONT_FLAG_ALIGN_right ) pixel_table_header += table_width_column - LIB_UI_PADDING_TABLE;
		else pixel_table_header += LIB_UI_PADDING_TABLE;

		uint64_t limit = lib_ui_string( LIB_FONT_FAMILY_ROBOTO, table -> header[ x ].cell.name, lib_string_length( table -> header[ x ].cell.name ), table_width_column );
		lib_font( LIB_FONT_FAMILY_ROBOTO, table -> header[ x ].cell.name, limit, LIB_UI_COLOR_DEFAULT, pixel_table_header, content_width, table -> header[ x ].cell.flag );
	}

	for( uint64_t y = 0; y < table -> limit_row; y++ ) {
		uint32_t *pixel_table_row = table -> pixel + (LIB_UI_ELEMENT_TABLE_height * table_width) + (LIB_UI_ELEMENT_TABLE_height * content_width * y);

		uint32_t color_foreground = STD_COLOR_WHITE;

		color_background = LIB_UI_COLOR_BACKGROUND_TABLE_ROW;
		if( table -> row[ y ].flag & LIB_UI_ELEMENT_FLAG_set ) { color_background = LIB_UI_COLOR_BACKGROUND_TABLE_ROW_SET; color_foreground = STD_COLOR_BLACK; }
		if( table -> row[ y ].flag & LIB_UI_ELEMENT_FLAG_hover ) color_background += LIB_UI_COLOR_INCREASE;
		lib_ui_fill_rectangle( pixel_table_row, content_width, EMPTY, content_width, LIB_UI_ELEMENT_TABLE_height - 1, color_background );

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
				lib_font( LIB_FONT_FAMILY_ROBOTO, table -> row[ y ].cell[ x ].name, limit, color_foreground, pixel_table_cell + offset, content_width, table -> row[ y ].cell[ x ].flag );
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
	// calculate current dimension of element
	textarea -> width_current	= textarea -> standard.width; if( textarea -> standard.width == (uint16_t) STD_MAX_unsigned ) textarea -> width_current = ui -> window -> current_width - textarea -> standard.x - LIB_UI_MARGIN_DEFAULT;
	textarea -> height_current	= textarea -> standard.height; if( textarea -> standard.height == (uint16_t) STD_MAX_unsigned ) textarea -> height_current = ui -> window -> current_height - textarea -> standard.y - LIB_UI_MARGIN_DEFAULT;

	// by default, element is enabled
	uint32_t color_background = STD_COLOR_WHITE;
	uint32_t color_foreground = LIB_UI_COLOR_BACKGROUND_TEXTAREA;

	// or set it as disbaled?
	if( textarea -> standard.flag & LIB_UI_ELEMENT_FLAG_disabled ) {
		color_background = STD_COLOR_GRAY_LIGHT;
		color_foreground = LIB_UI_COLOR_BACKGROUND_TEXTAREA + LIB_UI_COLOR_INCREASE_LITTLE;
	}

	// set pointer location of element inside window
	uint32_t *element = ui -> window -> pixel + (textarea -> standard.y * ui -> window -> current_width) + textarea -> standard.x;

	// set default background color
	lib_ui_fill_rectangle( element, ui -> window -> current_width, EMPTY, textarea -> width_current, textarea -> height_current, color_foreground );

	// show border around element (it's not part of element itself)
	for( uint64_t y = 0; y < textarea -> height_current + 1; y++ ) for( uint64_t x = 0; x < textarea -> width_current + 1; x++ ) { if( ! x || ! y ) element[ ((y - 1) * ui -> window -> current_width) + x - 1 ] = color_foreground - LIB_UI_COLOR_INCREASE_LITTLE; if( x == textarea -> width_current || y == textarea -> height_current ) element[ (y * ui -> window -> current_width) + x ] = color_foreground + LIB_UI_COLOR_INCREASE_LITTLE; }

	//---------------------------------------------------------------------

	// by default, plane will have same width as element
	uint64_t plane_width = EMPTY;

	// counted number of lines
	uint64_t lines = TRUE;

	// local variable
	uint8_t *string = textarea -> string;

	// find longest line inside string
	for( uint64_t i = 0; i < textarea -> length; i++ ) {
		// count current line characters
		uint64_t c = lib_string_length_line( (uint8_t *) &string[ i ] );

		// convert to pixels (in point of used font)
		uint64_t p = lib_font_length_string( textarea -> font, (uint8_t *) &string[ i ], c );

		// expand plane horizontaly?
		if( plane_width < p ) plane_width = p;	// yes

		// New Line character exist?
		if( i + c < textarea -> length && textarea -> string[ i + c ] == STD_ASCII_NEW_LINE )	// yes
			// count line
			lines++;

		// next line
		i += c;
	}

	// expand plane width by padding and element
	plane_width += LIB_UI_PADDING_TEXTAREA << STD_SHIFT_2;

	// height of plane calculated from amount of lines
	uint64_t plane_height = (lines * LIB_FONT_HEIGHT_pixel) + (LIB_UI_PADDING_TEXTAREA << STD_SHIFT_2);

	// correct plane width/height up to element dimension
	if( plane_width < textarea -> width_current ) plane_width = textarea -> width_current;
	if( plane_height < textarea -> height_current ) plane_height = textarea -> height_current;

	//---------------------------------------------------------------------

	// acquire plane area
	if( textarea -> plane ) free( textarea -> plane );
	textarea -> plane = (uint32_t *) malloc( (plane_width * plane_height) << STD_VIDEO_DEPTH_shift );

	// local value
	uint32_t *plane = textarea -> plane;

	// set default background color
	lib_ui_fill_rectangle( plane, plane_width, EMPTY, plane_width, plane_height, color_foreground );

	// apply padding
	plane += (LIB_UI_PADDING_TEXTAREA * plane_width) + LIB_UI_PADDING_TEXTAREA;

	// print string inside plane
	for( uint64_t i = 0; i <= textarea -> length; i++ ) {
		// current line length
		uint64_t c = lib_string_length_line( (uint8_t *) &string[ i ] );

		// change background color for current line
		if( textarea -> pointer_line == i ) lib_ui_fill_rectangle( plane - LIB_UI_PADDING_TEXTAREA, plane_width, EMPTY, plane_width, LIB_FONT_HEIGHT_pixel, color_foreground + LIB_UI_COLOR_INCREASE_LITTLE );

		// if empty, ignore
		if( c ) lib_font( textarea -> font, (uint8_t *) &string[ i ], c, color_background, plane, plane_width, EMPTY );

		// next line inside plane
		plane += LIB_FONT_HEIGHT_pixel * plane_width;

		// next line
		i += c;
	}

	//---------------------------------------------------------------------
	// offset
	//---------------------------------------------------------------------

		// todo:
		// zapamietaj pozycje starą offset x/y gdy trzymasz slider i przywróć gdy nacisniesz jakikolwiek klawisz

	uint64_t x = textarea -> cursor_x + LIB_UI_PADDING_TEXTAREA;
	uint64_t y = textarea -> cursor_y + LIB_UI_PADDING_TEXTAREA;

	// offset of X axis
	if( x > textarea -> offset_x && (x - textarea -> offset_x >= textarea -> width_current - LIB_UI_PADDING_TEXTAREA) ) textarea -> offset_x = (x - textarea -> width_current) + LIB_UI_PADDING_TEXTAREA + TRUE;
	if( x - LIB_UI_PADDING_TEXTAREA < textarea -> offset_x ) textarea -> offset_x = x - LIB_UI_PADDING_TEXTAREA;

	// offset of Y axis
	if( y > textarea -> offset_y && ((y + LIB_FONT_HEIGHT_pixel) - textarea -> offset_y >= textarea -> height_current - LIB_UI_PADDING_TEXTAREA) ) textarea -> offset_y = ((y + LIB_FONT_HEIGHT_pixel) - textarea -> height_current) + LIB_UI_PADDING_TEXTAREA;
	if( y < textarea -> offset_y ) textarea -> offset_y = y - LIB_UI_PADDING_TEXTAREA;

	//---------------------------------------------------------------------
	// cursor
	//---------------------------------------------------------------------

	// element enabled?
	if( ! (textarea -> standard.flag & LIB_UI_ELEMENT_FLAG_disabled) )
		// show cursor at position
		for( uint64_t i = y; i < y + LIB_FONT_HEIGHT_pixel; i++ ) textarea -> plane[ (i * plane_width) + x ] = ~textarea -> plane[ (i * plane_width) + x ] | STD_COLOR_mask;

	//---------------------------------------------------------------------
	// sync
	//---------------------------------------------------------------------

	// copy only part of plane to element based on element dimension
	uint64_t limit_x = textarea -> width_current;
	uint64_t limit_y = textarea -> height_current;

	// and offsets
	uint32_t *plane_offset = (uint32_t *) textarea -> plane + (textarea -> offset_y * plane_width) + textarea -> offset_x;

	// copy
	for( uint64_t y = 0; y < limit_y && textarea -> offset_y + y < plane_height; y++ ) for( uint64_t x = 0; x < limit_x && textarea -> offset_x + x < plane_width; x++ ) element[ (y * ui -> window -> current_width) + x ] = plane_offset[ (y * plane_width) + x ];

	//---------------------------------------------------------------------
	// sliders
	//---------------------------------------------------------------------

	// slider bottom
	if( plane_width > textarea -> width_current ) {
		double percent_offset = textarea -> offset_x / (double) plane_width;
		textarea -> slider_x = textarea -> width_current * percent_offset;
		
		double percent = textarea -> width_current / (double) plane_width;
		textarea -> slider_width = textarea -> width_current * percent;

		if( textarea -> slider_x + textarea -> slider_width > textarea -> width_current) textarea -> slider_x -= (textarea -> slider_x + textarea -> slider_width) - textarea -> width_current;

		for( uint64_t y = textarea -> height_current - LIB_UI_SLIDER_SIZE; y < textarea -> height_current; y++ ) {
			for( uint64_t x = textarea -> slider_x; x < (textarea -> slider_x + textarea -> slider_width); x++ ) {
				uint32_t target = element[ (y * ui -> window -> current_width) + x ];
				element[ (y * ui -> window -> current_width) + x ] = lib_color_blend( target, LUB_UI_COLOR_BACKGROUND_SLIDER );
			}
		}
	}

	// slider right
	if( plane_height > textarea -> height_current ) {
		double percent_offset = textarea -> offset_y / (double) plane_height;
		textarea -> slider_y = textarea -> height_current * percent_offset;
		
		double percent = textarea -> height_current / (double) plane_height;
		textarea -> slider_height = textarea -> height_current * percent;

		if( textarea -> slider_y + textarea -> slider_height > textarea -> height_current ) textarea -> slider_y -= (textarea -> slider_y + textarea -> slider_height) - textarea -> height_current;

		for( uint64_t y = textarea -> slider_y; y < textarea -> slider_y + textarea -> slider_height; y++ ) {
			for( uint64_t x = textarea -> width_current - LIB_UI_SLIDER_SIZE; x < textarea -> width_current; x++ ) {
				uint32_t target = element[ (y * ui -> window -> current_width) + x ];
				element[ (y * ui -> window -> current_width) + x ] = lib_color_blend( target, LUB_UI_COLOR_BACKGROUND_SLIDER );
			}
		}
	}
}

static uint64_t lib_ui_string( uint8_t font_family, uint8_t *string, uint64_t limit, uint64_t width_pixel ) {
	// set max length of string
	while( lib_font_length_string( font_family, string, limit ) > width_pixel ) if( ! --limit ) break;

	// new string limit
	return limit;
}

void lib_ui_update_input( struct LIB_UI_STRUCTURE *ui, uint64_t id, uint8_t *name ) {
	if( id >= ui -> limit_input ) return;

	if( *name ) {
		memcpy( ui -> input[ id ] -> standard.name, name, lib_string_length( name ) );

		ui -> input[ id ] -> standard.name[ lib_string_length( name ) ] = STD_ASCII_TERMINATOR;
	}
}

void lib_ui_update_table( struct LIB_UI_STRUCTURE *ui, uint64_t id, struct LIB_UI_STRUCTURE_ELEMENT_TABLE_ROW *row, uint64_t r ) {
	ui -> table[ id ] -> limit_row = r;
	ui -> table[ id ] -> row = row;

	ui -> table[ id ] -> offset_x = EMPTY;
	ui -> table[ id ] -> offset_y = EMPTY;
}
