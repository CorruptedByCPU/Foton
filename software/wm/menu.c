/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

int64_t wm_menu( void ) {
	// open menu file
	FILE *file = fopen( (uint8_t *) "/system/etc/menu" );

	// menu file doesn't exist?
	if( ! file ) {
		// execute console application
		std_exec( (uint8_t *) "console", 7, EMPTY );

		// done
		return EMPTY;
	}

	// allocate area for file content
	uint8_t *file_content = (uint8_t *) calloc( file -> byte + 1 );
	if( ! file_content ) {
		// cannot allocate enough memory
		fclose( file );

		// done
		return EMPTY;
	}

	// load file content into memory
	fread( file, file_content, file -> byte );

	// close menu file, we doesn't care anymore
	fclose( file );

	// find widest menu entry, and calculate height of menu window
	uint64_t width_pixel = 0;
	uint64_t height_pixel = 0;

	// until end of menu file
	uint8_t *search = file_content; while( *search ) {
		// line length
		uint64_t line_length = lib_string_length_line( search );

		// entry width in characters
		uint64_t width_current = lib_font_length_string( LIB_FONT_FAMILY_ROBOTO, search, lib_string_word_end( search, line_length, STD_ASCII_COMMA ) );

		// wider than others?
		if( width_current > width_pixel ) width_pixel = width_current;

		// menu entry
		height_pixel += LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel;

		// move to next line
		search += line_length + 1;
	}

	// test
	width_pixel += 4 + 16 + 2 + 0 + 6;	// 0 > label position of menu element

	// prepare empty menu window
	interface_menu = lib_interface_create( width_pixel, height_pixel, (uint8_t *) "Menu" );

	// create menu object
	wm_object_menu = wm_object_create( -LIB_INTERFACE_SHADOW_length, wm_object_taskbar -> y - (interface_menu -> height - LIB_INTERFACE_SHADOW_length), interface_menu -> width, interface_menu -> height );

	// update interface window descriptor
	interface_menu -> descriptor = wm_object_menu -> descriptor;

	// set shadow length
	interface_menu -> descriptor -> offset = LIB_INTERFACE_SHADOW_length;

	// add elements to interface from menu file
	uint64_t properties_length = 0; uint64_t elements = 0; uint64_t properties_index = 0;
	while( *file_content ) {
		// line length
		uint64_t line_length = lib_string_length_line( file_content );

		// ignore entries without command
		if( ! lib_string_count( file_content, line_length, STD_ASCII_COMMA ) ) {
			// move to next line
			file_content += line_length + 1;

			// next
			continue;
		}

		// alloc space for element
		properties_length += sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU );
		interface_menu -> properties = (uint8_t *) realloc( interface_menu -> properties, properties_length );

		//-------------------------------------------------------------

		// element structure position
		struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU *element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU *) &interface_menu -> properties[ properties_index ];

		// default properties of label and button
		element -> menu.type = LIB_INTERFACE_ELEMENT_TYPE_menu;
		element -> menu.flags = EMPTY;
		element -> menu.size_byte = sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU );

		// position and size
		element -> menu.x = interface_menu -> descriptor -> offset;
		element -> menu.y = LIB_INTERFACE_HEADER_HEIGHT_pixel + (elements * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel) + interface_menu -> descriptor -> offset;
		element -> menu.width = width_pixel;
		element -> menu.height = LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel;

		// length if proper
		element -> name_length = lib_string_word_end( file_content, line_length, STD_ASCII_COMMA );

		// name overflow?
		if( element -> name_length > LIB_INTERFACE_GLOBAL_NAME_limit )
			// limit name length
			element -> name_length = LIB_INTERFACE_GLOBAL_NAME_limit;


		// alloc area for element name
		uint8_t *name_target = (uint8_t *) calloc( element -> name_length + 1 );

		// copy element name
		for( uint64_t i = 0; i < element -> name_length; i++ ) name_target[ i ] = file_content[ i ];

		// update element name pointer
		element -> name = name_target;

		//-------------------------------------------------------------

		// search for command properties
		uint8_t *string_command = file_content + element -> name_length + 1;
		uint64_t string_command_length = lib_string_word_end( string_command, lib_string_length_line( string_command ), STD_ASCII_COMMA );

		// icon available?
		if( lib_string_count( file_content, line_length, STD_ASCII_COMMA ) == 2 ) {
			//search for icon properties
			uint8_t *string_icon = string_command + string_command_length + 1;
			uint64_t string_icon_length = lib_string_word_end( string_icon, lib_string_length_line( string_icon ), STD_ASCII_COMMA ); string_icon[ string_icon_length ] = STD_ASCII_TERMINATOR;

			// properties of file
			FILE *icon_file;

			// properties of image
			struct LIB_IMAGE_TGA_STRUCTURE *icon_image = EMPTY;

			// retrieve information about module file
			if( (icon_file = fopen( (uint8_t *) string_icon )) ) {
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

		//-------------------------------------------------------------

		// move to next line
		file_content += line_length + 1;

		// amount of elements
		elements++;

		properties_index += element -> menu.size_byte;
	}

	// mark it as own
	wm_object_menu -> pid = wm_pid;

	// prepare shadow of window
	lib_interface_shadow( interface_menu );

	// clear window content
	lib_interface_clear( interface_menu );

	// show window name in header if set
	lib_interface_name( interface_menu );

	// show interface elements
	lib_interface_draw( interface_menu );

	// debug
	wm_object_menu -> descriptor -> flags |= STD_WINDOW_FLAG_unstable | STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;

	// release file content
	free( file_content );

	// main loop
	while( TRUE ) {
		// check incomming interface events
		lib_interface_event( interface_menu );
	}

	// dummy
	return EMPTY;
}
