/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void init( void ) {
	// initialize interface library
	d3_interface.properties = (uint8_t *) &file_interface_start;
	lib_interface( (struct LIB_INTERFACE_STRUCTURE *) &d3_interface );

	// find control element of type: close
	struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *control = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *) lib_interface_element_by_id( (struct LIB_INTERFACE_STRUCTURE *) &d3_interface, 0 );

	// assign executable function to element
	control -> event = (void *) close;

	// initialize RGL library
	rgl = lib_rgl( d3_interface.width - (LIB_INTERFACE_BORDER_pixel << STD_SHIFT_2), d3_interface.height - (LIB_INTERFACE_HEADER_HEIGHT_pixel + LIB_INTERFACE_BORDER_pixel), d3_interface.width, (uint32_t *) ((uintptr_t) d3_interface.descriptor + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR ) + (((LIB_INTERFACE_HEADER_HEIGHT_pixel * d3_interface.width) + LIB_INTERFACE_BORDER_pixel) << STD_VIDEO_DEPTH_shift)) );

	// window content ready for display
	d3_interface.descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;

	//----------------------------------------------------------------------

	// parse object properties
	object_load();
}