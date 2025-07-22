/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void palette_ui( void ) {
	// set pointer to canvas location
	uint32_t *canvas = (uint32_t *) ui -> window -> pixel + ((LIB_UI_HEADER_HEIGHT) * ui -> window -> current_width) + LIB_UI_MARGIN_DEFAULT;

	// fill canvas with gradient based on HSV
	for( uint64_t y = 0; y < CANVAS_HEIGHT; y++ ) for( uint64_t x = 0; x < CANVAS_WIDTH; x++ ) canvas[ (y * ui -> window -> current_width) + x ] = lib_color_hsv( h, x / (double) CANVAS_WIDTH, (CANVAS_HEIGHT - y) / (double) CANVAS_HEIGHT );

	// draw a cross at current coordinates for canvas
	for( uint64_t x = 0; x < CANVAS_WIDTH; x++ ) canvas[ (canvas_y_axis * ui -> window -> current_width) + x ] = STD_COLOR_mask | ~canvas[ (canvas_y_axis * ui -> window -> current_width) + x ];
	for( uint64_t y = 0; y < CANVAS_HEIGHT; y++ ) canvas[ (y * ui -> window -> current_width) + canvas_x_axis ] = STD_COLOR_mask | ~canvas[ (y * ui -> window -> current_width) + canvas_x_axis ];

	//---------------------------------------------------------------------

	// set pointer to spectrum location
	uint32_t *spectrum = canvas + CANVAS_WIDTH + LIB_UI_PADDING_DEFAULT;

	// fill spectrum with gradient
	for( uint64_t y = 0; y < CANVAS_HEIGHT; y++ ) for( uint64_t x = 0; x < SPECTRUM_WIDTH; x++ ) spectrum[ (y * ui -> window -> current_width) + x ] = lib_color_hsv( ((double) y / (double) CANVAS_HEIGHT) * 359.0f, 1.0f, 1.0f );

	// draw a lever at current coortinate for spectrum
	for( uint64_t x = 0; x < SPECTRUM_WIDTH; x++ ) spectrum[ (spectrum_y_axis * ui -> window -> current_width) + x ] = STD_COLOR_mask | ~spectrum[ (spectrum_y_axis * ui -> window -> current_width) + x ];

	//---------------------------------------------------------------------

	// set pointer to sample location
	uint32_t *sample = spectrum + SPECTRUM_WIDTH + LIB_UI_PADDING_DEFAULT;

	// current HSV color to RGB
	uint32_t color = lib_color_hsv( h, s, v );

	// fill sample with current color
	for( uint64_t y = 0; y < SAMPLE_HEIGHT; y++ ) for( uint64_t x = 0; x < SAMPLE_WIDTH; x++ ) sample[ (y * ui -> window -> current_width) + x ] = color;

	//---------------------------------------------------------------------

	// uint64_t length;

	// color = lib_color_hsv( h, s, v );

	// length = lib_integer_to_string( (uint8_t) (color >> 16), 10, (uint8_t *) &input );
	// input[ length ] = STD_ASCII_TERMINATOR;
	// lib_ui_update_input( ui, ui_id_input_r, (uint8_t *) &input );

	// length = lib_integer_to_string( (uint8_t) (color >> 8), 10, (uint8_t *) &input );
	// input[ length ] = STD_ASCII_TERMINATOR;
	// lib_ui_update_input( ui, ui_id_input_g, (uint8_t *) &input );

	// length = lib_integer_to_string( (uint8_t) color, 10, (uint8_t *) &input );
	// input[ length ] = STD_ASCII_TERMINATOR;
	// lib_ui_update_input( ui, ui_id_input_b, (uint8_t *) &input );

	// length = lib_integer_to_string( (uint64_t) h, 10, (uint8_t *) &input );
	// input[ length ] = STD_ASCII_TERMINATOR;
	// lib_ui_update_input( ui, ui_id_input_h, (uint8_t *) &input );

	// length = lib_integer_to_string( (uint64_t) (s * 100.0f), 10, (uint8_t *) &input );
	// input[ length ] = STD_ASCII_TERMINATOR;
	// lib_ui_update_input( ui, ui_id_input_s, (uint8_t *) &input );

	// length = lib_integer_to_string( (uint64_t) (v * 100.0f), 10, (uint8_t *) &input );
	// input[ length ] = STD_ASCII_TERMINATOR;
	// lib_ui_update_input( ui, ui_id_input_v, (uint8_t *) &input );

	// lib_integer_to_string( color, 16, (uint8_t *) &input );
	// input[ 8 ] = STD_ASCII_TERMINATOR;
	// lib_ui_update_input( ui, ui_id_input_rgb, (uint8_t *) &input[ 2 ] );

	lib_ui_flush( ui );
}
