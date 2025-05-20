/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#include	"../library/ui.h"
#include	"../library/window.h"

uint64_t _main( uint64_t argc, uint8_t *argv[] ) {
	struct LIB_WINDOW_STRUCTURE *window = lib_window( -1, -1, 320, 200 );
	struct LIB_UI_STRUCTURE *ui = lib_ui( window );
	lib_ui_clean( ui );
	lib_ui_show_button( ui, lib_ui_add_button( ui, LIB_UI_MARGIN_DEFAULT + ((ui -> window -> width - (LIB_UI_MARGIN_DEFAULT << 1)) >> 1), (LIB_UI_MARGIN_DEFAULT << 1) + 16, (ui -> window -> width - (LIB_UI_MARGIN_DEFAULT << 1)) >> 1, 22, (uint8_t *) "Button" ) );
	lib_ui_show_checkbox( ui, lib_ui_add_checkbox( ui, LIB_UI_MARGIN_DEFAULT, (LIB_UI_MARGIN_DEFAULT * 2) + 16, (ui -> window -> width - (LIB_UI_MARGIN_DEFAULT << 1)) >> 1, (uint8_t *) "Checkbox" ) );
	lib_ui_show_checkbox( ui, lib_ui_add_checkbox( ui, LIB_UI_MARGIN_DEFAULT, (LIB_UI_MARGIN_DEFAULT * 3) + 32, (ui -> window -> width - (LIB_UI_MARGIN_DEFAULT << 1)) >> 1, (uint8_t *) "Checkbox" ) );
	lib_ui_show_radio( ui, lib_ui_add_radio( ui, LIB_UI_MARGIN_DEFAULT, (LIB_UI_MARGIN_DEFAULT * 4) + 48, (ui -> window -> width - (LIB_UI_MARGIN_DEFAULT << 1)) >> 1, 0, (uint8_t *) "Radio" ) );
	lib_ui_show_radio( ui, lib_ui_add_radio( ui, LIB_UI_MARGIN_DEFAULT, (LIB_UI_MARGIN_DEFAULT * 5) + 64, (ui -> window -> width - (LIB_UI_MARGIN_DEFAULT << 1)) >> 1, 0, (uint8_t *) "Radio" ) );
	lib_ui_show_radio( ui, lib_ui_add_radio( ui, LIB_UI_MARGIN_DEFAULT, (LIB_UI_MARGIN_DEFAULT * 6) + 80, (ui -> window -> width - (LIB_UI_MARGIN_DEFAULT << 1)) >> 1, 0, (uint8_t *) "Radio" ) );
	lib_ui_show_label( ui, lib_ui_add_label( ui, LIB_UI_MARGIN_DEFAULT, LIB_UI_MARGIN_DEFAULT, (uint8_t *) "Label" ) );
	ui -> window -> flags = LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_flush;

	while( TRUE ) { lib_ui_event( ui ); sleep( TRUE ); }

	return 0;
}
