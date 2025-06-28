/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void init( void ) {
	window = lib_window( 306, 3, 330, 300 );
	ui = lib_ui( window );

	lib_ui_clean( ui );

	lib_window_name( ui -> window, (uint8_t *) "3D Viewer (Demo)" );

	lib_ui_add_control( ui, LIB_UI_ELEMENT_CONTROL_TYPE_close );
	lib_ui_add_control( ui, LIB_UI_ELEMENT_CONTROL_TYPE_max );
	lib_ui_add_control( ui, LIB_UI_ELEMENT_CONTROL_TYPE_min );

	lib_ui_flush( ui );

	ui -> window -> flags = LIB_WINDOW_FLAG_resizable | LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_flush;

	// initialize RGL library
	rgl = lib_rgl( ui -> window -> current_width - (LIB_UI_BORDER_DEFAULT << STD_SHIFT_2), ui -> window -> current_height - (LIB_UI_HEADER_HEIGHT + LIB_UI_BORDER_DEFAULT), ui -> window -> current_width, (uint32_t *) ui -> window -> pixel + (LIB_UI_HEADER_HEIGHT * ui -> window -> current_width) + LIB_UI_BORDER_DEFAULT );

	// change background color to UI type
	rgl -> color_background = LIB_UI_COLOR_BACKGROUND_DEFAULT;
}
