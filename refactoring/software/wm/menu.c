/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

int64_t wm_menu( void ) {
	// prepare JSON structure for parsing
	lib_json_squeeze( (uint8_t *) &file_menu_json_start );

	// convert interface properties to a more accessible format
	menu_interface.properties = (uint8_t *) &file_menu_json_start;
	lib_interface_convert( (struct LIB_INTERFACE_STRUCTURE *) &menu_interface );

	// find control element of type: close
	struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU *menu;
	menu = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU *) lib_interface_element_by_id( (struct LIB_INTERFACE_STRUCTURE *) &menu_interface, 0 );
	menu -> event = (void *) wm_menu_exec;

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
	wm_object_menu = wm_object_create( -1, wm_object_taskbar -> y - menu_interface.height + 1, menu_interface.width, menu_interface.height );

	// mark it as own
	wm_object_menu -> pid = wm_pid;

	// special flag for menu window
	wm_object_menu -> descriptor -> flags |= STD_WINDOW_FLAG_unstable;

	// update menu interface descriptor
	menu_interface.descriptor = wm_object_menu -> descriptor;

	// clear window content
	lib_interface_clear( (struct LIB_INTERFACE_STRUCTURE *) &menu_interface );

	// set window name
	uint8_t system_release[] = "Foton v"KERNEL_version"."KERNEL_revision;
	for( uint64_t i = 0; i < lib_string_length( system_release ); i++ ) menu_interface.name[ menu_interface.name_length++ ] = system_release[ i ];

	// show window name in header if set
	lib_interface_name( (struct LIB_INTERFACE_STRUCTURE *) &menu_interface );

	// show interface elements
	lib_interface_draw( (struct LIB_INTERFACE_STRUCTURE *) &menu_interface );

	// main loop
	while( TRUE ) {
		// check events
		lib_interface_event( (struct LIB_INTERFACE_STRUCTURE *) &menu_interface );

		// release CPU time
		sleep( TRUE );
	}

	// dummy
	return EMPTY;
}


void wm_menu_exec( struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU *menu ) {
	// execute command provieded with menu entry
	std_exec( menu -> command, lib_string_length( menu -> command ), EMPTY, TRUE );

	// hide menu window
	wm_menu_switch( FALSE );
}

void wm_menu_switch( uint8_t menu_semaphore ) {
	// menu window already visible?
	if( menu_semaphore || wm_object_menu -> descriptor -> flags & STD_WINDOW_FLAG_active ) {
		// don't show anymore
		wm_object_menu -> descriptor -> flags &= ~STD_WINDOW_FLAG_visible;

		// and redraw area behind
		wm_object_menu -> descriptor -> flags |= STD_WINDOW_FLAG_flush;

		// search for next active window
		wm_object_active_new();
	} else {
		// generate and show menu window
		wm_object_menu -> descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_active | STD_WINDOW_FLAG_flush;

		// show above other objects
		wm_object_move_up( wm_object_menu );

		// mark as active
		wm_object_active = wm_object_menu;
	}
}
