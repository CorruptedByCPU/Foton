/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

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
	uint8_t *table_example[ 19 ][ 4 ] = {
		{ (uint8_t *) "FileID", (uint8_t *) "Filename", (uint8_t *) "SizeKB", (uint8_t *) "Type" },
		{ (uint8_t *) "1", (uint8_t *) "main.c", (uint8_t *) "15", (uint8_t *) "C Source" },
		{ (uint8_t *) "2", (uint8_t *) "readme.md", (uint8_t *) "2", (uint8_t *) "Markdown" },
		{ (uint8_t *) "3", (uint8_t *) "data.csv", (uint8_t *) "42", (uint8_t *) "CSV" },
		{ (uint8_t *) "4", (uint8_t *) "icon.png", (uint8_t *) "128", (uint8_t *) "Image" },
		{ (uint8_t *) "5", (uint8_t *) "notes.txt", (uint8_t *) "8", (uint8_t *) "Text" },
		{ (uint8_t *) "6", (uint8_t *) "archive.zip", (uint8_t *) "300", (uint8_t *) "ZIP Archive" },
		{ (uint8_t *) "7", (uint8_t *) "presentation.pptx", (uint8_t *) "2560", (uint8_t *) "PowerPoint" },
		{ (uint8_t *) "8", (uint8_t *) "report.pdf", (uint8_t *) "544", (uint8_t *) "PDF" },
		{ (uint8_t *) "9", (uint8_t *) "logo.svg", (uint8_t *) "3", (uint8_t *) "SVG" },
		{ (uint8_t *) "10", (uint8_t *) "src", (uint8_t *) "—", (uint8_t *) "Directory" },
		{ (uint8_t *) "11", (uint8_t *) "assets", (uint8_t *) "—", (uint8_t *) "Directory" },
		{ (uint8_t *) "12", (uint8_t *) "build", (uint8_t *) "—", (uint8_t *) "Directory" },
		{ (uint8_t *) "13", (uint8_t *) "docs", (uint8_t *) "—", (uint8_t *) "Directory" },
		{ (uint8_t *) "14", (uint8_t *) "Makefile", (uint8_t *) "1", (uint8_t *) "Makefile" },
		{ (uint8_t *) "15", (uint8_t *) "script.sh", (uint8_t *) "6", (uint8_t *) "Shell Script" },
		{ (uint8_t *) "16", (uint8_t *) "config.json", (uint8_t *) "4", (uint8_t *) "JSON" },
		{ (uint8_t *) "17", (uint8_t *) "LICENSE", (uint8_t *) "1", (uint8_t *) "Text" },
		{ (uint8_t *) "18", (uint8_t *) "bin", (uint8_t *) "—", (uint8_t *) "Directory" }
	};
	uint8_t ***table_content = (uint8_t ***) malloc( 19 * sizeof( uint8_t ** ) );
	for( uint8_t y = 0; y < 19; y++ ) {
		table_content[ y ] = (uint8_t **) malloc( 4 * sizeof( uint8_t * ) );

		for( uint8_t x = 0; x < 4; x++ ) {
			table_content[ y ][ x ] = (uint8_t *) calloc( lib_string_length( table_example[ y ][ x ] ) + TRUE );

			for( uint8_t n = 0; n < lib_string_length( table_example[ y ][ x ] ); n++ ) table_content[ y ][ x ][ n ] = table_example[ y ][ x ][ n ];
		}
	}
	lib_ui_add_table( ui, x2, y2, -1, TEST_HEIGHT_pixel - y2 - LIB_UI_MARGIN_DEFAULT, EMPTY, table_content, 4, 19 );

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
