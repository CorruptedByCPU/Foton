/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#ifndef	LIB_JSON
		#include	"./json.h"
	#endif
	#ifndef	LIB_INTERFACE
		#include	"./interface.h"
	#endif

const uint8_t lib_interface_string_window[] = "window";
const uint8_t lib_interface_string_x[] = "x";
const uint8_t lib_interface_string_y[] = "y";
const uint8_t lib_interface_string_width[] = "width";
const uint8_t lib_interface_string_height[] = "height";
const uint8_t lib_interface_string_name[] = "name";
const uint8_t lib_interface_string_label[] = "label";
const uint8_t lib_interface_string_button[] = "button";
const uint8_t lib_interface_string_id[] = "id";
const uint8_t lib_interface_string_align[] = "align";
const uint8_t lib_interface_string_center[] = "center";
const uint8_t lib_interface_string_right[] = "right";
const uint8_t lib_interface_string_justify[] = "justify";
const uint8_t lib_interface_string_type[] = "type";
const uint8_t lib_interface_string_control[] = "control";
const uint8_t lib_interface_string_close[] = "close";
const uint8_t lib_interface_string_minimize[] = "minimize";
const uint8_t lib_interface_string_maximize[] = "maximize";
const uint8_t lib_interface_string_menu[] = "menu";
const uint8_t lib_interface_string_command[] = "command";
const uint8_t lib_interface_string_icon[] = "icon";

void lib_interface( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// prepare JSON structure for parsing
	lib_json_squeeze( interface -> properties );

	// convert interface properties to a more accessible format
	lib_interface_convert( interface );

	// if dimensions aquired from JSON structure
	if( interface -> width && interface -> height ) {
		// create window
		lib_interface_window( interface );

		// show interface elements
		lib_interface_draw( interface );
	}
}

void lib_interface_clear( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// fill window with default background
	uint32_t *pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR )) + (interface -> descriptor -> offset * interface -> width) + interface -> descriptor -> offset;
	for( uint16_t y = 0; y < interface -> height - (interface -> descriptor -> offset << STD_SHIFT_2); y++ )
		for( uint16_t x = 0; x < interface -> width - (interface -> descriptor -> offset << STD_SHIFT_2); x++ )
			// draw pixel
			pixel[ (y * interface -> width) + x ] = LIB_INTERFACE_COLOR_background;
}

void lib_interface_convert( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// properties of new interface format
	uint64_t i = EMPTY;	// properties index
	uint8_t *properties = (uint8_t *) malloc( TRUE );	// alloc default area

	// properties of JSON structure
	struct LIB_JSON_STRUCTURE json = lib_json( interface -> properties );

	// parse elements inside JSON structure
	do {
		// window object?
		if( lib_json_key( json, (uint8_t *) &lib_interface_string_window ) ) {
			// window object properties
			struct LIB_JSON_STRUCTURE window = lib_json( (uint8_t *) json.value );

			// parse all keys of this object
			do {
				// retrieve width value
				if( lib_json_key( window, (uint8_t *) &lib_interface_string_width ) ) interface -> width = window.value + (LIB_INTERFACE_SHADOW_length << STD_SHIFT_2);
	
				// retrieve height value
				if( lib_json_key( window, (uint8_t *) &lib_interface_string_height ) ) interface -> height = window.value + (LIB_INTERFACE_SHADOW_length << STD_SHIFT_2);

				// retrieve name value
				if( lib_json_key( window, (uint8_t *) &lib_interface_string_name ) ) {
					// set name length
					interface -> name_length = window.length;
					if( window.length > LIB_INTERFACE_NAME_limit ) interface -> name_length = LIB_INTERFACE_NAME_limit;

					// copy name
					uint8_t *name = (uint8_t *) window.value;
					for( uint64_t i = 0; i < interface -> name_length; i++ ) interface -> name[ i ] = name[ i ];
				}
			// next key
			} while( lib_json_next( (struct LIB_JSON_STRUCTURE *) &window ) );
		}

		// control?
		if( lib_json_key( json, (uint8_t *) &lib_interface_string_control ) ) {
			// alloc space for element
			properties = (uint8_t *) realloc( properties, i + sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL ) );

			// element structure position
			struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *) &properties[ i ];
	
			// control properties
			struct LIB_JSON_STRUCTURE control = lib_json( (uint8_t *) json.value );

			// default properties of control
			element -> control.y = LIB_INTERFACE_SHADOW_length;
			element -> control.width = LIB_INTERFACE_HEADER_HEIGHT_pixel;
			element -> control.height = LIB_INTERFACE_HEADER_HEIGHT_pixel;
			element -> control.flags = EMPTY;
			element -> control.size_byte = sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL );

			// parse all keys
			do {
				// id
				if( lib_json_key( control, (uint8_t *) &lib_interface_string_id ) ) element -> control.id = control.value;

				// x
				if( lib_json_key( control, (uint8_t *) &lib_interface_string_x ) ) element -> control.x = control.value + LIB_INTERFACE_SHADOW_length;

				// type
				if( lib_json_key( control, (uint8_t *) &lib_interface_string_type ) ) {
					// close
					if( lib_string_compare( (uint8_t *) control.value, (uint8_t *) "close", control.length ) ) element -> control.type = LIB_INTERFACE_ELEMENT_TYPE_control_close;

					// maximize
					if( lib_string_compare( (uint8_t *) control.value, (uint8_t *) "maximize", control.length ) ) element -> control.type = LIB_INTERFACE_ELEMENT_TYPE_control_maximize;

					// minimize
					if( lib_string_compare( (uint8_t *) control.value, (uint8_t *) "minimize", control.length ) ) element -> control.type = LIB_INTERFACE_ELEMENT_TYPE_control_minimize;
				}
			// next key
			} while( lib_json_next( (struct LIB_JSON_STRUCTURE *) &control ) );

			// window interface contains additional control element
			interface -> controls++;

			// change interface structure index
			i += element -> control.size_byte;
		}

		// check element type
		uint8_t type = EMPTY;
		if( lib_json_key( json, (uint8_t *) &lib_interface_string_label ) ) type = LIB_INTERFACE_ELEMENT_TYPE_label;
		if( lib_json_key( json, (uint8_t *) &lib_interface_string_button ) ) type = LIB_INTERFACE_ELEMENT_TYPE_button;

		// label or button?
		if( type == LIB_INTERFACE_ELEMENT_TYPE_label || type == LIB_INTERFACE_ELEMENT_TYPE_button ) {
			// alloc space for element
			properties = (uint8_t *) realloc( properties, i + sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON ) );

			// element structure position
			struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *) &properties[ i ];
	
			// label or button properties
			struct LIB_JSON_STRUCTURE label_or_button = lib_json( (uint8_t *) json.value );

			// default properties of label and button
			element -> label_or_button.type = type;
			element -> label_or_button.flags = EMPTY;
			element -> label_or_button.size_byte = sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON );

			// parse all keys
			do {
				// element name alignment
				uint16_t element_name_align = EMPTY;

				// id
				if( lib_json_key( label_or_button, (uint8_t *) &lib_interface_string_id ) ) element -> label_or_button.id = label_or_button.value;

				// x
				if( lib_json_key( label_or_button, (uint8_t *) &lib_interface_string_x ) ) element -> label_or_button.x = label_or_button.value + LIB_INTERFACE_SHADOW_length;

				// y
				if( lib_json_key( label_or_button, (uint8_t *) &lib_interface_string_y ) ) element -> label_or_button.y = label_or_button.value + LIB_INTERFACE_SHADOW_length;

				// width
				if( lib_json_key( label_or_button, (uint8_t *) &lib_interface_string_width ) ) element -> label_or_button.width = label_or_button.value;

				// height
				if( lib_json_key( label_or_button, (uint8_t *) &lib_interface_string_height ) ) element -> label_or_button.height = label_or_button.value;
		
				// align
				if( lib_json_key( label_or_button, (uint8_t *) &lib_interface_string_align ) ) {
					// by default
					element -> label_or_button.flags = LIB_FONT_ALIGN_left;

					// center?
					if( lib_string_compare( (uint8_t *) label_or_button.value, (uint8_t *) &lib_interface_string_center, label_or_button.length ) )
						element -> label_or_button.flags = LIB_FONT_ALIGN_center;

					// right?
					if( lib_string_compare( (uint8_t *) label_or_button.value, (uint8_t *) &lib_interface_string_right, label_or_button.length ) )
						element -> label_or_button.flags = LIB_FONT_ALIGN_right;
				}

				// name
				if( lib_json_key( label_or_button, (uint8_t *) &lib_interface_string_name ) ) {
					// length if proper
					element -> name_length = label_or_button.length;

					// alloc area for element name
					uint8_t *name_target = (uint8_t *) calloc( element -> name_length + 1 );

					// copy element name
					uint8_t *name_source = (uint8_t *) label_or_button.value;
					for( uint64_t i = 0; i < element -> name_length; i++ ) name_target[ i ] = name_source[ i ];

					// update element name pointer
					element -> name = name_target;
				}

			// next key
			} while( lib_json_next( (struct LIB_JSON_STRUCTURE *) &label_or_button ) );

			// change interface structure index
			i += element -> label_or_button.size_byte;
		}

		// menu?
		if( lib_json_key( json, (uint8_t *) &lib_interface_string_menu ) ) {
			// alloc space for element
			properties = (uint8_t *) realloc( properties, i + sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU ) );

			// element structure position
			struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU *element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU *) &properties[ i ];
	
			// control properties
			struct LIB_JSON_STRUCTURE menu = lib_json( (uint8_t *) json.value );

			// default properties of menu entry
			element -> menu.type = LIB_INTERFACE_ELEMENT_TYPE_menu;
			element -> menu.flags = EMPTY;
			element -> menu.size_byte = sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU );

			// parse all keys
			do {
				// id
				if( lib_json_key( menu, (uint8_t *) &lib_interface_string_id ) ) element -> menu.id = menu.value;

				// x
				if( lib_json_key( menu, (uint8_t *) &lib_interface_string_x ) ) element -> menu.x = menu.value + LIB_INTERFACE_SHADOW_length;

				// y
				if( lib_json_key( menu, (uint8_t *) &lib_interface_string_y ) ) element -> menu.y = menu.value + LIB_INTERFACE_SHADOW_length;

				// width
				if( lib_json_key( menu, (uint8_t *) &lib_interface_string_width ) ) element -> menu.width = menu.value;

				// height
				if( lib_json_key( menu, (uint8_t *) &lib_interface_string_height ) ) element -> menu.height = menu.value;

				// name
				if( lib_json_key( menu, (uint8_t *) &lib_interface_string_name ) ) {
					// length if proper
					element -> name_length = menu.length;

					// alloc area for element name
					uint8_t *name_target = (uint8_t *) calloc( element -> name_length + 1 );

					// copy element name
					uint8_t *name_source = (uint8_t *) menu.value;
					for( uint64_t i = 0; i < element -> name_length; i++ ) name_target[ i ] = name_source[ i ];

					// update element name pointer
					element -> name = name_target;
				}

				// command
				if( lib_json_key( menu, (uint8_t *) &lib_interface_string_command ) ) {
					// alloc area for element name
					uint8_t *command_target = (uint8_t *) calloc( menu.length + 1 );

					// copy element name
					uint8_t *command_source = (uint8_t *) menu.value;
					for( uint64_t i = 0; i < menu.length; i++ ) command_target[ i ] = command_source[ i ];

					// update element command pointer
					element -> command = command_target;
				}

				// icon
				if( lib_json_key( menu, (uint8_t *) &lib_interface_string_icon ) ) {
					// properties of file
					FILE *icon_file;

					// properties of image
					struct LIB_IMAGE_TGA_STRUCTURE *icon_image = EMPTY;

					// retrieve information about module file
					uint8_t *icon_file_string = (uint8_t *) menu.value; icon_file_string[ menu.length ] = STD_ASCII_TERMINATOR;
					if( (icon_file = fopen( (uint8_t *) menu.value )) ) {
						// assign area for file
						icon_image = (struct LIB_IMAGE_TGA_STRUCTURE *) malloc( icon_file -> byte );

						// load file content
						fread( icon_file, (uint8_t *) icon_image, icon_file -> byte );

						// copy image content to cursor object
						element -> icon = (uint32_t *) malloc( icon_image -> width * icon_image -> height * STD_VIDEO_DEPTH_byte );
						lib_image_tga_parse( (uint8_t *) icon_image, element -> icon, icon_file -> byte );

						// release file content
						// free( icon_image );

						// close file
						// fclose( icon_file );
					}
				}
			// next key
			} while( lib_json_next( (struct LIB_JSON_STRUCTURE *) &menu ) );

			// change interface structure index
			i += element -> menu.size_byte;
		}

	// until no more elements
	} while( lib_json_next( (struct LIB_JSON_STRUCTURE *) &json ) );

	// last element must be NULL
	properties = (uint8_t *) realloc( properties, i + TRUE );
	properties[ i ] = LIB_INTERFACE_ELEMENT_TYPE_null;

	// store new properties pointer
	interface -> properties = properties;
};

void lib_interface_draw( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// first element properties
	struct LIB_INTERFACE_STRUCTURE_ELEMENT *element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) interface -> properties;

	// process all interface elements
	while( element -> type != LIB_INTERFACE_ELEMENT_TYPE_null ) {
		switch( element -> type ) {
			case LIB_INTERFACE_ELEMENT_TYPE_label: { lib_interface_element_label( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *) element ); break; }
			case LIB_INTERFACE_ELEMENT_TYPE_control_close: { lib_interface_element_control( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *) element ); break; }
			case LIB_INTERFACE_ELEMENT_TYPE_control_maximize: { lib_interface_element_control( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *) element ); break; }
			case LIB_INTERFACE_ELEMENT_TYPE_control_minimize: { lib_interface_element_control( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *) element ); break; }
			case LIB_INTERFACE_ELEMENT_TYPE_menu: { lib_interface_element_menu( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU *) element ); break; }
		}
	
		// next element properties
		element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) ((uint64_t) element + element -> size_byte);
	}
}

uintptr_t lib_interface_element_by_id( struct LIB_INTERFACE_STRUCTURE *interface, uint64_t id ) {
	// check which element is under cursor position
	uint8_t *element = (uint8_t *) interface -> properties; uint64_t e = 0;
	while( element[ e ] != LIB_INTERFACE_ELEMENT_TYPE_null ) {
		// element properties
		struct LIB_INTERFACE_STRUCTURE_ELEMENT *properties = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) &element[ e ];

		// element found?
		if( properties -> id == id ) return (uintptr_t) properties;

		// next element from list
		e += properties -> size_byte;
	}

	// element of ID not found
	return EMPTY;
}

void lib_interface_element_control( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *element ) {
	// properties of control buttons of window
	uint32_t *pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR )) + (interface -> descriptor -> offset * interface -> width) + element -> control.x;

	// choose background color
	uint32_t background_color = LIB_INTERFACE_COLOR_background;
	if( element -> control.flags & LIB_INTERFACE_ELEMENT_FLAG_hover ) {
		if( element -> control.type == LIB_INTERFACE_ELEMENT_TYPE_control_close ) background_color = LIB_INTERFACE_COLOR_background_hover;
		else background_color = LIB_INTERFACE_COLOR_background_light;
	}

	// clear element space
	for( uint8_t y = 0; y < LIB_INTERFACE_HEADER_HEIGHT_pixel; y++ )
		for( uint8_t x = 0; x < LIB_INTERFACE_HEADER_HEIGHT_pixel; x++ )
			pixel[ (y * interface -> width) + x ] = background_color;

	// choose element type
	switch( element -> control.type ) {	
		case LIB_INTERFACE_ELEMENT_TYPE_control_minimize: {
			// display minimize window button
			for( uint64_t x = 6; x <= 18; x++ ) pixel[ (18 * interface -> width) + x ] = 0xFFC0C0C0;

			// done
			break;
		}
		case LIB_INTERFACE_ELEMENT_TYPE_control_maximize: {
			// display window maximize button
			for( uint64_t y = 6; y <= 18; y++ ) {
				pixel[ (6 * interface -> width) + y ] = 0xFFC0C0C0;
				pixel[ (y * interface -> width) + 6 ] = 0xFFC0C0C0;
				pixel[ (y * interface -> width) + 18 ] = 0xFFC0C0C0;
				pixel[ (18 * interface -> width) + y ] = 0xFFC0C0C0;
			}

			// done
			break;
		}
		case LIB_INTERFACE_ELEMENT_TYPE_control_close: {
			// display close window button
			for( uint64_t y = 6; y <= 18; y++ ) {
				pixel[ (y * interface -> width) + y ] = 0xFFF0F0F0;
				pixel[ (LIB_INTERFACE_HEADER_HEIGHT_pixel - y) + (y * interface -> width) ] = 0xFFF0F0F0;
			}

			// done
			break;
		}
	}
}

void lib_interface_element_label( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *element ) {
	// limit string length to element width
	while( lib_font_length_string( LIB_FONT_FAMILY_ROBOTO, element -> name, element -> name_length ) > element -> label_or_button.width ) if( ! --element -> name_length ) return;

	// compute absolute address of first pixel of element space
	uint32_t *pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR )) + (element -> label_or_button.y * interface -> width) + element -> label_or_button.x;

	// fill element with background color
	for( uint16_t y = 0; y < element -> label_or_button.height; y++ )
		for( uint16_t x = 0; x < element -> label_or_button.width; x++ )
			pixel[ (y * interface -> width) + x ] = LIB_INTERFACE_COLOR_background;

	// vertical align of element content
	if( element -> label_or_button.height > LIB_FONT_HEIGHT_pixel ) pixel += ((element -> label_or_button.height - LIB_FONT_HEIGHT_pixel) >> STD_SHIFT_2) * interface -> width;

	// horizontal align of element content
	if( element -> label_or_button.flags & LIB_FONT_ALIGN_center ) pixel += element -> label_or_button.width >> STD_SHIFT_2;
	if( element -> label_or_button.flags & LIB_FONT_ALIGN_right ) pixel += element -> label_or_button.width;

	// display the content of element
	lib_font( LIB_FONT_FAMILY_ROBOTO, element -> name, element -> name_length, LIB_INTERFACE_COLOR_foreground, (uint32_t *) pixel, interface -> width, element -> label_or_button.flags );
}

void lib_interface_element_menu( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU *element ) {
	// limit string length to element width
	while( lib_font_length_string( LIB_FONT_FAMILY_ROBOTO, element -> name, element -> name_length ) > element -> menu.width ) if( ! --element -> name_length ) return;

	// compute absolute address of first pixel of element space
	uint32_t *pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR )) + (element -> menu.y * interface -> width) + element -> menu.x;

	// choose background color
	uint32_t background_color = LIB_INTERFACE_COLOR_background;
	if( element -> menu.flags & LIB_INTERFACE_ELEMENT_FLAG_hover ) background_color = LIB_INTERFACE_COLOR_MENU_background_hover;

	// fill element with background color
	for( uint16_t y = 0; y < element -> menu.height; y++ )
		for( uint16_t x = 0; x < element -> menu.width; x++ )
			pixel[ (y * interface -> width) + x ] = background_color;

	// if hovered
	if( element -> menu.flags & LIB_INTERFACE_ELEMENT_FLAG_hover ) {
		// add some fancy :)
		for( uint16_t x = 0; x < element -> menu.width; x++ ) pixel[ x ] = LIB_INTERFACE_COLOR_MENU_background_hover - 0x00040404;
		for( uint16_t x = 0; x < element -> menu.width; x++ ) pixel[ x + ((element -> menu.height - 1) * interface -> width) ] = LIB_INTERFACE_COLOR_MENU_background_hover + 0x00101010;
	}

	// display the content of element
	lib_font( LIB_FONT_FAMILY_ROBOTO, element -> name, element -> name_length, LIB_INTERFACE_COLOR_foreground, (uint32_t *) pixel + 4 + 16 + 2 + (((LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel - LIB_FONT_HEIGHT_pixel) >> STD_SHIFT_2) * interface -> width), interface -> width, element -> menu.flags );

	// icon provided?
	if( element -> icon ) {
		// compute absolute address of first pixel of icon
		uint32_t *icon = (uint32_t *) pixel + 4 + (((LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel - LIB_FONT_HEIGHT_pixel) >> STD_SHIFT_2) * interface -> width);

		// load icon to element area
		for( uint16_t y = 0; y < 16; y++ )
			for( uint16_t x = 0; x < 16; x++ )
				icon[ (y * interface -> width) + x ] = lib_color_blend( icon[ (y * interface -> width) + x ], element -> icon[ (y * 16) + x ] );
	}
}

void lib_interface_event( struct LIB_INTERFACE_STRUCTURE *interface, uint8_t force ) {
	// incomming message
	uint8_t ipc_data[ STD_IPC_SIZE_byte ];

	// receive pending messages
	if( force || std_ipc_receive_by_type( (uint8_t *) &ipc_data, STD_IPC_TYPE_mouse ) ) {
		// message properties
		struct STD_IPC_STRUCTURE_MOUSE *mouse = (struct STD_IPC_STRUCTURE_MOUSE *) &ipc_data;

		// released left mouse button?
		if( force || mouse -> button == (uint8_t) ~STD_IPC_MOUSE_BUTTON_left ) {
			// check which element is under cursor position
			uint8_t *element = (uint8_t *) interface -> properties; uint64_t e = 0;
			while( element[ e ] != LIB_INTERFACE_ELEMENT_TYPE_null ) {
				// element properties
				struct LIB_INTERFACE_STRUCTURE_ELEMENT *properties = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) &element[ e ];

				// cursor overlaps this element? (check only if object is located under cursor)
				if( interface -> descriptor -> x >= properties -> x && interface -> descriptor -> x < properties -> x + properties -> width && interface -> descriptor -> y >= properties -> y && interface -> descriptor -> y < properties -> y + properties -> height ) {
					// execute event of element
					switch( properties -> type ) {
						case LIB_INTERFACE_ELEMENT_TYPE_control_close: {
							// properties of control
							struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *control = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *) properties;

							// if event function exist, do it
							if( control -> event ) control -> event();

							// done
							break;
						}

						case LIB_INTERFACE_ELEMENT_TYPE_control_minimize: {
							// minimize window
							interface -> descriptor -> flags |= STD_WINDOW_FLAG_minimize | STD_WINDOW_FLAG_flush;

							// done
							break;
						}

						case LIB_INTERFACE_ELEMENT_TYPE_menu: {
							// properties of control
							struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU *menu = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU *) properties;

							// if event function exist, do it
							if( menu -> event ) menu -> event( menu );

							// done
							break;
						}
					}
				}

				// next element from list
				e += properties -> size_byte;
			}
		}
	}

	//--------------------------------------------------------------------------------
	// "hover over elements"
	//--------------------------------------------------------------------------------
	lib_interface_hover( interface );
}

void lib_interface_hover( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// check every element of interface
	uint8_t *element = (uint8_t *) interface -> properties; uint64_t e = 0;

	// check which element is under cursor position
	while( element[ e ] != LIB_INTERFACE_ELEMENT_TYPE_null ) {
		// element properties
		struct LIB_INTERFACE_STRUCTURE_ELEMENT *properties = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) &element[ e ];

		// ignore unsupported elements
		if( properties -> type == LIB_INTERFACE_ELEMENT_TYPE_label ) { e += properties -> size_byte; continue; }

		// last event
		uint8_t previous = properties -> flags;

		// cursor overlaps this element? (check only if object is located under cursor)
		if( interface -> descriptor -> x >= properties -> x && interface -> descriptor -> x < properties -> x + properties -> width && interface -> descriptor -> y >= properties -> y && interface -> descriptor -> y < properties -> y + properties -> height )
			// mark as hovered
			properties -> flags |= LIB_INTERFACE_ELEMENT_FLAG_hover;
		else
			// mark as not hovered
			properties -> flags &= ~LIB_INTERFACE_ELEMENT_FLAG_hover;

		// if "event" changed
		if( properties -> flags != previous ) {
			// redraw element inside object
			switch( properties -> type ) {
				case LIB_INTERFACE_ELEMENT_TYPE_control_close: { lib_interface_element_control( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *) &element[ e ] ); break; }
				case LIB_INTERFACE_ELEMENT_TYPE_control_maximize: { lib_interface_element_control( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *) &element[ e ] ); break; }
				case LIB_INTERFACE_ELEMENT_TYPE_control_minimize: { lib_interface_element_control( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *) &element[ e ] ); break; }
				case LIB_INTERFACE_ELEMENT_TYPE_menu: { lib_interface_element_menu( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU *) &element[ e ] ); break; }
			}

			// update window content on screen
			interface -> descriptor -> flags |= STD_WINDOW_FLAG_flush;
		}

		// next element from list
		e += properties -> size_byte;
	}
}

void lib_interface_name( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// window name set?
	if( ! interface -> name_length ) return;	// no

	// limit name length to header width
	while( lib_font_length_string( LIB_FONT_FAMILY_ROBOTO, interface -> name, interface -> name_length ) > interface -> width - ((interface -> controls * LIB_INTERFACE_HEADER_HEIGHT_pixel) + interface -> descriptor -> offset) ) if( ! --interface -> name_length ) return;

	// clear window header with default background
	uint32_t *pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR ));
	for( uint16_t y = interface -> descriptor -> offset; y < LIB_INTERFACE_HEADER_HEIGHT_pixel + interface -> descriptor -> offset; y++ )
		for( uint16_t x = interface -> descriptor -> offset; x < interface -> width - ((interface -> controls * LIB_INTERFACE_HEADER_HEIGHT_pixel) + interface -> descriptor -> offset); x++ )
			// draw pixel
			pixel[ (y * interface -> width) + x ] = LIB_INTERFACE_COLOR_background;

	// print new header
	lib_font( LIB_FONT_FAMILY_ROBOTO, (uint8_t *) &interface -> name, interface -> name_length, STD_COLOR_WHITE, pixel + ((4 + interface -> descriptor -> offset) * interface -> width) + 4 + interface -> descriptor -> offset, interface -> width, LIB_FONT_ALIGN_left );

	// synchronize header name with window
	interface -> descriptor -> name_length = interface -> name_length;
	for( uint8_t i = 0; i < interface -> name_length; i++ ) interface -> descriptor -> name[ i ] = interface -> name[ i ];

	// inform Window Manager about new window name
	interface -> descriptor -> flags |= STD_WINDOW_FLAG_name;
}

void lib_interface_shadow( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// no shadow?
	if( ! interface -> descriptor -> offset ) return;

	// set shadow color
	uint32_t *pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR ));
	for( uint16_t i = 0; i < interface -> descriptor -> offset; i ++ )
		for( uint16_t y = i + interface -> descriptor -> offset; y < interface -> height - (i + interface -> descriptor -> offset); y++ )
			for( uint16_t x = i + interface -> descriptor -> offset; x < interface -> width - (i + interface -> descriptor -> offset); x++ )
				// draw pixel
				pixel[ (y * interface -> width) + x ] = LIB_INTERFACE_SHADOW_color;

	// blur shadow
	lib_image_blur( pixel, interface -> descriptor -> offset, interface -> width, interface -> height );
}

void lib_interface_window( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// obtain information about kernel framebuffer
	struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER kernel_framebuffer;
	std_framebuffer( (struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER *) &kernel_framebuffer );

	// remember Window Manager PID
	int64_t wm_pid = kernel_framebuffer.pid;

	// allocate gui data container
	uint8_t wm_data[ STD_IPC_SIZE_byte ];

	// prepeare new window request
	struct STD_IPC_STRUCTURE_WINDOW *request = (struct STD_IPC_STRUCTURE_WINDOW *) &wm_data;
	struct STD_IPC_STRUCTURE_WINDOW_DESCRIPTOR *answer = EMPTY;	// answer will be in here

	//----------------------------------------------------------------------

	// window properties
	request -> ipc.type = STD_IPC_TYPE_event;
	request -> x = (kernel_framebuffer.width_pixel >> STD_SHIFT_2) - (interface -> width >> STD_SHIFT_2);
	request -> y = ((kernel_framebuffer.height_pixel - LIB_INTERFACE_HEADER_HEIGHT_pixel) >> STD_SHIFT_2) - (interface -> height >> STD_SHIFT_2);
	request -> width = interface -> width;
	request -> height = interface -> height;

	// send request to Window Manager
	std_ipc_send( wm_pid, (uint8_t *) request );

	// wait for answer
	uint64_t timeout = std_microtime() + 1024;	// wait no more than ~1 second
	while( ! std_ipc_receive( (uint8_t *) wm_data ) ) if( timeout < std_microtime() ) {
		// show error
		print( "Could not connect to Window Manager." );

		// failsafe exit
		exit();
	}

	// window assigned?
	answer = (struct STD_IPC_STRUCTURE_WINDOW_DESCRIPTOR *) &wm_data;
	if( ! answer -> descriptor ) {
		// show error
		print( "Window Manager denied request." );

		// failsafe exit
		exit();
	}

	// properties of console window
	interface -> descriptor = (struct STD_WINDOW_STRUCTURE_DESCRIPTOR *) answer -> descriptor;

	// set shadow length
	interface -> descriptor -> offset = LIB_INTERFACE_SHADOW_length;

	// prepare shadow of window
	lib_interface_shadow( interface );

	// clear window content
	lib_interface_clear( interface );

	// show window name in header if set
	lib_interface_name( interface );
}