// /*===============================================================================
//  Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
// ===============================================================================*/

void wm_menu_exec( struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU *menu ) {
	MACRO_DEBUF();

	// execute command proviede with menu entry
	std_exec( menu -> command, lib_string_length( menu -> command ), EMPTY );
}

int64_t wm_menu( void ) {
	// prepare JSON structure for parsing
	lib_json_squeeze( (uint8_t *) &file_menu_json_start );

	// convert interface properties to a more accessible format
	menu_interface.properties = (uint8_t *) &file_menu_json_start;
	lib_interface_convert( (struct LIB_INTERFACE_STRUCTURE *) &menu_interface );

	// connect each menu entry to execution function
	uint8_t *element = (uint8_t *) menu_interface.properties; uint64_t e = 0;
	while( element[ e ] != LIB_INTERFACE_ELEMENT_TYPE_null ) {
		// element properties
		struct LIB_INTERFACE_STRUCTURE_ELEMENT *properties = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) &element[ e ];

		// menu entry?
		if( properties -> type & LIB_INTERFACE_ELEMENT_TYPE_menu ) {
			// properties of control
			struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU *menu = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU *) properties;

			// connect
			menu -> event = (void *) wm_menu_exec;
		}

		// next element from list
		e += properties -> size_byte;
	}

	// create menu object
	wm_object_menu = wm_object_create( -LIB_INTERFACE_SHADOW_length, wm_object_taskbar -> y - (menu_interface.height - LIB_INTERFACE_SHADOW_length), menu_interface.width, menu_interface.height );

	// mark it as own
	wm_object_menu -> pid = wm_pid;

	// update menu interface descriptor
	menu_interface.descriptor = wm_object_menu -> descriptor;

	// set shadow length
	menu_interface.descriptor -> offset = LIB_INTERFACE_SHADOW_length;

	// prepare shadow of window
	lib_interface_shadow( (struct LIB_INTERFACE_STRUCTURE *) &menu_interface );

	// clear window content
	lib_interface_clear( (struct LIB_INTERFACE_STRUCTURE *) &menu_interface );

	// show window name in header if set
	lib_interface_name( (struct LIB_INTERFACE_STRUCTURE *) &menu_interface );

	// show interface elements
	lib_interface_draw( (struct LIB_INTERFACE_STRUCTURE *) &menu_interface );

	// debug
	wm_object_menu -> descriptor -> flags |= STD_WINDOW_FLAG_unstable;

	// main loop
	while( TRUE ) {
		// check elements hover
		lib_interface_hover( (struct LIB_INTERFACE_STRUCTURE *) &menu_interface );

		// release CPU time
		std_sleep( TRUE );
	}

	// dummy
	return EMPTY;
}
