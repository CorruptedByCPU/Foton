/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#include	"../library/ui.h"
#include	"../library/window.h"

struct LIB_UI_STRUCTURE *ui;

void create_ui( void ) {
	// column width
	uint64_t width = (ui -> window -> width - (LIB_UI_MARGIN_DEFAULT * 3)) >> STD_SHIFT_2;

	// column 0
	uint64_t x0 = LIB_UI_MARGIN_DEFAULT;
	uint64_t y0 = LIB_UI_HEADER_HEIGHT + LIB_UI_MARGIN_DEFAULT;
	lib_ui_show_label( ui, lib_ui_add_label( ui, x0, y0, (uint8_t *) "Use TAB/SHIFT key to move around." ) );
	y0 += LIB_UI_LABEL_HEIGHT + LIB_UI_MARGIN_DEFAULT;
	lib_ui_show_checkbox( ui, lib_ui_add_checkbox( ui, x0, y0, width, (uint8_t *) "Checkbox.0" ) );
	y0 += LIB_UI_CHECKBOX_HEIGHT + LIB_UI_MARGIN_DEFAULT;
	lib_ui_show_checkbox( ui, lib_ui_add_checkbox( ui, x0, y0, width, (uint8_t *) "Checkbox.1" ) );
	y0 += LIB_UI_CHECKBOX_HEIGHT + LIB_UI_MARGIN_DEFAULT;
	lib_ui_show_button( ui, lib_ui_add_button( ui, x0, y0, width, LIB_UI_BUTTON_HEIGHT, (uint8_t *) "Button.0" ) );
	y0 += LIB_UI_BUTTON_HEIGHT + LIB_UI_MARGIN_DEFAULT;
	lib_ui_show_button( ui, lib_ui_add_button( ui, x0, y0, width, LIB_UI_BUTTON_HEIGHT, (uint8_t *) "Button.1" ) );
	y0 += LIB_UI_BUTTON_HEIGHT + LIB_UI_MARGIN_DEFAULT;
	lib_ui_show_button( ui, lib_ui_add_button( ui, x0, y0, width, LIB_UI_BUTTON_HEIGHT, (uint8_t *) "Button.2" ) );

	// column 1
	uint64_t x1 = LIB_UI_MARGIN_DEFAULT + width + LIB_UI_MARGIN_DEFAULT;
	uint64_t y1 = LIB_UI_HEADER_HEIGHT + LIB_UI_MARGIN_DEFAULT + LIB_UI_LABEL_HEIGHT + LIB_UI_MARGIN_DEFAULT;
	lib_ui_show_radio( ui, lib_ui_add_radio( ui, x1, y1, width, 0, (uint8_t *) "Radio.0 (Group.0)" ) );
	y1 += LIB_UI_RADIO_HEIGHT + LIB_UI_MARGIN_DEFAULT;
	lib_ui_show_radio( ui, lib_ui_add_radio( ui, x1, y1, width, 0, (uint8_t *) "Radio.1 (Group.0)" ) );
	y1 += LIB_UI_RADIO_HEIGHT + LIB_UI_MARGIN_DEFAULT;
	lib_ui_show_radio( ui, lib_ui_add_radio( ui, x1, y1, width, 0, (uint8_t *) "Radio.2 (Group.0)" ) );
	y1 += LIB_UI_RADIO_HEIGHT + LIB_UI_MARGIN_DEFAULT;
	lib_ui_show_radio( ui, lib_ui_add_radio( ui, x1, y1, width, 0, (uint8_t *) "Radio.3 (Group.0)" ) );
	y1 += LIB_UI_RADIO_HEIGHT + LIB_UI_MARGIN_DEFAULT;
	lib_ui_show_radio( ui, lib_ui_add_radio( ui, x1, y1, width, 1, (uint8_t *) "Radio.0 (Group.1)" ) );
	y1 += LIB_UI_RADIO_HEIGHT + LIB_UI_MARGIN_DEFAULT;
	lib_ui_show_radio( ui, lib_ui_add_radio( ui, x1, y1, width, 1, (uint8_t *) "Radio.1 (Group.1)" ) );
}

uint64_t _main( uint64_t argc, uint8_t *argv[] ) {
	ui = lib_ui( lib_window( -1, -1, 320, 200 ) );
	lib_ui_clean( ui );

	create_ui();

	ui -> window -> flags = LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_flush;

	while( TRUE ) { lib_ui_event( ui ); sleep( TRUE ); }

	return 0;
}
