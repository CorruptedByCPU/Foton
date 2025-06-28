/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kuro_init( void ) {
	// initialize icon list
	kuro_icon = (uint32_t **) malloc( TRUE );

	// register initial icon (directory change)
	kuro_icon_register( UP, (uint8_t *) "/var/share/media/icon/default/empty.tga" );
	kuro_icon_register( DIRECTORY, (uint8_t *) "/var/share/media/icon/default/places/folder.tga" );
	kuro_icon_register( UNKNOWN, (uint8_t *) "/var/share/media/icon/default/mimetypes/unknown.tga" );

	// create window
	struct LIB_WINDOW_STRUCTURE *window = lib_window( 3, 3, KURO_DEFAULT_WIDTH, KURO_DEFAULT_HEIGHT );

	// we allow window resize
	window -> flags |= LIB_WINDOW_FLAG_resizable;

	// initialize interface properties for our window
	ui = lib_ui( window );

	// set ui window name
	lib_window_name( ui -> window, (uint8_t *) "Kuro - File Manager" );

	// add default window controls
	lib_ui_add_control( ui, LIB_UI_ELEMENT_CONTROL_TYPE_close );
	lib_ui_add_control( ui, LIB_UI_ELEMENT_CONTROL_TYPE_max );
	lib_ui_add_control( ui, LIB_UI_ELEMENT_CONTROL_TYPE_min );

	// window ready for show up
	ui -> window -> flags |= LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_flush;
}
