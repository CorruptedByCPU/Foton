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
const uint8_t lib_interface_string_input[] = "input";
const uint8_t lib_interface_string_limit[] = "limit";
const uint8_t lib_interface_string_checkbox[] = "checkbox";
const uint8_t lib_interface_string_group[] = "group";
const uint8_t lib_interface_string_radio[] = "radio";
const uint8_t lib_interface_string_selected[] = "selected";
const uint8_t lib_interface_string_file[] = "file";
const uint8_t lib_interface_string_path[] = "path";

uint8_t lib_interface( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// prepare JSON structure for parsing
	lib_json_squeeze( interface -> properties );

	// convert interface properties to a more accessible format
	lib_interface_convert( interface );

	// if dimensions aquired from JSON structure
	if( interface -> width && interface -> height ) {
		// create window
		if( ! lib_interface_window( interface ) ) return FALSE;

		// show interface elements
		lib_interface_draw( interface );

		// select default element
		struct LIB_INTERFACE_STRUCTURE_SELECT select = lib_interface_select( interface );
		interface -> element_select = select.next;
	// no
	} else return FALSE;

	// done
	return TRUE;
}

void lib_interface_border( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// default border color
	uint32_t color = LIB_INTERFACE_BORDER_COLOR_default;
	uint32_t color_shadow = LIB_INTERFACE_BORDER_COLOR_default_shadow;

	// change border of window if not active
	if( ! (interface -> descriptor -> flags & STD_WINDOW_FLAG_active) ) { color = LIB_INTERFACE_BORDER_COLOR_inactive; color_shadow = LIB_INTERFACE_BORDER_COLOR_inactive_shadow; }

	// and point border
	uint32_t *pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR ));
	for( uint16_t y = 0; y < interface -> height; y++ )
		for( uint16_t x = 0; x < interface -> width; x++ ) {
			if( ! x || ! y ) { pixel[ (y * interface -> width) + x ] = color; }
			if( x == interface -> width - 1 || y == interface -> height - 1 ) pixel[ (y * interface -> width) + x ] = color_shadow;
		}
}

void lib_interface_clear( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// by default, color
	uint32_t background_color = LIB_INTERFACE_COLOR_background;
	if( interface -> background_color ) background_color = interface -> background_color;	// change to choosen one

	// fill window with default background
	uint32_t *pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR ));
	for( int64_t i = 0; i < interface -> width * interface -> height; i++ )
		// draw pixel
		pixel[ i ] = background_color;

	// show default border
	lib_interface_border( interface );
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
				// retrieve x value
				if( lib_json_key( window, (uint8_t *) &lib_interface_string_x ) ) interface -> x = window.value;
	
				// retrieve y value
				if( lib_json_key( window, (uint8_t *) &lib_interface_string_y ) ) interface -> y = window.value;

				// retrieve width value
				if( lib_json_key( window, (uint8_t *) &lib_interface_string_width ) ) interface -> width = window.value;
	
				// retrieve height value
				if( lib_json_key( window, (uint8_t *) &lib_interface_string_height ) ) interface -> height = window.value;

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
			element -> control.x = interface -> controls;	// order from right, not position
			element -> control.y = EMPTY;
			element -> control.width = LIB_INTERFACE_HEADER_HEIGHT_pixel;
			element -> control.height = LIB_INTERFACE_HEADER_HEIGHT_pixel;
			element -> control.flags = EMPTY;
			element -> control.size_byte = sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL );

			// parse all keys
			do {
				// id
				if( lib_json_key( control, (uint8_t *) &lib_interface_string_id ) ) element -> control.id = control.value;

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
				if( lib_json_key( label_or_button, (uint8_t *) &lib_interface_string_x ) ) element -> label_or_button.x = label_or_button.value;

				// y
				if( lib_json_key( label_or_button, (uint8_t *) &lib_interface_string_y ) ) element -> label_or_button.y = label_or_button.value;

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
				if( lib_json_key( menu, (uint8_t *) &lib_interface_string_x ) ) element -> menu.x = menu.value;

				// y
				if( lib_json_key( menu, (uint8_t *) &lib_interface_string_y ) ) element -> menu.y = menu.value;

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
					struct STD_STRUCTURE_FILE icon_file = { EMPTY };

					// properties of image
					struct LIB_IMAGE_STRUCTURE_TGA *icon_image = EMPTY;

					// retrieve information about module file
					uint8_t *icon_file_string = (uint8_t *) menu.value; icon_file_string[ menu.length ] = STD_ASCII_TERMINATOR;
					if( (icon_file.socket = std_file_open( (uint8_t *) menu.value, menu.length )) ) {
						// retrieve properties of file
						std_file( (struct STD_STRUCTURE_FILE *) &icon_file );

						// assign area for file
						icon_image = (struct LIB_IMAGE_STRUCTURE_TGA *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( icon_file.byte ) >> STD_SHIFT_PAGE );

						// load file content
						std_file_read( (struct STD_STRUCTURE_FILE *) &icon_file, (uint8_t *) icon_image, icon_file.byte );

						// copy image content to cursor object
						element -> icon = (uint32_t *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( icon_image -> width * icon_image -> height * STD_VIDEO_DEPTH_byte ) >> STD_SHIFT_PAGE );
						lib_image_tga_parse( (uint8_t *) icon_image, element -> icon, icon_file.byte );

						// release file content
						std_memory_release( (uintptr_t) icon_image, MACRO_PAGE_ALIGN_UP( icon_file.byte ) >> STD_SHIFT_PAGE );

						// close file
						std_file_close( icon_file.socket );
					}
				}
			// next key
			} while( lib_json_next( (struct LIB_JSON_STRUCTURE *) &menu ) );

			// change interface structure index
			i += element -> menu.size_byte;
		}

		// input?
		if( lib_json_key( json, (uint8_t *) &lib_interface_string_input ) ) {
			// alloc space for element
			properties = (uint8_t *) realloc( properties, i + sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_INPUT ) );

			// element structure position
			struct LIB_INTERFACE_STRUCTURE_ELEMENT_INPUT *element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_INPUT *) &properties[ i ];
	
			// input properties
			struct LIB_JSON_STRUCTURE input = lib_json( (uint8_t *) json.value );

			// default properties of input
			element -> input.type = LIB_INTERFACE_ELEMENT_TYPE_input;
			element -> input.flags = EMPTY;
			element -> input.size_byte = sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_INPUT );

			// parse all keys
			do {
				// id
				if( lib_json_key( input, (uint8_t *) &lib_interface_string_id ) ) element -> input.id = input.value;

				// x
				if( lib_json_key( input, (uint8_t *) &lib_interface_string_x ) ) element -> input.x = input.value;

				// y
				if( lib_json_key( input, (uint8_t *) &lib_interface_string_y ) ) element -> input.y = input.value;

				// width
				if( lib_json_key( input, (uint8_t *) &lib_interface_string_width ) ) element -> input.width = input.value;

				// height
				if( lib_json_key( input, (uint8_t *) &lib_interface_string_height ) ) element -> input.height = input.value;
		
				// length
				if( lib_json_key( input, (uint8_t *) &lib_interface_string_limit ) ) element -> name_length = input.value;

				// name
				if( lib_json_key( input, (uint8_t *) &lib_interface_string_name ) ) {
					// alloc area for element content
					uint8_t *name = (uint8_t *) calloc( element -> name_length + 1 );
					
					// set element content
					uint8_t *source = (uint8_t *) input.value;
					for( uint64_t i = 0; i < input.length; i++ ) name[ i ] = source[ i ];

					// set element content properties
					element -> name		= name;
					element -> offset	= EMPTY;
					element -> index	= EMPTY;
				}
			// next key
			} while( lib_json_next( (struct LIB_JSON_STRUCTURE *) &input ) );

			// change interface structure index
			i += element -> input.size_byte;
		}

		// checkbox?
		if( lib_json_key( json, (uint8_t *) &lib_interface_string_checkbox ) ) {
			// alloc space for element
			properties = (uint8_t *) realloc( properties, i + sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_CHECKBOX ) );

			// element structure position
			struct LIB_INTERFACE_STRUCTURE_ELEMENT_CHECKBOX *element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_CHECKBOX *) &properties[ i ];
	
			// checkbox properties
			struct LIB_JSON_STRUCTURE checkbox = lib_json( (uint8_t *) json.value );

			// default properties of checkbox
			element -> checkbox.type = LIB_INTERFACE_ELEMENT_TYPE_checkbox;
			element -> checkbox.flags = EMPTY;
			element -> checkbox.size_byte = sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_CHECKBOX );

			// parse all keys
			do {
				// element name alignment
				uint16_t element_name_align = EMPTY;

				// id
				if( lib_json_key( checkbox, (uint8_t *) &lib_interface_string_id ) ) element -> checkbox.id = checkbox.value;

				// x
				if( lib_json_key( checkbox, (uint8_t *) &lib_interface_string_x ) ) element -> checkbox.x = checkbox.value;

				// y
				if( lib_json_key( checkbox, (uint8_t *) &lib_interface_string_y ) ) element -> checkbox.y = checkbox.value;

				// width
				if( lib_json_key( checkbox, (uint8_t *) &lib_interface_string_width ) ) element -> checkbox.width = checkbox.value;

				// height
				if( lib_json_key( checkbox, (uint8_t *) &lib_interface_string_height ) ) element -> checkbox.height = checkbox.value;

				// selected
				if( lib_json_key( checkbox, (uint8_t *) &lib_interface_string_selected ) ) element -> checkbox.selected = checkbox.value;
		
				// name
				if( lib_json_key( checkbox, (uint8_t *) &lib_interface_string_name ) ) {
					// length if proper
					element -> name_length = checkbox.length;

					// alloc area for element name
					uint8_t *name_target = (uint8_t *) calloc( element -> name_length + 1 );

					// copy element name
					uint8_t *name_source = (uint8_t *) checkbox.value;
					for( uint64_t i = 0; i < element -> name_length; i++ ) name_target[ i ] = name_source[ i ];

					// update element name pointer
					element -> name = name_target;
				}

			// next key
			} while( lib_json_next( (struct LIB_JSON_STRUCTURE *) &checkbox ) );

			// change interface structure index
			i += element -> checkbox.size_byte;
		}

		// radio?
		if( lib_json_key( json, (uint8_t *) &lib_interface_string_radio ) ) {
			// alloc space for element
			properties = (uint8_t *) realloc( properties, i + sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_RADIO ) );

			// element structure position
			struct LIB_INTERFACE_STRUCTURE_ELEMENT_RADIO *element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_RADIO *) &properties[ i ];
	
			// radio properties
			struct LIB_JSON_STRUCTURE radio = lib_json( (uint8_t *) json.value );

			// default properties of radio
			element -> radio.type = LIB_INTERFACE_ELEMENT_TYPE_radio;
			element -> radio.flags = EMPTY;
			element -> radio.size_byte = sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_RADIO );

			// parse all keys
			do {
				// element name alignment
				uint16_t element_name_align = EMPTY;

				// id
				if( lib_json_key( radio, (uint8_t *) &lib_interface_string_id ) ) element -> radio.id = radio.value;

				// x
				if( lib_json_key( radio, (uint8_t *) &lib_interface_string_x ) ) element -> radio.x = radio.value;

				// y
				if( lib_json_key( radio, (uint8_t *) &lib_interface_string_y ) ) element -> radio.y = radio.value;

				// width
				if( lib_json_key( radio, (uint8_t *) &lib_interface_string_width ) ) element -> radio.width = radio.value;

				// height
				if( lib_json_key( radio, (uint8_t *) &lib_interface_string_height ) ) element -> radio.height = radio.value;
		
				// group
				if( lib_json_key( radio, (uint8_t *) &lib_interface_string_group ) ) element -> radio.group = radio.value;

				// selected
				if( lib_json_key( radio, (uint8_t *) &lib_interface_string_selected ) ) element -> radio.selected = radio.value;

				// name
				if( lib_json_key( radio, (uint8_t *) &lib_interface_string_name ) ) {
					// length if proper
					element -> name_length = radio.length;

					// alloc area for element name
					uint8_t *name_target = (uint8_t *) calloc( element -> name_length + 1 );

					// copy element name
					uint8_t *name_source = (uint8_t *) radio.value;
					for( uint64_t i = 0; i < element -> name_length; i++ ) name_target[ i ] = name_source[ i ];

					// update element name pointer
					element -> name = name_target;
				}

			// next key
			} while( lib_json_next( (struct LIB_JSON_STRUCTURE *) &radio ) );

			// change interface structure index
			i += element -> radio.size_byte;
		}

		// file?
		if( lib_json_key( json, (uint8_t *) &lib_interface_string_file ) ) {
			// alloc space for element
			properties = (uint8_t *) realloc( properties, i + sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE ) );

			// element structure position
			struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE *element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE *) &properties[ i ];
	
			// file properties
			struct LIB_JSON_STRUCTURE file = lib_json( (uint8_t *) json.value );

			// default properties of input
			element -> file.type = LIB_INTERFACE_ELEMENT_TYPE_file;
			element -> file.flags = EMPTY;
			element -> file.size_byte = sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE );

			// parse all keys
			do {
				// id
				if( lib_json_key( file, (uint8_t *) &lib_interface_string_id ) ) element -> file.id = file.value;

				// x
				if( lib_json_key( file, (uint8_t *) &lib_interface_string_x ) ) element -> file.x = file.value;

				// y
				if( lib_json_key( file, (uint8_t *) &lib_interface_string_y ) ) element -> file.y = file.value;

				// width
				if( lib_json_key( file, (uint8_t *) &lib_interface_string_width ) ) element -> file.width = file.value;

				// height
				if( lib_json_key( file, (uint8_t *) &lib_interface_string_height ) ) element -> file.height = file.value;		

				// path?
				if( lib_json_key( file, (uint8_t *) &lib_interface_string_path ) ) {
					// length if proper
					element -> name_length = file.length;

					// alloc area for element name
					uint8_t *name_target = (uint8_t *) calloc( element -> name_length + 1 );

					// copy element name
					uint8_t *name_source = (uint8_t *) file.value;
					for( uint64_t i = 0; i < element -> name_length; i++ ) name_target[ i ] = name_source[ i ];

					// update element name pointer
					element -> name = name_target;
				}
			// next key
			} while( lib_json_next( (struct LIB_JSON_STRUCTURE *) &file ) );

			// change interface structure index
			i += element -> file.size_byte;
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
		// redraw element inside object
		lib_interface_draw_select( interface, element );
	
		// next element properties
		element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) ((uint64_t) element + element -> size_byte);
	}
}

void lib_interface_draw_select( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT *element ) {			// redraw element inside object
	// select redraw function
	switch( element -> type ) {
		case LIB_INTERFACE_ELEMENT_TYPE_label: { lib_interface_element_label( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *) element ); break; }
		case LIB_INTERFACE_ELEMENT_TYPE_button: { lib_interface_element_button( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *) element ); break; }
		case LIB_INTERFACE_ELEMENT_TYPE_control_close: { lib_interface_element_control( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *) element ); break; }
		case LIB_INTERFACE_ELEMENT_TYPE_control_maximize: { lib_interface_element_control( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *) element ); break; }
		case LIB_INTERFACE_ELEMENT_TYPE_control_minimize: { lib_interface_element_control( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *) element ); break; }
		case LIB_INTERFACE_ELEMENT_TYPE_menu: { lib_interface_element_menu( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU *) element ); break; }
		case LIB_INTERFACE_ELEMENT_TYPE_input: { lib_interface_element_input( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT_INPUT *) element ); break; }
		case LIB_INTERFACE_ELEMENT_TYPE_checkbox: { lib_interface_element_checkbox( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT_CHECKBOX *) element ); break; }
		case LIB_INTERFACE_ELEMENT_TYPE_radio: { lib_interface_element_radio( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT_RADIO *) element ); break; }
		case LIB_INTERFACE_ELEMENT_TYPE_file: { lib_interface_element_file( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE *) element ); break; }
	}
}

void lib_interface_element_button( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *element ) {
	// limit string length to element width
	uint64_t limit = lib_interface_string( LIB_FONT_FAMILY_ROBOTO, element -> name, element -> name_length, element -> label_or_button.width );

	// compute absolute address of first pixel of element space
	uint32_t *pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR )) + (element -> label_or_button.y * interface -> width) + element -> label_or_button.x;

	// select background color
	uint32_t color = LIB_INTERFACE_COLOR_background_button_default;
	if( element -> label_or_button.flags & LIB_INTERFACE_ELEMENT_FLAG_hover ) color += LIB_INTERFACE_COLOR_background_lighter;
	if( element -> label_or_button.flags & LIB_INTERFACE_ELEMENT_FLAG_active ) color += LIB_INTERFACE_COLOR_background_lighter;

	// fill element with background color
	for( uint16_t y = 0; y < element -> label_or_button.height; y++ )
		for( uint16_t x = 0; x < element -> label_or_button.width; x++ )
			pixel[ (y * interface -> width) + x ] = color;

	// vertical align of element content
	if( element -> label_or_button.height > LIB_FONT_HEIGHT_pixel ) pixel += ((element -> label_or_button.height - LIB_FONT_HEIGHT_pixel) >> STD_SHIFT_2) * interface -> width;

	// horizontal align of element content
	pixel += element -> label_or_button.width >> STD_SHIFT_2;

	// display the content of element
	lib_font( LIB_FONT_FAMILY_ROBOTO, element -> name, limit, LIB_INTERFACE_COLOR_foreground, (uint32_t *) pixel, interface -> width, LIB_FONT_ALIGN_center );
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

void lib_interface_element_checkbox( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_CHECKBOX *element ) {
	// limit string length to element width
	uint64_t limit = lib_interface_string( LIB_FONT_FAMILY_ROBOTO, element -> name, element -> name_length, (element -> checkbox.width - element -> checkbox.height) );

	// compute absolute address of first pixel of element space
	uint32_t *pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR )) + (element -> checkbox.y * interface -> width) + element -> checkbox.x;

	// select background color
	uint32_t color = LIB_INTERFACE_COLOR_background_checkbox_default;
	if( element -> checkbox.flags & LIB_INTERFACE_ELEMENT_FLAG_hover ) color += LIB_INTERFACE_COLOR_background_lighter;
	if( element -> checkbox.flags & LIB_INTERFACE_ELEMENT_FLAG_active ) color += LIB_INTERFACE_COLOR_background_lighter;

	// clean background
	for( uint16_t y = 0; y < element -> checkbox.height; y++ )
		for( uint16_t x = 0; x < element -> checkbox.width; x++ )
			pixel[ (y * interface -> width) + x ] = LIB_INTERFACE_COLOR_background;

	// checkbox
	for( uint16_t y = 0; y < element -> checkbox.height; y++ )
		for( uint16_t x = 0; x < element -> checkbox.height; x++ )
			pixel[ (y * interface -> width) + x ] = color;

	// selected?
	if( element -> checkbox.selected ) {
		color = LIB_INTERFACE_COLOR_background_checkbox_selected;
		if( element -> checkbox.flags & LIB_INTERFACE_ELEMENT_FLAG_active ) color += LIB_INTERFACE_COLOR_background_lighter;
		if( element -> checkbox.flags & LIB_INTERFACE_ELEMENT_FLAG_hover ) color += LIB_INTERFACE_COLOR_background_lighter;
		for( uint16_t y = 0; y < element -> checkbox.height; y++ )
			for( uint16_t x = 0; x < element -> checkbox.height; x++ )
				pixel[ (y * interface -> width) + x ] = color;
	}

	// vertical align of element content
	if( element -> checkbox.height > LIB_FONT_HEIGHT_pixel ) pixel += ((element -> checkbox.height - LIB_FONT_HEIGHT_pixel) >> STD_SHIFT_2) * interface -> width;

	// display the content of element
	lib_font( LIB_FONT_FAMILY_ROBOTO, element -> name, limit, LIB_INTERFACE_COLOR_foreground, (uint32_t *) pixel + element -> checkbox.height + 4, interface -> width, EMPTY );
}

void lib_interface_element_control( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *element ) {
	// properties of control buttons of window
	uint32_t *pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR )) + interface -> width - LIB_INTERFACE_HEADER_HEIGHT_pixel - (element -> control.x * LIB_INTERFACE_HEADER_HEIGHT_pixel) - LIB_INTERFACE_BORDER_pixel;

	// choose background color
	uint32_t background_color = LIB_INTERFACE_COLOR_background;
	if( element -> control.flags & LIB_INTERFACE_ELEMENT_FLAG_hover ) {
		if( element -> control.type == LIB_INTERFACE_ELEMENT_TYPE_control_close ) background_color = LIB_INTERFACE_COLOR_background_control_close_hover;
		else background_color += LIB_INTERFACE_COLOR_background_lighter;
	}

	// clear element space
	for( uint8_t y = TRUE; y < LIB_INTERFACE_HEADER_HEIGHT_pixel; y++ )
		for( uint8_t x = 0; x < LIB_INTERFACE_HEADER_HEIGHT_pixel; x++ )
			pixel[ (y * interface -> width) + x ] = background_color;

	// choose element type
	switch( element -> control.type ) {	
		case LIB_INTERFACE_ELEMENT_TYPE_control_minimize: {
			// display minimize window button
			for( uint64_t x = 8; x <= 16; x++ ) pixel[ (16 * interface -> width) + x ] = 0xFFC0C0C0;

			// done
			break;
		}
		case LIB_INTERFACE_ELEMENT_TYPE_control_maximize: {
			// display window maximize button
			for( uint64_t y = 8; y <= 16; y++ ) {
				pixel[ (8 * interface -> width) + y ] = 0xFFC0C0C0;
				pixel[ (y * interface -> width) + 8 ] = 0xFFC0C0C0;
				pixel[ (y * interface -> width) + 16 ] = 0xFFC0C0C0;
				pixel[ (16 * interface -> width) + y ] = 0xFFC0C0C0;
			}

			// done
			break;
		}
		case LIB_INTERFACE_ELEMENT_TYPE_control_close: {
			// display close window button
			for( uint64_t y = 8; y <= 16; y++ ) {
				pixel[ (y * interface -> width) + y ] = 0xFFF0F0F0;
				pixel[ (LIB_INTERFACE_HEADER_HEIGHT_pixel - y) + (y * interface -> width) ] = 0xFFF0F0F0;
			}

			// done
			break;
		}
	}
}

void lib_interface_element_input( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_INPUT *element ) {
	// limit string length to element width
	uint64_t name_length = lib_string_length( element -> name ) - element -> offset;
	while( lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, element -> name + element -> offset, name_length ) > element -> input.width - 4 ) if( ! --name_length ) return;

	// compute absolute address of first pixel of element space
	if( element -> input.x == STD_MAX_unsigned ) element -> input.x = (interface -> width >> STD_SHIFT_2) - (element -> input.x >> STD_SHIFT_2);
	if( element -> input.y == STD_MAX_unsigned ) element -> input.y = (interface -> height >> STD_SHIFT_2) - (element -> input.y >> STD_SHIFT_2);
	uint32_t *pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR )) + (element -> input.y * interface -> width) + element -> input.x;

	// select background color
	uint32_t color = LIB_INTERFACE_COLOR_background_input_default;
	if( element -> input.flags & LIB_INTERFACE_ELEMENT_FLAG_hover ) color += LIB_INTERFACE_COLOR_background_lighter;
	if( element -> input.flags & LIB_INTERFACE_ELEMENT_FLAG_active ) color += LIB_INTERFACE_COLOR_background_lighter;

	// fill element with background color
	for( uint16_t y = 0; y < element -> input.height; y++ )
		for( uint16_t x = 0; x < element -> input.width; x++ )
			pixel[ (y * interface -> width) + x ] = color;

	// vertical align of element content
	uint32_t *pixel_string = pixel;
	if( element -> input.height > LIB_FONT_HEIGHT_pixel ) pixel_string += ((element -> input.height - LIB_FONT_HEIGHT_pixel) >> STD_SHIFT_2) * interface -> width;

	// display the content of element
	lib_font( LIB_FONT_FAMILY_ROBOTO_MONO, element -> name + element -> offset, name_length, LIB_INTERFACE_COLOR_foreground, (uint32_t *) pixel_string + 4, interface -> width, LIB_FONT_ALIGN_left );

	// if element is active
	if( interface -> element_select != (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) element ) return;	// nope

	// show cursor position 
	uint64_t x = lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, element -> name + element -> offset, element -> index - element -> offset );
	for( uint64_t y = 2; y < element -> input.height - 2; y++ )
		pixel[ (y * interface -> width) + x + 4 ] = STD_COLOR_WHITE;
}

void lib_interface_element_label( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *element ) {
	// limit string length to element width
	uint64_t limit = lib_interface_string( LIB_FONT_FAMILY_ROBOTO, element -> name, element -> name_length, element -> label_or_button.width );

	// compute absolute address of first pixel of element space
	uint32_t *pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR )) + (element -> label_or_button.y * interface -> width) + element -> label_or_button.x;

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
	lib_font( LIB_FONT_FAMILY_ROBOTO, element -> name, limit, LIB_INTERFACE_COLOR_foreground, (uint32_t *) pixel, interface -> width, element -> label_or_button.flags );
}

void lib_interface_element_menu( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU *element ) {
	// limit string length to element width
	uint64_t limit = lib_interface_string( LIB_FONT_FAMILY_ROBOTO, element -> name, element -> name_length, element -> menu.width );

	// compute absolute address of first pixel of element space
	uint32_t *pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR )) + (element -> menu.y * interface -> width) + element -> menu.x;

	// choose background color
	uint32_t color = LIB_INTERFACE_COLOR_background_menu_default;
	if( element -> menu.flags & LIB_INTERFACE_ELEMENT_FLAG_hover ) color += LIB_INTERFACE_COLOR_background_lighter;
	if( element -> menu.flags & LIB_INTERFACE_ELEMENT_FLAG_active ) color += LIB_INTERFACE_COLOR_background_lighter + LIB_INTERFACE_COLOR_background_menu_selected;

	// fill element with background color
	for( uint16_t y = 0; y < element -> menu.height; y++ )
		for( uint16_t x = 0; x < element -> menu.width; x++ )
			pixel[ (y * interface -> width) + x ] = color;

	// display the content of element
	lib_font( LIB_FONT_FAMILY_ROBOTO, element -> name, limit, LIB_INTERFACE_COLOR_foreground, (uint32_t *) pixel + 4 + 16 + 2 + (((LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel - LIB_FONT_HEIGHT_pixel) >> STD_SHIFT_2) * interface -> width), interface -> width, LIB_FONT_ALIGN_left );

	// icon provided?
	if( element -> icon ) {
		// compute absolute address of first pixel of icon
		uint32_t *icon = (uint32_t *) pixel + 4 + (((LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel - LIB_FONT_HEIGHT_pixel) >> STD_SHIFT_2) * interface -> width);

		// copy scaled image content to element area
		double x_scale_factor = (double) (48.0f / 16.0f);
		double y_scale_factor = (double) (48.0f / 16.0f);

		// load icon to element area
		for( uint16_t y = 0; y < 16; y++ )
			for( uint16_t x = 0; x < 16; x++ )
				icon[ (y * interface -> width) + x ] = lib_color_blend( icon[ (y * interface -> width) + x ], element -> icon[ (uint64_t) (((uint64_t) (y_scale_factor * y) * 48) + (uint64_t) (x * x_scale_factor)) ] );
	}
}

void lib_interface_element_radio( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_RADIO *element ) {
	// limit name length to header width
	uint64_t limit = lib_interface_string( LIB_FONT_FAMILY_ROBOTO, element -> name, element -> name_length, (element -> radio.width - element -> radio.height) );

	// compute absolute address of first pixel of element space
	uint32_t *pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR )) + (element -> radio.y * interface -> width) + element -> radio.x;

	// select background color
	uint32_t color = LIB_INTERFACE_COLOR_background_radio_default;
	if( element -> radio.flags & LIB_INTERFACE_ELEMENT_FLAG_hover ) color += LIB_INTERFACE_COLOR_background_lighter;
	if( element -> radio.flags & LIB_INTERFACE_ELEMENT_FLAG_active ) color += LIB_INTERFACE_COLOR_background_lighter;

	// clean background
	for( uint16_t y = 0; y < element -> radio.height; y++ )
		for( uint16_t x = 0; x < element -> radio.width; x++ )
			pixel[ (y * interface -> width) + x ] = LIB_INTERFACE_COLOR_background;

	// radio
	uint64_t o = 0;
	for( uint16_t y = 0; y < element -> radio.height; y++ ) {
		for( uint16_t x = (element -> radio.height >> STD_SHIFT_2) - o; x < (element -> radio.height >> STD_SHIFT_2) + o; x++ )
			pixel[ (y * interface -> width) + x ] = color;

		if( y < (element -> radio.height >> STD_SHIFT_2) ) o++;
		else o--;
	}

	// selected?
	if( element -> radio.selected ) {
		color = LIB_INTERFACE_COLOR_background_radio_selected;
		if( element -> radio.flags & LIB_INTERFACE_ELEMENT_FLAG_active ) color += LIB_INTERFACE_COLOR_background_lighter;
		if( element -> radio.flags & LIB_INTERFACE_ELEMENT_FLAG_hover ) color += LIB_INTERFACE_COLOR_background_lighter;
		o = 0;
		for( uint16_t y = 0; y < element -> radio.height; y++ ) {
			for( uint16_t x = (element -> radio.height >> STD_SHIFT_2) - o; x < (element -> radio.height >> STD_SHIFT_2) + o; x++ )
				pixel[ (y * interface -> width) + x ] = color;

			if( y < (element -> radio.height >> STD_SHIFT_2) ) o++;
			else o--;
		}
	}

	// vertical align of element content
	if( element -> radio.height > LIB_FONT_HEIGHT_pixel ) pixel += ((element -> radio.height - LIB_FONT_HEIGHT_pixel) >> STD_SHIFT_2) * interface -> width;

	// display the content of element
	lib_font( LIB_FONT_FAMILY_ROBOTO, element -> name, limit, LIB_INTERFACE_COLOR_foreground, (uint32_t *) pixel + element -> radio.height + 4, interface -> width, EMPTY );
}

void lib_interface_element_file( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE *element ) {
	// compute absolute address of first pixel of element space
	uint32_t *pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR )) + (element -> file.y * interface -> width) + element -> file.x;

	// dimensions of element
	uint16_t width = element -> file.width;
	uint16_t height = element -> file.height;
	if( width == (uint16_t) STD_MAX_unsigned ) width = interface -> width - (element -> file.x + LIB_INTERFACE_BORDER_pixel);
	if( height == (uint16_t) STD_MAX_unsigned ) height = interface -> height - (element -> file.y + LIB_INTERFACE_BORDER_pixel);

	// clean background
	uint32_t color = LIB_INTERFACE_COLOR_background_file_default;
	for( uint16_t y = 0; y < height; y++ )
		for( uint16_t x = 0; x < width; x++ )
			pixel[ (y * interface -> width) + x ] = color;

	// if not open yet
	if( ! element -> socket ) {
		// open file
		element -> socket = fopen( element -> name );

		// alloc area for file content
		element -> content = (uint8_t *) malloc( element -> socket -> byte );

		// load file content
		fread( element -> socket, element -> content, element -> socket -> byte );

		// redraw of element content is required
		element -> file.flags |= LIB_INTERFACE_ELEMENT_FLAG_flush;
	}

	// flush requested?
	if( ! (element -> file.flags & LIB_INTERFACE_ELEMENT_FLAG_flush) ) return;	// no

	// amount of files to show
	element -> limit = -LIB_VFS_default;
	for( uint64_t b = 0; b < (element -> socket -> byte >> STD_SHIFT_PAGE); b++ ) {
		// properties of directory entry
		struct LIB_VFS_STRUCTURE *entry = (struct LIB_VFS_STRUCTURE *) &element -> content[ b * STD_PAGE_byte ];

		// file exist?
		for( uint64_t e = 0; e < STD_PAGE_byte / sizeof( struct LIB_VFS_STRUCTURE ); e++ )
			if( entry[ e ].name_length ) element -> limit++;
	}

	// expand/shrink previous area to meet actual requirements
	if( element -> area ) element -> area = realloc( element -> area, (element -> limit * ((LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel) * width)) << STD_VIDEO_DEPTH_shift );
	else element -> area = malloc( (element -> limit * ((LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel) * width)) << STD_VIDEO_DEPTH_shift );

	// set background color for each entry
	color = LIB_INTERFACE_COLOR_background_file_default;
	uint64_t change = -1;
	for( uint64_t y = 0; y < (element -> limit * (LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel)); y++ ) {
		if( ! (y % LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel) ) change++;
		if( change % 2 ) color = LIB_INTERFACE_COLOR_background_file_odd;
		else color = LIB_INTERFACE_COLOR_background_file_default;
		if( change == element -> selected ) color = LIB_INTERFACE_COLOR_background_file_selected;

		for( uint64_t x = 0; x < width; x++ ) {
			element -> area[ (y * width) + x ] = color;
		}
	}

	// load default icons

	// default
	uint8_t local_file_path_default[] = "/system/var/gfx/icons/text-plain.tga";
	uint32_t *local_icon_default = lib_interface_icon( (uint8_t *) &local_file_path_default, sizeof( local_file_path_default ) - 1 );
	// directory
	uint8_t local_file_path_directory[] = "/system/var/gfx/icons/folder-green.tga";
	uint32_t *local_icon_directory = lib_interface_icon( (uint8_t *) &local_file_path_directory, sizeof( local_file_path_directory ) - 1 );

	// display the content of element
	uint32_t *pixel_entry = element -> area + (((LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel - LIB_FONT_HEIGHT_pixel) >> STD_SHIFT_2) * width);
	uint64_t entry_count = EMPTY;
	for( uint64_t b = 0; b < (element -> socket -> byte >> STD_SHIFT_PAGE); b++ ) {
		// properties of directory entry
		struct LIB_VFS_STRUCTURE *entry = (struct LIB_VFS_STRUCTURE *) &element -> content[ b * STD_PAGE_byte ];

		// file exist?
		for( uint64_t e = 0; e < STD_PAGE_byte / sizeof( struct LIB_VFS_STRUCTURE ); e++ ) if( entry[ e ].name_length ) {
			// movement links?
			if( (entry[ e ].name_length == TRUE && *entry[ e ].name == STD_ASCII_DOT) || (entry[ e ].name_length == 2 && lib_string_compare( entry[ e ].name, (uint8_t *) "..", 2 )) ) continue;	// ignore

			// select icon
			uint32_t *icon_source = local_icon_default;
			if( entry[ e ].type == STD_FILE_TYPE_directory ) icon_source = local_icon_directory;

			// compute absolute address of first pixel of icon
			uint32_t *pixel_icon = (uint32_t *) pixel_entry + 4 - width;

			// copy scaled image content to element area
			double x_scale_factor = (double) (48.0f / 16.0f);
			double y_scale_factor = (double) (48.0f / 16.0f);

			// load icon to element area
			for( uint16_t y = 0; y < 16; y++ ) {
				for( uint16_t x = 0; x < 16; x++ )
					pixel_icon[ (y * width) + x ] = lib_color_blend( pixel_icon[ (y * width) + x ], icon_source[ (uint64_t) (((uint64_t) (y_scale_factor * y) * 48) + (uint64_t) (x * x_scale_factor)) ] );
			}

			// limit name length to header width
			uint8_t *string = (uint8_t *) calloc( entry[ e ].name_length + 1);
			for( uint64_t i = 0; i < entry[ e ].name_length; i++ ) string[ i ] = entry[ e ].name[ i ];
			uint64_t limit = lib_interface_string( LIB_FONT_FAMILY_ROBOTO, string, entry[ e ].name_length, width - (4 + 16 + 2 + LIB_FONT_WIDTH_pixel + lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) "0000.0 X", 8 ) + 4) );

			// display the content of element
			lib_font( LIB_FONT_FAMILY_ROBOTO, string, limit, LIB_INTERFACE_COLOR_foreground, pixel_entry + 4 + 16 + 2, width, LIB_FONT_ALIGN_left );
			free( string );

			// unity type
			uint8_t unit = 0;	// bytes by default
			while( pow( 1024, unit ) < entry[ e ].byte ) unit++;
			uint8_t *test;
			uint64_t test_limit;
			if( unit > 1 )
				test = lib_float_to_string( (double) entry[ e ].byte / (double) pow( 1024, unit - 1 ), 1 );
			else {
				test = calloc( 4 + 1 );
				test_limit = lib_integer_to_string( entry[ e ].byte, STD_NUMBER_SYSTEM_decimal, test );
			}

			test_limit = lib_string_length( test );
			test = realloc( test, test_limit + 2 );
			test[ test_limit     ] = STD_ASCII_SPACE;
			test[ test_limit + 1 ] = lib_type_byte( entry[ e ].byte );

			lib_font( LIB_FONT_FAMILY_ROBOTO_MONO, test, lib_string_length( test ), LIB_INTERFACE_COLOR_foreground, pixel_entry + width - 4, width, LIB_FONT_ALIGN_right );

			free( test );

			// move pixel pointer to next entry
			pixel_entry += ((LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel) * width);
		}
	}

	free( local_icon_default );
	free( local_icon_directory );

	// which part of area should we see
	if( (element -> selected + 1) * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel >= element -> offset + height ) element -> offset = ((element -> selected + 1) * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel) - height;
	else if( element -> selected * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel < element -> offset ) element -> offset = element -> selected * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel;

	// sync entries
	uint32_t *area_offset = element -> area + (element -> offset * width);
	for( size_t y = 0; y < (element -> limit * (LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel)) && y < height; y++ )
		for( size_t x = 0; x < width; x++ )
			pixel[ (y * interface -> width) + x ] = area_offset[ (y * width) + x ];

	interface -> descriptor -> flags |= STD_WINDOW_FLAG_flush;

	// done
	// element -> file.flags &= ~LIB_INTERFACE_ELEMENT_FLAG_flush;
}

struct LIB_INTERFACE_STRUCTURE *lib_interface_event( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// incomming message
	uint8_t ipc_data[ STD_IPC_SIZE_byte ];

	// receive pending messages
	if( std_ipc_receive_by_type( (uint8_t *) &ipc_data, STD_IPC_TYPE_mouse ) ) {
		// message properties
		struct STD_STRUCTURE_IPC_MOUSE *mouse = (struct STD_STRUCTURE_IPC_MOUSE *) &ipc_data;

		// released left mouse button?
		if( mouse -> button == (uint8_t) ~STD_IPC_MOUSE_BUTTON_left ) lib_interface_event_handler( interface );
	}

	//--------------------------------------------------------------------------------
	// "hover over elements"
	//--------------------------------------------------------------------------------
	lib_interface_active_or_hover( interface );

	// acquired new window properties?
	if( interface -> descriptor -> flags & STD_WINDOW_FLAG_properties ) {
		// disable flag
		interface -> descriptor -> flags ^= STD_WINDOW_FLAG_properties;

		// minimal dimesions are preserved?
		if( interface -> min_width > interface -> descriptor -> new_width ) interface -> descriptor -> new_width = interface -> min_width;	// no, set correction
		if( interface -> min_height > interface -> descriptor -> new_height ) interface -> descriptor -> new_height = interface -> min_height;	// no, set correction

		// alloc area for new interface properties
		struct LIB_INTERFACE_STRUCTURE *new_interface = (struct LIB_INTERFACE_STRUCTURE *) malloc( sizeof( struct LIB_INTERFACE_STRUCTURE ) );

		// copy required interface properties from old one
		//----------------------------------------------------------------------
		new_interface -> properties		= interface -> properties;
		//----------------------------------------------------------------------
		new_interface -> previous_x		= interface -> previous_x;
		new_interface -> previous_y		= interface -> previous_y;
		new_interface -> previous_width		= interface -> previous_width;
		new_interface -> previous_height	= interface -> previous_height;
		//----------------------------------------------------------------------
		new_interface -> min_width		= interface -> min_width;
		new_interface -> min_height		= interface -> min_height;
		//----------------------------------------------------------------------
		new_interface -> controls		= interface -> controls;
		new_interface -> element_select		= interface -> element_select;
		new_interface -> background_color	= interface -> background_color;
		//----------------------------------------------------------------------

		// set new location and dimension
		new_interface -> x	= interface -> descriptor -> new_x;
		new_interface -> y	= interface -> descriptor -> new_y;
		new_interface -> width	= interface -> descriptor -> new_width;
		new_interface -> height	= interface -> descriptor -> new_height;

		// copy window name
		new_interface -> name_length = EMPTY;
		for( uint64_t i = 0; i < interface -> name_length; i++ ) new_interface -> name[ new_interface -> name_length++ ] = interface -> name[ i ];

		// create new window
		if( ! lib_interface_window( new_interface ) ) {
			// release new interface area
			free( new_interface );

			// nothing to do
			return EMPTY;
		}

		// show window name in header if set
		lib_interface_name( new_interface );

		// show interface elements
		lib_interface_draw( new_interface );

		// release old interface window
		interface -> descriptor -> flags |= STD_WINDOW_FLAG_release;

		// release old interface area
		free( interface );

		// new window created
		return new_interface;
	}

	// active window change?
	if( (interface -> descriptor -> flags & STD_WINDOW_FLAG_active) != interface -> active_semaphore ) {
		// remember current status
		interface -> active_semaphore = interface -> descriptor -> flags & STD_WINDOW_FLAG_active;

		// update window border
		lib_interface_border( interface );

		// and header
		lib_interface_name_rewrite( interface );

		// update window content
		interface -> descriptor -> flags |= STD_WINDOW_FLAG_flush;
	}

	// nothing to do
	return EMPTY;
}

void lib_interface_event_handler( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// check which element is under cursor position
	uint8_t *element = (uint8_t *) interface -> properties; uint64_t e = 0;
	while( element[ e ] != LIB_INTERFACE_ELEMENT_TYPE_null ) {
		// element properties
		struct LIB_INTERFACE_STRUCTURE_ELEMENT *properties = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) &element[ e ];

		// cursor overlaps this element? (check only if object is located under cursor)
		if( ((properties -> type == LIB_INTERFACE_ELEMENT_TYPE_control_close || properties -> type == LIB_INTERFACE_ELEMENT_TYPE_control_maximize || properties -> type == LIB_INTERFACE_ELEMENT_TYPE_control_minimize) && (interface -> descriptor -> x >= interface -> width - (LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel + (properties -> x * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel)) && interface -> descriptor -> x < ((interface -> width - (LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel + (properties -> x * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel)))) + properties -> width && interface -> descriptor -> y >= properties -> y && interface -> descriptor -> y < properties -> y + properties -> height)) || interface -> descriptor -> x >= properties -> x && interface -> descriptor -> x < properties -> x + properties -> width && interface -> descriptor -> y >= properties -> y && interface -> descriptor -> y < properties -> y + properties -> height ) {
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

				case LIB_INTERFACE_ELEMENT_TYPE_control_maximize: {
					// window already maximized?
					if( interface -> previous_x || interface -> previous_y || interface -> previous_width || interface -> previous_height ) {
						// request old window properties
						interface -> descriptor -> new_x = interface -> previous_x;
						interface -> descriptor -> new_y = interface -> previous_y;
						interface -> descriptor -> new_width = interface -> previous_width;
						interface -> descriptor -> new_height = interface -> previous_height;

						// reset previous properties
						interface -> previous_x		= EMPTY;
						interface -> previous_y		= EMPTY;
						interface -> previous_width	= EMPTY;
						interface -> previous_height	= EMPTY;

						// inform interface library about request
						interface -> descriptor -> flags |= STD_WINDOW_FLAG_properties;
					} else {
						// preserve current window properties
						interface -> previous_x		= interface -> x;
						interface -> previous_y		= interface -> y;
						interface -> previous_width	= interface -> width;
						interface -> previous_height	= interface -> height;

						// maximize window
						interface -> descriptor -> flags |= STD_WINDOW_FLAG_maximize;
					}

					// done
					break;
				}

				case LIB_INTERFACE_ELEMENT_TYPE_control_minimize: {
					// minimize window
					interface -> descriptor -> flags |= STD_WINDOW_FLAG_minimize;

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

				case LIB_INTERFACE_ELEMENT_TYPE_checkbox: {
					// properties of checkbox
					struct LIB_INTERFACE_STRUCTURE_ELEMENT *checkbox = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) properties;

					// active element
					interface -> element_select = checkbox;

					// set selected semaphore
					if( checkbox -> selected ) checkbox -> selected = FALSE;
					else checkbox -> selected = TRUE;

					// mark is on interface
					lib_interface_draw_select( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) checkbox );

					// redraw window content
					interface -> descriptor -> flags |= STD_WINDOW_FLAG_flush;

					// done
					break;
				}

				case LIB_INTERFACE_ELEMENT_TYPE_radio: {
					// properties of checkbox
					struct LIB_INTERFACE_STRUCTURE_ELEMENT *radio = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) properties;

					// first element properties
					struct LIB_INTERFACE_STRUCTURE_ELEMENT *element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) interface -> properties;

					// active element
					interface -> element_select = radio;

					// disable selection for any other element of that group
					while( element -> type != LIB_INTERFACE_ELEMENT_TYPE_null ) {
						// part of group?
						if( element == radio ) element -> selected = TRUE;
						else if( element -> group == radio -> group ) element -> selected = FALSE;
					
						// mark is on interface
						lib_interface_draw_select( interface, element );

						// next element properties
						element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) ((uint64_t) element + element -> size_byte);
					}

					// redraw window content
					interface -> descriptor -> flags |= STD_WINDOW_FLAG_flush;

					// done
					break;
				}

				case LIB_INTERFACE_ELEMENT_TYPE_input: {
					// properties of checkbox
					struct LIB_INTERFACE_STRUCTURE_ELEMENT *element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) properties;

					// selected element
					if( interface -> element_select && interface -> element_select != element && interface -> element_select -> type == LIB_INTERFACE_ELEMENT_TYPE_input ) {
						struct LIB_INTERFACE_STRUCTURE_ELEMENT *release = interface -> element_select;
						interface -> element_select -> flags &= ~LIB_INTERFACE_ELEMENT_FLAG_active;
						interface -> element_select = EMPTY;
						lib_interface_draw_select( interface, release );	
					}
					interface -> element_select = element;

					// set selected semaphore
					element -> selected = TRUE;

					// mark is on interface
					lib_interface_draw_select( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) element );

					// redraw window content
					interface -> descriptor -> flags |= STD_WINDOW_FLAG_flush;

					// done
					break;
				}
			}
		}

		// next element from list
		e += properties -> size_byte;
	}
}

uint16_t lib_interface_event_keyboard( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// incomming message
	uint8_t ipc_data[ STD_IPC_SIZE_byte ];

	// receive pending messages
	if( ! std_ipc_receive_by_type( (uint8_t *) &ipc_data, STD_IPC_TYPE_keyboard ) ) return EMPTY;

	// message properties
	struct STD_STRUCTURE_IPC_KEYBOARD *keyboard = (struct STD_STRUCTURE_IPC_KEYBOARD *) &ipc_data;

	// pressed LEFT ALT key?
	if( keyboard -> key == STD_KEY_ALT_LEFT ) { interface -> key_alt_semaphore = TRUE; return keyboard -> key; }
	if( keyboard -> key == (STD_KEY_ALT_LEFT | STD_KEY_RELEASE) ) interface -> key_alt_semaphore = FALSE;

	// pressed LEFT CTRL key?
	if( keyboard -> key == STD_KEY_CTRL_LEFT ) { interface -> key_ctrl_semaphore = TRUE; return keyboard -> key; }
	if( keyboard -> key == (STD_KEY_CTRL_LEFT | STD_KEY_RELEASE) ) interface -> key_ctrl_semaphore = FALSE;

	// pressed SHIFT key?
	if( keyboard -> key == STD_KEY_SHIFT_LEFT ) { interface -> key_shift_semaphore = TRUE; return keyboard -> key; }
	if( keyboard -> key == (STD_KEY_SHIFT_LEFT | STD_KEY_RELEASE) ) interface -> key_shift_semaphore = FALSE;

	// key releaase?
	if( keyboard -> key & STD_KEY_RELEASE ) return keyboard -> key;

	// ignore any key, when ALT key is on hold
	if( interface -> key_alt_semaphore ) { log( "ALT on hold.\n" ); return keyboard -> key; }

	// TAB key?
	if( keyboard -> key == STD_KEY_TAB ) {
		// find previous and next element of interface
		struct LIB_INTERFACE_STRUCTURE_SELECT select = lib_interface_select( interface );

		// by default next will be selected
		struct LIB_INTERFACE_STRUCTURE_ELEMENT *selected = select.next;
		if( interface -> key_shift_semaphore ) selected = select.previous;

		// update interface only if selected and current are different
		if( interface -> element_select != selected ) {
			// selected element
			if( interface -> element_select && interface -> element_select -> type == LIB_INTERFACE_ELEMENT_TYPE_input ) {
				// preserve element pointer
				struct LIB_INTERFACE_STRUCTURE_ELEMENT_INPUT *deselected = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_INPUT *) interface -> element_select;
		
				// clear element state
				deselected -> input.flags &= ~LIB_INTERFACE_ELEMENT_FLAG_active;
				deselected -> offset = EMPTY;
				deselected -> index = EMPTY;

				// not any more
				interface -> element_select = EMPTY;

				// update element state inside window
				lib_interface_draw_select( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) deselected );
			}
			
			// unmark current element if active
			if( interface -> element_select ) {
				// clear element state
				interface -> element_select -> flags &= ~LIB_INTERFACE_ELEMENT_FLAG_active;

				// update element state inside window
				lib_interface_draw_select( interface, interface -> element_select );
			}

			// setup selected element
			interface -> element_select = selected;
			interface -> element_select -> flags |= LIB_INTERFACE_ELEMENT_FLAG_active;

			// mark is on interface
			lib_interface_draw_select( interface, selected );

			// redraw window content
			interface -> descriptor -> flags |= STD_WINDOW_FLAG_flush;
		}

		// done
		return keyboard -> key;
	}

	// ignore below functions if no element is selected
	if( ! interface -> element_select ) return keyboard -> key;

	// SPACE key?
	if( keyboard -> key == STD_KEY_SPACE ) {
		// element type of
		switch( interface -> element_select -> type ) {
			case LIB_INTERFACE_ELEMENT_TYPE_menu: {
				// properties of element
				struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU *menu = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU *) interface -> element_select;

				// if event function exist, do it
				if( menu -> event ) menu -> event( menu );

				// done
				break;
			}

			case LIB_INTERFACE_ELEMENT_TYPE_checkbox: {
				// set selected semaphore
				if( interface -> element_select -> selected ) interface -> element_select -> selected = FALSE;
				else interface -> element_select -> selected = TRUE;

				// mark is on interface
				lib_interface_draw_select( interface, interface -> element_select );

				// redraw window content
				interface -> descriptor -> flags |= STD_WINDOW_FLAG_flush;

				// done
				break;
			}

			case LIB_INTERFACE_ELEMENT_TYPE_radio: {
				// first element properties
				struct LIB_INTERFACE_STRUCTURE_ELEMENT *element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) interface -> properties;

				// disable selection for any other element of that group
				while( element -> type != LIB_INTERFACE_ELEMENT_TYPE_null ) {
					// part of group?
					if( element == interface -> element_select ) element -> selected = TRUE;
					else if( element -> group == interface -> element_select -> group ) element -> selected = FALSE;
				
					// mark it on interface
					lib_interface_draw_select( interface, element );

					// next element properties
					element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) ((uint64_t) element + element -> size_byte);
				}

				// redraw window content
				interface -> descriptor -> flags |= STD_WINDOW_FLAG_flush;

				// done
				break;
			}
		}
	}

	// element type of
	if( interface -> element_select -> type == LIB_INTERFACE_ELEMENT_TYPE_input ) {
		// properties of element
		struct LIB_INTERFACE_STRUCTURE_ELEMENT_INPUT *input = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_INPUT *) interface -> element_select;

		// parse provided key
		input -> index = lib_input_not_interactive( input -> name, input -> name_length, input -> index, keyboard -> key, interface -> key_ctrl_semaphore );

		// calcualte offset, if required
		while( input -> offset > input -> index ) input -> offset--;
		uint64_t length = input -> index - input -> offset;
		while( lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, input -> name + input -> offset, length-- ) > (input -> input.width - 4) ) input -> offset++;

		// update content of element
		lib_interface_draw_select( interface, interface -> element_select );

		// redraw window content
		interface -> descriptor -> flags |= STD_WINDOW_FLAG_flush;
	}

	// element type of
	if( interface -> element_select -> type == LIB_INTERFACE_ELEMENT_TYPE_file ) {
		// updated
		uint8_t update = FALSE;

		// properties of element
		struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE *element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE *) interface -> element_select;

		// key: Arrow Down
		if( keyboard -> key == STD_KEY_ARROW_DOWN ) if( element -> selected < element -> limit - 1 ) { element -> selected++; update = TRUE; }

		// key: Arrow Up
		if( keyboard -> key == STD_KEY_ARROW_UP ) if( element -> selected ) { element -> selected--; update = TRUE; }

		// redraw window content (if required)
		if( update ) {
			element -> file.flags |= LIB_INTERFACE_ELEMENT_FLAG_flush;
	
			// update content of element
			lib_interface_draw_select( interface, interface -> element_select );
	
			interface -> descriptor -> flags |= STD_WINDOW_FLAG_flush;
		}
	}

	// return parsed key
	return keyboard -> key;
}

void lib_interface_active_or_hover( struct LIB_INTERFACE_STRUCTURE *interface ) {
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

		// control element?
		if( properties -> type == LIB_INTERFACE_ELEMENT_TYPE_control_close || properties -> type == LIB_INTERFACE_ELEMENT_TYPE_control_maximize || properties -> type == LIB_INTERFACE_ELEMENT_TYPE_control_minimize ) {
			// cursor overlaps this element? (check only if object is located under cursor)
			if( interface -> descriptor -> x >= interface -> width - (LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel + (properties -> x * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel)) && interface -> descriptor -> x < ((interface -> width - (LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel + (properties -> x * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel)))) + properties -> width && interface -> descriptor -> y >= properties -> y && interface -> descriptor -> y < properties -> y + properties -> height )
				// mark as hovered
				properties -> flags |= LIB_INTERFACE_ELEMENT_FLAG_hover;
			else
				// mark as not hovered
				properties -> flags &= ~LIB_INTERFACE_ELEMENT_FLAG_hover;
		} else
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
			lib_interface_draw_select( interface, properties );

			// update window content on screen
			interface -> descriptor -> flags |= STD_WINDOW_FLAG_flush;
		}

		// next element from list
		e += properties -> size_byte;
	}
}

uint32_t *lib_interface_icon( uint8_t *path, uint64_t length ) {
	// properties of file
	struct STD_STRUCTURE_FILE file = { EMPTY };

	if( (file.socket = std_file_open( path, length )) ) {
		// retrieve properties of file
		std_file( (struct STD_STRUCTURE_FILE *) &file );

		// assign area for file
		struct LIB_IMAGE_STRUCTURE_TGA *image = (struct LIB_IMAGE_STRUCTURE_TGA *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( file.byte ) >> STD_SHIFT_PAGE );

		// load file content
		std_file_read( (struct STD_STRUCTURE_FILE *) &file, (uint8_t *) image, file.byte );

		// copy image content to cursor object
		uint32_t *icon = (uint32_t *) malloc( image -> width * image -> height * STD_VIDEO_DEPTH_byte );
		lib_image_tga_parse( (uint8_t *) image, icon, file.byte );

		// release file content
		std_memory_release( (uintptr_t) image, MACRO_PAGE_ALIGN_UP( file.byte ) >> STD_SHIFT_PAGE );

		// close file
		std_file_close( file.socket );

		// done
		return icon;
	}

	// cannot locate specified file
	return EMPTY;
}

void lib_interface_name( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// window name set?
	if( ! interface -> name_length ) return;	// no

	// synchronize header name with window
	interface -> descriptor -> name_length = interface -> name_length;
	for( uint8_t i = 0; i < interface -> name_length; i++ ) interface -> descriptor -> name[ i ] = interface -> name[ i ];

	// draw new header name
	lib_interface_name_rewrite( interface );

	// inform Window Manager about new window name
	interface -> descriptor -> flags |= STD_WINDOW_FLAG_name;
}

void lib_interface_name_rewrite( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// clear window header with default background
	uint32_t *pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR ));
	for( uint16_t y = TRUE; y < LIB_INTERFACE_HEADER_HEIGHT_pixel; y++ )
		for( uint16_t x = TRUE; x < interface -> width - (1 + (interface -> controls * LIB_INTERFACE_HEADER_HEIGHT_pixel)); x++ )
			// draw pixel
			pixel[ (y * interface -> width) + x ] = LIB_INTERFACE_COLOR_background;

	// limit name length to header width
	uint8_t *string = (uint8_t *) calloc( interface -> name_length + 1);
	for( uint64_t i = 0; i < interface -> name_length; i++ ) string[ i ] = interface -> name[ i ];
	uint64_t limit = lib_interface_string( LIB_FONT_FAMILY_ROBOTO, string, interface -> name_length, interface -> width - (interface -> controls * LIB_INTERFACE_HEADER_HEIGHT_pixel) );

	// default border color
	uint32_t color = 0xFFFFFFFF;

	// change border of window if not active
	if( ! (interface -> descriptor -> flags & STD_WINDOW_FLAG_active) ) color = 0xFF808080;

	// print new header
	lib_font( LIB_FONT_FAMILY_ROBOTO, string, limit, color, pixel + (5 * interface -> width) + 5, interface -> width, LIB_FONT_ALIGN_left );
	free( string );
}

uint64_t lib_interface_string( uint8_t font_family, uint8_t *string, uint64_t limit, uint64_t pixel ) {
	// remember original value
	uint64_t new_limit = limit;

	// set max length of string
	while( lib_font_length_string( font_family, string, new_limit ) > pixel ) if( ! --new_limit ) break;

	// replace last 3 chars with triplet (if shrinked)
	if( new_limit < limit ) for( uint64_t i = new_limit; i && i > (new_limit - 3); i-- ) string[ i - 1 ] = STD_ASCII_DOT;

	// new string limit
	return new_limit;
}

uint8_t lib_interface_window( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// obtain information about kernel framebuffer
	struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER kernel_framebuffer;
	std_framebuffer( (struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER *) &kernel_framebuffer );

	// remember Window Manager PID
	int64_t wm_pid = kernel_framebuffer.pid;

	// allocate gui data container
	uint8_t wm_data_request[ STD_IPC_SIZE_byte ] = { EMPTY };
		// allocate gui data container
	uint8_t wm_data_answer[ STD_IPC_SIZE_byte ] = { EMPTY };

	// prepeare new window request
	struct STD_STRUCTURE_IPC_WINDOW *request = (struct STD_STRUCTURE_IPC_WINDOW *) &wm_data_request;
	struct STD_STRUCTURE_IPC_WINDOW_DESCRIPTOR *answer = (struct STD_STRUCTURE_IPC_WINDOW_DESCRIPTOR *) &wm_data_answer;

	//----------------------------------------------------------------------

	// window properties
	request -> ipc.type = STD_IPC_TYPE_event;
	request -> width = interface -> width;
	request -> height = interface -> height;

	// center window?
	if( interface -> x == STD_MAX_unsigned && interface -> y == STD_MAX_unsigned ) {
		// yes
		request -> x = (kernel_framebuffer.width_pixel >> STD_SHIFT_2) - (interface -> width >> STD_SHIFT_2);
		request -> y = ((kernel_framebuffer.height_pixel - LIB_INTERFACE_HEADER_HEIGHT_pixel) >> STD_SHIFT_2) - (interface -> height >> STD_SHIFT_2);
	} else {
		// no
		request -> x = interface -> x;
		request -> y = interface -> y;
	}

	// send request to Window Manager
	std_ipc_send( wm_pid, (uint8_t *) request );

	// wait for answer
	uint64_t timeout = std_microtime() + 32768;	// TODO, HPET, RTC...
	while( (! std_ipc_receive( (uint8_t *) answer ) || answer -> ipc.type != STD_IPC_TYPE_event) && timeout > std_microtime() );

	// window assigned?
	if( ! answer -> descriptor ) {
		// show error
		print( "Window Manager denied request." );

		// failed
		return FALSE;
	}

	// properties of console window
	interface -> descriptor = (struct STD_STRUCTURE_WINDOW_DESCRIPTOR *) answer -> descriptor;

	// clear window content
	lib_interface_clear( interface );

	// show window name in header if set
	lib_interface_name( interface );

	// done
	return TRUE;
}

struct LIB_INTERFACE_STRUCTURE_SELECT lib_interface_select( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// start from first element
	struct LIB_INTERFACE_STRUCTURE_ELEMENT *element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) interface -> properties;

	// init pointers
	struct LIB_INTERFACE_STRUCTURE_SELECT	select = { EMPTY };

	// search for previous
	while( element -> type ) {
		// previos already selected?
		if( element == interface -> element_select && select.previous ) break;	// yes

		// allowed element type?
		if( element -> type == LIB_INTERFACE_ELEMENT_TYPE_menu || element -> type == LIB_INTERFACE_ELEMENT_TYPE_button || element -> type == LIB_INTERFACE_ELEMENT_TYPE_checkbox || element -> type == LIB_INTERFACE_ELEMENT_TYPE_input || element -> type == LIB_INTERFACE_ELEMENT_TYPE_radio || element -> type == LIB_INTERFACE_ELEMENT_TYPE_file ) select.previous = element;	// yes

		// check next element from list
		element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) ((uintptr_t) element + element -> size_byte);
	}

	// start search from next of current active or beginning
	if( interface -> element_select ) select.next = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) ((uintptr_t) interface -> element_select + interface -> element_select -> size_byte);
	else select.next = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) interface -> properties;

	// search for next, allow only one round loop
	uint8_t loop = TRUE;
	while( TRUE ) {
		// end of list?
		if( ! select.next -> type ) {
			// last round loop?
			if( ! loop ) break;	// yes

			// start from beginning
			select.next = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) interface -> properties;

			// end
			loop = FALSE;
		}

		// allowed element type?
		if( select.next -> type == LIB_INTERFACE_ELEMENT_TYPE_menu || select.next -> type == LIB_INTERFACE_ELEMENT_TYPE_button || select.next -> type == LIB_INTERFACE_ELEMENT_TYPE_checkbox || select.next -> type == LIB_INTERFACE_ELEMENT_TYPE_input || select.next -> type == LIB_INTERFACE_ELEMENT_TYPE_radio || select.next -> type == LIB_INTERFACE_ELEMENT_TYPE_file ) break;	// found

		// check next element from list
		select.next = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) ((uintptr_t) select.next + select.next -> size_byte);
	}
	
	// return found element pointers
	return select;
}