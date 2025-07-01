/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void init( void ) {
	window = lib_window( 273, 3, V3D_WIDTH + (LIB_UI_BORDER_DEFAULT << STD_SHIFT_2), V3D_HEIGHT + LIB_UI_HEADER_HEIGHT + LIB_UI_BORDER_DEFAULT );
	ui = lib_ui( window );

	lib_ui_clean( ui );

	ui -> icon = lib_image_scale( lib_ui_icon( (uint8_t *) "/var/share/media/icon/3d.tga" ), 48, 48, 16, 16 );

	// add icon to window properties
	for( uint64_t i = 0; i < 16 * 16; i++ ) window -> icon[ i ] = ui -> icon[ i ];

	lib_window_name( ui -> window, (uint8_t *) "3D Viewer (Demo)" );

	lib_ui_add_control( ui, LIB_UI_ELEMENT_CONTROL_TYPE_close );
	lib_ui_add_control( ui, LIB_UI_ELEMENT_CONTROL_TYPE_max );
	lib_ui_add_control( ui, LIB_UI_ELEMENT_CONTROL_TYPE_min );

	lib_ui_flush( ui );

	ui -> window -> flags = LIB_WINDOW_FLAG_resizable | LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_icon | LIB_WINDOW_FLAG_flush;

	// initialize RGL library
	rgl = lib_rgl( V3D_WIDTH, V3D_HEIGHT, V3D_WIDTH + (LIB_UI_BORDER_DEFAULT << STD_SHIFT_2), (uint32_t *) ui -> window -> pixel + (LIB_UI_HEADER_HEIGHT * ui -> window -> current_width) + LIB_UI_BORDER_DEFAULT );

	// change background color to UI type
	rgl -> color_background = LIB_UI_COLOR_BACKGROUND_DEFAULT;
}
