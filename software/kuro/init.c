/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kuro_init( void ) {
	// alloc area for interface properties
	kuro_interface = (struct LIB_INTERFACE_STRUCTURE *) malloc( sizeof( struct LIB_INTERFACE_STRUCTURE ) );

	// initialize interface library
	kuro_interface -> properties = (uint8_t *) &file_interface_start;
	if( ! lib_interface( kuro_interface ) ) { log( "Cannot create window.\n" ); exit(); }

	// set minimal window size as current
	kuro_interface -> min_width = kuro_interface -> width;
	kuro_interface -> min_height = kuro_interface -> height;

	//----------------------------------------------------------------------

	// find element of ID: 0
	struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *control = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *) lib_interface_element_by_id( kuro_interface, 0 );

	// assign executable function to element
	control -> event = exit;	

	// find element of ID: 1
	kuro_files = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE *) lib_interface_element_by_id( kuro_interface, 1 );

	// define our own colors
	kuro_files -> color_default	= KURO_LIST_COLOR_default;
	kuro_files -> color_odd		= KURO_LIST_COLOR_odd;
	kuro_files -> color_selected	= KURO_LIST_COLOR_selected;

	// find element of ID: 2
	kuro_storages = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE *) lib_interface_element_by_id( kuro_interface, 2 );

	// define our own colors
	kuro_storages -> color_default	= KURO_LIST_COLOR_default;
	kuro_storages -> color_odd	= KURO_LIST_COLOR_odd;
	kuro_storages -> color_selected	= KURO_LIST_COLOR_selected;

	//----------------------------------------------------------------------

	// initialize icon list
	kuro_icons = (uint32_t **) malloc( TRUE );

	// register initial icon (directory change)
	kuro_icon_register( KURO_MIMETYPE_up, (uint8_t *) "/var/share/media/icon/default/up.tga" );

	//----------------------------------------------------------------------
}

// 	// // do not allow deselection, do not allow more than 1 at a time, immedietly action
// 	// kuro_storages -> flags = LIB_INTERFACE_ELEMENT_LIST_FLAG_persistent | LIB_INTERFACE_ELEMENT_LIST_FLAG_individual | LIB_INTERFACE_ELEMENT_LIST_FLAG_single_click;

// 	// // first entry selected as default
// 	// kuro_storages -> entry[ FALSE ].flags = LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_active;
