/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#include	"../library/integer.h"
#include	"../library/ui.h"
#include	"../library/window.h"

#define	TEST_WIDTH_pixel	600
#define	TEST_HEIGHT_pixel	200

struct LIB_UI_STRUCTURE *ui;

void create_ui( void ) {
	// column width
	uint64_t column_width = (320 - (LIB_UI_MARGIN_DEFAULT * 3)) >> STD_SHIFT_2;

	// column 0
	uint64_t x0 = LIB_UI_MARGIN_DEFAULT;
	uint64_t y0 = LIB_UI_HEADER_HEIGHT + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_label( ui, x0, y0, column_width, (uint8_t *) "Use TAB/SHIFT key or mouse for interaction." );
	y0 += LIB_UI_ELEMENT_LABEL_height + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_input( ui, x0, y0, column_width, (uint8_t *) "Input.0", EMPTY );
	y0 += LIB_UI_ELEMENT_INPUT_height + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_checkbox( ui, x0, y0, column_width, (uint8_t *) "Checkbox.0", EMPTY );
	y0 += LIB_UI_ELEMENT_CHECKBOX_height + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_checkbox( ui, x0, y0, column_width, (uint8_t *) "Checkbox.1", LIB_UI_ELEMENT_FLAG_set );
	y0 += LIB_UI_ELEMENT_CHECKBOX_height + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_button( ui, x0, y0, column_width, (uint8_t *) "Button.0", LIB_UI_ELEMENT_BUTTON_height, EMPTY );
	y0 += LIB_UI_ELEMENT_BUTTON_height + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_button( ui, x0, y0, column_width, (uint8_t *) "Button.1", LIB_UI_ELEMENT_BUTTON_height, EMPTY );
	y0 += LIB_UI_ELEMENT_BUTTON_height + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_button( ui, x0, y0, column_width, (uint8_t *) "Button.2 (disabled)", LIB_UI_ELEMENT_BUTTON_height, LIB_UI_ELEMENT_FLAG_disabled );

	// column 1
	uint64_t x1 = LIB_UI_MARGIN_DEFAULT + column_width + LIB_UI_PADDING_DEFAULT;
	uint64_t y1 = LIB_UI_HEADER_HEIGHT + LIB_UI_PADDING_DEFAULT + LIB_UI_ELEMENT_LABEL_height + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_input( ui, x1, y1, column_width, (uint8_t *) "Input.1 (disabled)", LIB_UI_ELEMENT_FLAG_disabled );
	y1 += LIB_UI_ELEMENT_INPUT_height + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_radio( ui, x1, y1, column_width, (uint8_t *) "Radio.0 (Group.0)", 0, EMPTY );
	y1 += LIB_UI_ELEMENT_RADIO_height + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_radio( ui, x1, y1, column_width, (uint8_t *) "Radio.1 (Group.0)", 0, EMPTY );
	y1 += LIB_UI_ELEMENT_RADIO_height + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_radio( ui, x1, y1, column_width, (uint8_t *) "Radio.2 (Group.0)", 0, LIB_UI_ELEMENT_FLAG_set );
	y1 += LIB_UI_ELEMENT_RADIO_height + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_radio( ui, x1, y1, column_width, (uint8_t *) "Radio.3 (Group.0)", 0, EMPTY );
	y1 += LIB_UI_ELEMENT_RADIO_height + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_radio( ui, x1, y1, column_width, (uint8_t *) "Radio.0 (Group.1)", 1, EMPTY );
	y1 += LIB_UI_ELEMENT_RADIO_height + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_radio( ui, x1, y1, column_width, (uint8_t *) "Radio.1 (Group.1)", 1, LIB_UI_ELEMENT_FLAG_set );

	// column 2
	uint64_t x2 = LIB_UI_MARGIN_DEFAULT + column_width + LIB_UI_PADDING_DEFAULT + column_width + LIB_UI_PADDING_DEFAULT;
	uint64_t y2 = LIB_UI_HEADER_HEIGHT;
	struct STD_STRUCTURE_DIR *dir = (struct STD_STRUCTURE_DIR *) std_dir( (uint8_t *) ".", TRUE );
	struct LIB_UI_STRUCTURE_ELEMENT_TABLE_HEADER *table_header = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE_HEADER *) malloc( 2 * sizeof( struct LIB_UI_STRUCTURE_ELEMENT_TABLE_HEADER ) );
	// header
		// subcolumn 0
		table_header[ 0 ].width	= EMPTY;
		table_header[ 0 ].cell.flag	= EMPTY;
		table_header[ 0 ].cell.name	= (uint8_t *) calloc( 5 );
		uint8_t name[ 4 ] = "Name";
		for( uint8_t i = 0; i < 4; i++ ) table_header[ 0 ].cell.name[ i ] = name[ i ];
		table_header[ 0 ].cell.icon	= EMPTY;
		// subcolumn 1
		table_header[ 1 ].width	= EMPTY;
		table_header[ 1 ].cell.flag	= EMPTY;
		table_header[ 1 ].cell.name	= (uint8_t *) calloc( 5 );
		uint8_t size[ 4 ] = "Size";
		for( uint8_t i = 0; i < 4; i++ ) table_header[ 1 ].cell.name[ i ] = size[ i ];
		table_header[ 1 ].cell.icon	= EMPTY;
	// rows
	struct LIB_UI_STRUCTURE_ELEMENT_TABLE_ROW *table_content = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE_ROW *) malloc( FALSE );
	uint64_t y = 0;
	while( dir[ y ].type ) {
		table_content = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE_ROW *) realloc( table_content, (y + 1) * sizeof( struct LIB_UI_STRUCTURE_ELEMENT_TABLE_ROW ) );
		//---
		table_content[ y ].cell = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE_CELL *) malloc( 2 * sizeof( struct LIB_UI_STRUCTURE_ELEMENT_TABLE_CELL ) );
		table_content[ y ].flag = EMPTY;
		table_content[ y ].cell[ 0 ].flag = EMPTY;
		table_content[ y ].cell[ 0 ].name = (uint8_t *) calloc( dir[ y ].name_limit + TRUE );
		for( uint64_t i = 0; i < dir[ y ].name_limit; i++ ) table_content[ y ].cell[ 0 ].name[ i ] = dir[ y ].name[ i ];
		table_content[ y ].cell[ 0 ].icon = EMPTY;
		//---
		table_content[ y ].cell[ 1 ].flag = LIB_FONT_FLAG_ALIGN_right;
		if( dir[ y ].type & STD_FILE_TYPE_link || dir[ y ].type & STD_FILE_TYPE_directory ) table_content[ y ].cell[ 1 ].name = EMPTY;
		else {
			table_content[ y ].cell[ 1 ].name = (uint8_t *) calloc( lib_integer_digit_count( dir[ y ].limit, STD_NUMBER_SYSTEM_decimal ) + TRUE );
			lib_integer_to_string( dir[ y ].limit, STD_NUMBER_SYSTEM_decimal, table_content[ y ].cell[ 1 ].name );
		}
		table_content[ y ].cell[ 1 ].icon = EMPTY;
		//---
		y++;
	}
	lib_ui_add_table( ui, x2, y2, -1, TEST_HEIGHT_pixel - y2 - LIB_UI_MARGIN_DEFAULT, EMPTY, table_header, table_content, 2, y );

	lib_window_name( ui -> window, (uint8_t *) "GUI Debug Window" );

	lib_ui_add_control( ui, LIB_UI_ELEMENT_CONTROL_TYPE_close );
	lib_ui_add_control( ui, LIB_UI_ELEMENT_CONTROL_TYPE_max );
	lib_ui_add_control( ui, LIB_UI_ELEMENT_CONTROL_TYPE_min );

	lib_ui_flush( ui );
}

uint64_t _main( uint64_t argc, uint8_t *argv[] ) {
	struct LIB_WINDOW_STRUCTURE *window = lib_window( -1, -1, TEST_WIDTH_pixel, TEST_HEIGHT_pixel );
	window -> flags = LIB_WINDOW_FLAG_resizable;

	ui = lib_ui( window );
	lib_ui_clean( ui );

	create_ui();

	ui -> window -> flags |= LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_flush;

	while( TRUE ) { lib_ui_event( ui ); sleep( TRUE ); }

	return 0;
}
