/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void taris_init( void ) {
	// initialize interface library
	taris_interface.properties = (uint8_t *) &file_interface_start;
	lib_interface( (struct LIB_INTERFACE_STRUCTURE *) &taris_interface );

	// find control element of type: close
	struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *control = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *) lib_interface_element_by_id( (struct LIB_INTERFACE_STRUCTURE *) &taris_interface, 0 );

	// assign executable function to element
	control -> event = (void *) close;

	// find label element with points value
	taris_points = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *) lib_interface_element_by_id( (struct LIB_INTERFACE_STRUCTURE *) &taris_interface, 1 );

	// find label element with lines value
	taris_lines = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *) lib_interface_element_by_id( (struct LIB_INTERFACE_STRUCTURE *) &taris_interface, 2 );

	// find label element with level value
	taris_level = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *) lib_interface_element_by_id( (struct LIB_INTERFACE_STRUCTURE *) &taris_interface, 3 );

	// initialize RGL library
	taris_rgl = lib_rgl( TARIS_PLAYGROUND_WIDTH_pixel, TARIS_PLAYGROUND_HEIGHT_pixel, taris_interface.width, (uint32_t *) ((uintptr_t) taris_interface.descriptor + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR ) + ((((taris_interface.descriptor -> offset + LIB_INTERFACE_HEADER_HEIGHT_pixel + TARIS_MENU_HEIGHT_pixel) * taris_interface.width) + LIB_INTERFACE_BORDER_pixel + taris_interface.descriptor -> offset) << STD_VIDEO_DEPTH_shift)) );

	// update window content on screen
	taris_interface.descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;
}