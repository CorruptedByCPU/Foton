/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#ifndef	LIB_INTERFACE
		#include	"./interface.h"
	#endif
	#ifndef	LIB_WINDOW
		#include	"./window.h"
	#endif

uint8_t lib_interface( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// if dimensions aquired from JSON structure
	if( interface -> width && interface -> height ) {
		// create window
		if( ! (interface -> descriptor = lib_window( interface -> x, interface -> y, interface -> width, interface -> height )) ) return FALSE;

		// clear window content
		lib_interface_clear( interface );

		// show window name in header if set
		lib_interface_name( interface );

		// show interface elements
		// lib_interface_draw( interface );
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
	if( ! (interface -> descriptor -> flags & LIB_WINDOW_FLAG_active) ) { color = LIB_INTERFACE_BORDER_COLOR_inactive; color_shadow = LIB_INTERFACE_BORDER_COLOR_inactive_shadow; }

	// and point border
	uint32_t *pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR ));
	for( uint16_t y = 0; y < interface -> height; y++ )
		for( uint16_t x = 0; x < interface -> width; x++ ) {
			if( ! x || ! y ) { pixel[ (y * interface -> width) + x ] = color; }
			if( x == interface -> width - 1 || y == interface -> height - 1 ) pixel[ (y * interface -> width) + x ] = color_shadow;
		}
}

void lib_interface_clear( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// by default, color
	uint32_t background_color = LIB_INTERFACE_COLOR_background;
	if( interface -> background_color & STD_COLOR_mask ) background_color = interface -> background_color;	// change to choosen one

	// fill window with default background
	uint32_t *pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR ));
	for( int64_t i = 0; i < interface -> width * interface -> height; i++ )
		// draw pixel
		pixel[ i ] = background_color;

	// show default border
	lib_interface_border( interface );
}

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
		case LIB_INTERFACE_ELEMENT_TYPE_list: { lib_interface_element_file( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE *) element ); break; }
	}
}

void lib_interface_element_button( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *element ) {
	// limit string length to element width
	uint64_t limit = lib_interface_string( LIB_FONT_FAMILY_ROBOTO, element -> name, element -> name_length, element -> label_or_button.width );

	// compute absolute address of first pixel of element space
	uint32_t *pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR )) + (element -> label_or_button.y * interface -> width) + element -> label_or_button.x;

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
	uint32_t *pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR )) + (element -> checkbox.y * interface -> width) + element -> checkbox.x;

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
	uint32_t *pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR )) + interface -> width - LIB_INTERFACE_HEADER_HEIGHT_pixel - (element -> control.x * LIB_INTERFACE_HEADER_HEIGHT_pixel) - LIB_INTERFACE_BORDER_pixel;

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
	uint32_t *pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR )) + (element -> input.y * interface -> width) + element -> input.x;

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
	uint32_t *pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR )) + (element -> label_or_button.y * interface -> width) + element -> label_or_button.x;

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
	uint32_t *pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR )) + (element -> menu.y * interface -> width) + element -> menu.x;

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
	uint32_t *pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR )) + (element -> radio.y * interface -> width) + element -> radio.x;

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
	uint32_t *pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR )) + (element -> file.y * interface -> width) + element -> file.x;

	// dimensions of element
	uint16_t width = element -> file.width;
	uint16_t height = element -> file.height;
	if( width == (uint16_t) STD_MAX_unsigned ) width = interface -> width - (element -> file.x + LIB_INTERFACE_BORDER_pixel);
	if( height == (uint16_t) STD_MAX_unsigned ) height = interface -> height - (element -> file.y + LIB_INTERFACE_BORDER_pixel);

	// clean background
	uint32_t color = element -> color_default;
	for( uint16_t y = 0; y < height; y++ )
		for( uint16_t x = 0; x < width; x++ )
			pixel[ (y * interface -> width) + x ] = color;

	// expand/shrink previous area to meet actual requirements
	if( element -> pixel ) element -> pixel = realloc( element -> pixel, (element -> limit * ((LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel) * width)) << STD_VIDEO_DEPTH_shift );
	else element -> pixel = malloc( (element -> limit * ((LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel) * width)) << STD_VIDEO_DEPTH_shift );

	// draw all entries
	for( uint64_t e = 0; e < element -> limit; e++ ) {
		//--------------------------------------------------------------

		// set background color
		uint32_t color = element -> color_default;

		// modify background color for odd entries (strips)
		if( e % 2 ) color = element -> color_odd;

		// modify background color if
		if( element -> entry[ e ].flags & LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_active ) color = element -> color_selected;
		if( element -> entry[ e ].flags & LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_hover ) color += 0x00080808;
		if( element -> entry[ e ].flags & LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_select ) color += 0x00101010;

		// fill
		uint32_t *pixel_entry = (uint32_t *) element -> pixel + (e * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel * width);
		for( uint64_t y = 0; y < LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel; y++ )
			for( uint64_t x = 0; x < width; x++ )
				pixel_entry[ (y * width) + x ] = color;

		//--------------------------------------------------------------

		// vertical align
		pixel_entry += (((LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel - LIB_FONT_HEIGHT_pixel) >> STD_SHIFT_2) * width) + 4;

		// if icon provided
		if( element -> entry[ e ].icon )
			// load icon to element area
			for( uint16_t y = 0; y < 16; y++ )
				for( uint16_t x = 0; x < 16; x++ )
					pixel_entry[ (y * width) + x ] = lib_color_blend( pixel_entry[ (y * width) + x ], element -> entry[ e ].icon[ (y * 16) + x ] );

		//--------------------------------------------------------------

		// limit name length to entry width
		uint64_t limit;
		if( element -> entry[ e ].byte == STD_MAX_unsigned ) limit = lib_interface_string( LIB_FONT_FAMILY_ROBOTO, element -> entry[ e ].name, element -> entry[ e ].name_length, width - (16 + 2 + LIB_FONT_WIDTH_pixel + 4) );
		else limit = lib_interface_string( LIB_FONT_FAMILY_ROBOTO, element -> entry[ e ].name, element -> entry[ e ].name_length, width - (16 + 2 + LIB_FONT_WIDTH_pixel + lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) "0000.0 X", 8 ) + 4) );

		// name
		lib_font( LIB_FONT_FAMILY_ROBOTO, element -> entry[ e ].name, limit, LIB_INTERFACE_COLOR_foreground, pixel_entry + 4 + 16 + 2, width, LIB_FONT_ALIGN_left );

		//--------------------------------------------------------------

		// convert Bytes to human readable string if provided
		if( element -> entry[ e ].byte != STD_MAX_unsigned ) {
			// calculate unit type
			uint8_t unit = 0;	// Bytes by default
			while( pow( 1024, unit ) < element -> entry[ e ].byte ) unit++;

			// size string properties
			uint8_t *byte_string;
			uint64_t byte_limit;

			// convert Bytes to string (based on unit)
			if( unit > 1 ) {
				// float
				byte_string = lib_float_to_string( (double) element -> entry[ e ].byte / (double) pow( 1024, unit - 1 ), 1 );
				byte_limit = lib_string_length( byte_string );
			} else {
				// integer
				byte_string = calloc( 4 + 1 );
				byte_limit = lib_integer_to_string( element -> entry[ e ].byte, STD_NUMBER_SYSTEM_decimal, byte_string );
			}

			// add unit type
			byte_string = realloc( byte_string, byte_limit + 2 );
			byte_string[ byte_limit     ] = STD_ASCII_SPACE;
			byte_string[ byte_limit + 1 ] = lib_type_byte( element -> entry[ e ].byte );

			// size
			lib_font( LIB_FONT_FAMILY_ROBOTO_MONO, byte_string, byte_limit + 2, LIB_INTERFACE_COLOR_foreground, pixel_entry + width - 8, width, LIB_FONT_ALIGN_right );

			// release prepared size string
			free( byte_string );
		}
	}

	// if correction required
	if( element -> offset + height > element -> limit * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel ) {
		// apply, for overflow at bottom
		if( element -> limit * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel > height ) element -> offset = (element -> limit * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel) - height;
		// overflow at top
		else element -> offset = EMPTY;
	}

	// sync entries
	uint32_t *pixel_offset = (uint32_t *) element -> pixel + (element -> offset * width);
	for( uint64_t y = 0; y < (element -> limit * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel) && y < height; y++ )
		for( size_t x = 0; x < width; x++ )
			pixel[ (y * interface -> width) + x ] = pixel_offset[ (y * width) + x ];

	// reload window content
	interface -> descriptor -> flags |= LIB_WINDOW_FLAG_flush;
}

struct LIB_INTERFACE_STRUCTURE *lib_interface_event( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// incomming message
	uint8_t ipc_data[ STD_IPC_SIZE_byte ] = { EMPTY };

	// message properties
	struct STD_STRUCTURE_IPC_MOUSE *mouse = (struct STD_STRUCTURE_IPC_MOUSE *) &ipc_data;

	// // receive pending messages
	// if( std_ipc_receive_by_type( (uint8_t *) &ipc_data, STD_IPC_TYPE_mouse ) ) {
	// 	// pressed left mouse button?
	// 	if( mouse -> button == (uint8_t) STD_IPC_MOUSE_BUTTON_left ) lib_interface_event_handler_press( interface );

	// 	// released left mouse button?
	// 	if( mouse -> button == (uint8_t) ~STD_IPC_MOUSE_BUTTON_left ) lib_interface_event_handler_release( interface );
	// }

	// //--------------------------------------------------------------------------------
	// // "hover over elements"
	// //--------------------------------------------------------------------------------
	// lib_interface_active_or_hover( interface, mouse -> scroll );

	// acquired new window properties?
	if( interface -> descriptor -> flags & LIB_WINDOW_FLAG_properties ) {
		// disable flag
		interface -> descriptor -> flags ^= LIB_WINDOW_FLAG_properties;

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

		// create new window
		if( ! (new_interface -> descriptor = lib_window( new_interface -> x, new_interface -> y, new_interface -> width, new_interface -> height )) ) {
			// release new interface area
			free( new_interface );

			// nothing to do
			return EMPTY;
		}

		// clear window content
		lib_interface_clear( new_interface );

		// show window name in header if set
		for( uint8_t i = 0; i < interface -> descriptor -> name_length; i++ ) new_interface -> descriptor -> name[ new_interface -> descriptor -> name_length++ ] = interface -> descriptor -> name[ i ];
		lib_interface_name_rewrite( new_interface );

		// show interface elements
		// lib_interface_draw( new_interface );

		// copy required interface properties from old one
		//----------------------------------------------------------------------
		new_interface -> descriptor -> flags = interface -> descriptor -> flags;
		//----------------------------------------------------------------------

		// release old interface window
		interface -> descriptor -> flags |= LIB_WINDOW_FLAG_release;

		// release old interface area
		free( interface );

		// new window created
		return new_interface;
	}

	// active window change?
	if( (interface -> descriptor -> flags & LIB_WINDOW_FLAG_active) != interface -> active_semaphore ) {
		// remember current status
		interface -> active_semaphore = interface -> descriptor -> flags & LIB_WINDOW_FLAG_active;

		// update window border
		lib_interface_border( interface );

		// and header
		lib_interface_name_rewrite( interface );

		// update window content
		interface -> descriptor -> flags |= LIB_WINDOW_FLAG_flush;
	}

	// nothing to do
	return EMPTY;
}

void lib_interface_event_handler_press( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// check which element is under cursor position
	uint8_t *element = (uint8_t *) interface -> properties; uint64_t e = 0;
	while( element[ e ] != LIB_INTERFACE_ELEMENT_TYPE_null ) {
		// element properties
		struct LIB_INTERFACE_STRUCTURE_ELEMENT *properties = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) &element[ e ];

		// cursor overlaps this element? (check only if object is located under cursor)
		if( ((properties -> type == LIB_INTERFACE_ELEMENT_TYPE_control_close || properties -> type == LIB_INTERFACE_ELEMENT_TYPE_control_maximize || properties -> type == LIB_INTERFACE_ELEMENT_TYPE_control_minimize) && (interface -> descriptor -> x >= interface -> width - (LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel + (properties -> x * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel)) && interface -> descriptor -> x < ((interface -> width - (LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel + (properties -> x * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel)))) + properties -> width && interface -> descriptor -> y >= properties -> y && interface -> descriptor -> y < properties -> y + properties -> height)) || interface -> descriptor -> x >= properties -> x && interface -> descriptor -> x < properties -> x + properties -> width && interface -> descriptor -> y >= properties -> y && interface -> descriptor -> y < properties -> y + properties -> height ) {
			// new selected element
			interface -> element_select = properties;

			// execute event of element
			switch( properties -> type ) {
				case LIB_INTERFACE_ELEMENT_TYPE_list: {
					// properties of element
					struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE *element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE *) properties;

					// no entries?
					if( ! element -> limit ) break;	// nothing to do

					// calculate entry id
					uint64_t entry_id = ((element -> offset + (interface -> descriptor -> y - properties -> y)) / LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel);

					// outside of available entries?
					if( entry_id >= element -> limit ) break;	// ignore

					// remove select flag from any entry
					for( uint64_t i = 0; i < element -> limit; i++ ) element -> entry[ i ].flags &= ~LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_select;

					// set active entry
					if( interface -> key_ctrl_semaphore && ! (element -> flags & LIB_INTERFACE_ELEMENT_LIST_FLAG_individual) )
						// change active flag strictly for this entry
						element -> entry[ entry_id ].flags ^= LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_active | LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_select;
					else {
						// remove active flag from all entries
						for( uint64_t i = 0; i < element -> limit; i++ ) element -> entry[ i ].flags &= ~(LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_active | LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_select );

						// set active flag
						element -> entry[ entry_id ].flags |= LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_active | LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_select;
					}

					// dimensions of element
					uint16_t height = element -> file.height;
					if( height == (uint16_t) STD_MAX_unsigned ) height = interface -> height - (element -> file.y + LIB_INTERFACE_BORDER_pixel);

					// if newly active element is only partly visible, scroll a little bit
					if( (entry_id + 1) * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel >= element -> offset + height ) element -> offset = ((entry_id + 1) * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel) - height;
					else if( entry_id * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel < element -> offset ) element -> offset = entry_id * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel;

					// double-click?
					if( std_microtime() - element -> microtime < LIB_INTERFACE_LATENCY_microtime ) {
						// active flag already set?
						if( element -> entry[ entry_id ].flags & LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_active )
							// add run flag to entry
							element -> entry[ entry_id ].flags |= LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_run;
					} else if( element -> flags & LIB_INTERFACE_ELEMENT_LIST_FLAG_single_click )
						// add run flag to entry
						element -> entry[ entry_id ].flags |= LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_run;

					// preserve current microtime
					element -> microtime = std_microtime();

					// mark is on interface
					lib_interface_draw_select( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) properties );

					// redraw window content
					interface -> descriptor -> flags |= LIB_WINDOW_FLAG_flush;

					// done
					break;
				}
			}
		}

		// next element from list
		e += properties -> size_byte;
	}
}

void lib_interface_event_handler_release( struct LIB_INTERFACE_STRUCTURE *interface ) {
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
						interface -> descriptor -> flags |= LIB_WINDOW_FLAG_properties;
					} else {
						// preserve current window properties
						interface -> previous_x		= interface -> x;
						interface -> previous_y		= interface -> y;
						interface -> previous_width	= interface -> width;
						interface -> previous_height	= interface -> height;

						// maximize window
						interface -> descriptor -> flags |= LIB_WINDOW_FLAG_maximize;
					}

					// done
					break;
				}

				case LIB_INTERFACE_ELEMENT_TYPE_control_minimize: {
					// minimize window
					interface -> descriptor -> flags |= LIB_WINDOW_FLAG_minimize;

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
					interface -> descriptor -> flags |= LIB_WINDOW_FLAG_flush;

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
					interface -> descriptor -> flags |= LIB_WINDOW_FLAG_flush;

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
					interface -> descriptor -> flags |= LIB_WINDOW_FLAG_flush;

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
	if( interface -> key_alt_semaphore ) return keyboard -> key;

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
			interface -> descriptor -> flags |= LIB_WINDOW_FLAG_flush;
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
				interface -> descriptor -> flags |= LIB_WINDOW_FLAG_flush;

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
				interface -> descriptor -> flags |= LIB_WINDOW_FLAG_flush;

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
		interface -> descriptor -> flags |= LIB_WINDOW_FLAG_flush;
	}

	// element type of
	if( interface -> element_select -> type == LIB_INTERFACE_ELEMENT_TYPE_list ) {
		// apply modifications?
		uint8_t sync = FALSE;

		// properties of element
		struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE *element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE *) interface -> element_select;

		// find select flag
		uint64_t i = element -> limit;
		for( ; i > 0; --i ) if( element -> entry[ i ].flags & LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_select ) break;

		// key: Arrow Up/Down
		if( keyboard -> key == STD_KEY_ARROW_DOWN || keyboard ->key == STD_KEY_ARROW_UP ) {
			// remove hover flag from current entry
			element -> entry[ i ].flags &= ~LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_select;

			// next entry
			if( keyboard -> key == STD_KEY_ARROW_UP && i ) i--;

			// previous entry
			if( keyboard -> key == STD_KEY_ARROW_DOWN && i < element -> limit - 1 ) i++;

			// set hover flag strictly for this entry
			element -> entry[ i ].flags |= LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_select;

			// refresh
			sync = TRUE;
		}

		// key: BACKSPACE
		if( keyboard -> key == STD_KEY_BACKSPACE ) {
			// deselect entry
			element -> offset = EMPTY;

			// add run flag to first entry
			element -> entry[ FALSE ].flags |= LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_run;
		}

		// key: ENTER
		if( keyboard -> key == STD_KEY_ENTER ) {
			// set run flag on currently selected entry
			element -> entry[ i ].flags |= LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_run;
		}

		// key: SPACE
		if( keyboard -> key == STD_KEY_SPACE && interface -> key_ctrl_semaphore && ! (element -> flags & LIB_INTERFACE_ELEMENT_LIST_FLAG_individual) ) {
			// change active flag on currently selected entry
			element -> entry[ i ].flags ^= LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_active;

			// refresh
			sync = TRUE;
		}

		// dimensions of element
		uint16_t height = element -> file.height;
		if( height == (uint16_t) STD_MAX_unsigned ) height = interface -> height - (element -> file.y + LIB_INTERFACE_BORDER_pixel);

		// which part of area should we see
		if( (i + 1) * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel >= element -> offset + height ) element -> offset = ((i + 1) * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel) - height;
		else if( i * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel < element -> offset ) element -> offset = i * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel;

		// redraw window content (if required)
		if( sync ) {
			// update content of element
			lib_interface_draw_select( interface, interface -> element_select );

			// synchronize window content
			interface -> descriptor -> flags |= LIB_WINDOW_FLAG_flush;
		}
	}

	// return parsed key
	return keyboard -> key;
}

void lib_interface_active_or_hover( struct LIB_INTERFACE_STRUCTURE *interface, int16_t scroll ) {
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
		uint8_t update = FALSE;

		// control element?
		if( properties -> type == LIB_INTERFACE_ELEMENT_TYPE_control_close || properties -> type == LIB_INTERFACE_ELEMENT_TYPE_control_maximize || properties -> type == LIB_INTERFACE_ELEMENT_TYPE_control_minimize ) {
			// cursor overlaps this element? (check only if object is located under cursor)
			if( interface -> descriptor -> x >= interface -> width - (LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel + (properties -> x * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel)) && interface -> descriptor -> x < ((interface -> width - (LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel + (properties -> x * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel)))) + properties -> width && interface -> descriptor -> y >= properties -> y && interface -> descriptor -> y < properties -> y + properties -> height )
				// mark as hovered
				properties -> flags |= LIB_INTERFACE_ELEMENT_FLAG_hover;
			else
				// mark as not hovered
				properties -> flags &= ~LIB_INTERFACE_ELEMENT_FLAG_hover;
		} else {
			// dimensions of element
			uint16_t width = properties -> width;
			uint16_t height = properties -> height;
			if( width == (uint16_t) STD_MAX_unsigned ) width = interface -> width - (properties -> x + LIB_INTERFACE_BORDER_pixel);
			if( height == (uint16_t) STD_MAX_unsigned ) height = interface -> height - (properties -> y + LIB_INTERFACE_BORDER_pixel);
			
			// cursor overlaps this element? (check only if object is located under cursor)
			if( interface -> descriptor -> x >= properties -> x && interface -> descriptor -> x < properties -> x + width && interface -> descriptor -> y >= properties -> y && interface -> descriptor -> y < properties -> y + height ) {
				// element type of
				switch( properties -> type ) {
					case LIB_INTERFACE_ELEMENT_TYPE_list: {
						// properties of element
						struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE *element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE *) properties;

						// no entries?
						if( ! element -> limit ) break;	// nothing to do

						// scroll movement?
						if( scroll ) {
							// down
							if( scroll > 0 ) {
								// pixel by pixel
								if( (element -> offset + height + (scroll * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel)) < (element -> limit * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel) )
									element -> offset += scroll * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel;
								else if( (element -> limit * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel) > height )
									element -> offset = (element -> limit * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel) - height;
							}

							// up
							if( scroll < 0 ) {
								scroll = ~scroll + 1;
								if( element -> offset > (scroll * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel) )
									element -> offset -= scroll * LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel;
								else element -> offset = EMPTY;
							}
							
							// refresh
							update = TRUE;

							// done
							break;
						}

						// choose new hovered entry
						if( ! (element -> entry[ ((element -> offset + (interface -> descriptor -> y - properties -> y)) / LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel) ].flags & LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_hover) ) {
							// remove hover flag from all entries
							for( uint64_t i = 0; i < element -> limit; i++ ) element -> entry[ i ].flags &= ~LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_hover;

							// set hover flag strictly for this entry
							element -> entry[ ((element -> offset + (interface -> descriptor -> y - properties -> y)) / LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel) ].flags |= LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_hover;

							// refresh
							update = TRUE;
						}

						// done
						break;
					}

					default: {
						// mark as hovered
						properties -> flags |= LIB_INTERFACE_ELEMENT_FLAG_hover;
					}
				}
			} else
				// mark as not hovered
				properties -> flags &= ~LIB_INTERFACE_ELEMENT_FLAG_hover;
		}

		// if "event" changed or requested update
		if( update || properties -> flags != previous ) {
			// redraw element inside object
			lib_interface_draw_select( interface, properties );

			// update window content on screen
			interface -> descriptor -> flags |= LIB_WINDOW_FLAG_flush;
		}

		// next element from list
		e += properties -> size_byte;
	}
}

uint32_t *lib_interface_icon( uint8_t *path ) {
	// file properties
	FILE *file = EMPTY;

	// file exist?
	if( (file = fopen( path, EMPTY )) ) {
		// assign area for file
		struct LIB_IMAGE_STRUCTURE_TGA *image = (struct LIB_IMAGE_STRUCTURE_TGA *) malloc( MACRO_PAGE_ALIGN_UP( file -> byte ) >> STD_SHIFT_PAGE );

		// load file content
		fread( file, (uint8_t *) image, file -> byte );

		// copy image content to cursor object
		uint32_t *icon = (uint32_t *) malloc( image -> width * image -> height * STD_VIDEO_DEPTH_byte );
		lib_image_tga_parse( (uint8_t *) image, icon, file -> byte );

		// release file content
		free( image );

		// close file
		fclose( file );

		// done
		return icon;
	}

	// cannot locate specified file
	return EMPTY;
}

void lib_interface_name( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// window name set?
	if( ! interface -> descriptor -> name_length ) return;	// no

	// draw new header name
	lib_interface_name_rewrite( interface );

	// inform Window Manager about new window name
	interface -> descriptor -> flags |= LIB_WINDOW_FLAG_name;
}

void lib_interface_name_rewrite( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// window name set?
	if( ! interface -> descriptor -> name_length ) return;	// no

	// clear window header with default background
	uint32_t *pixel = (uint32_t *) ((uintptr_t) interface -> descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR ));
	for( uint16_t y = TRUE; y < LIB_INTERFACE_HEADER_HEIGHT_pixel; y++ )
		for( uint16_t x = TRUE; x < interface -> width - (1 + (interface -> controls * LIB_INTERFACE_HEADER_HEIGHT_pixel)); x++ )
			// draw pixel
			pixel[ (y * interface -> width) + x ] = LIB_INTERFACE_COLOR_background;

	// limit name length to header width
	uint64_t limit = lib_interface_string( LIB_FONT_FAMILY_ROBOTO, interface -> descriptor -> name, interface -> descriptor -> name_length, interface -> width - (interface -> controls * LIB_INTERFACE_HEADER_HEIGHT_pixel) );

	// default border color
	uint32_t color = 0xFFFFFFFF;

	// change border of window if not active
	if( ! (interface -> descriptor -> flags & LIB_WINDOW_FLAG_active) ) color = 0xFF808080;

	// print new header
	lib_font( LIB_FONT_FAMILY_ROBOTO, interface -> descriptor -> name, limit, color, pixel + (5 * interface -> width) + 5, interface -> width, LIB_FONT_ALIGN_left );
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

struct LIB_INTERFACE_STRUCTURE_SELECT lib_interface_select( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// start from first element
	struct LIB_INTERFACE_STRUCTURE_ELEMENT *element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) interface -> properties;

	// init pointers
	struct LIB_INTERFACE_STRUCTURE_SELECT select = { EMPTY };

	// search for previous
	while( element -> type ) {
		// previos already selected?
		if( element == interface -> element_select && select.previous ) break;	// yes

		// allowed element type?
		if( element -> type == LIB_INTERFACE_ELEMENT_TYPE_menu || element -> type == LIB_INTERFACE_ELEMENT_TYPE_button || element -> type == LIB_INTERFACE_ELEMENT_TYPE_checkbox || element -> type == LIB_INTERFACE_ELEMENT_TYPE_input || element -> type == LIB_INTERFACE_ELEMENT_TYPE_radio || element -> type == LIB_INTERFACE_ELEMENT_TYPE_list ) select.previous = element;	// yes

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
		if( select.next -> type == LIB_INTERFACE_ELEMENT_TYPE_menu || select.next -> type == LIB_INTERFACE_ELEMENT_TYPE_button || select.next -> type == LIB_INTERFACE_ELEMENT_TYPE_checkbox || select.next -> type == LIB_INTERFACE_ELEMENT_TYPE_input || select.next -> type == LIB_INTERFACE_ELEMENT_TYPE_radio || select.next -> type == LIB_INTERFACE_ELEMENT_TYPE_list ) break;	// found

		// check next element from list
		select.next = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) ((uintptr_t) select.next + select.next -> size_byte);
	}
	
	// return found element pointers
	return select;
}
