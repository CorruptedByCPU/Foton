/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void palette_init( void ) {
	// initial canvas
	canvas_x_axis = CANVAS_WIDTH - (s * CANVAS_WIDTH);
	canvas_y_axis = CANVAS_HEIGHT - (v * CANVAS_HEIGHT);
	// and spectrum pointers
	spectrum_y_axis = (h / 360.0f) * CANVAS_HEIGHT;

	//---------------------------------------------------------------------

	// create Palette window
	// window = lib_window( -1, -1, WINDOW_WIDTH, WINDOW_HEIGHT );	// at default location
	window = lib_window( 129 + 606, 3, WINDOW_WIDTH, WINDOW_HEIGHT );	// DEBUG

	// name window
	lib_window_name( window, (uint8_t *) "Palette" );

	// initialize UI for that window
	ui = lib_ui( window );

	// add Palette icon
	ui -> icon = lib_image_scale( lib_icon_icon( (uint8_t *) "/var/share/media/icon/default/app/gcolor3.tga" ), 48, 48, 16, 16 ); for( uint64_t i = 0; i < 16 * 16; i++ ) window -> icon[ i ] = ui -> icon[ i ];

	//---------------------------------------------------------------------

	// create elements of UI

	// control buttons
	lib_ui_add_control( ui, LIB_UI_ELEMENT_CONTROL_TYPE_close );
	lib_ui_add_control( ui, LIB_UI_ELEMENT_CONTROL_TYPE_min );
	// window is not resizable

	// local coordinates
	uint64_t x, y;

	// column 2 location
	x = LIB_UI_MARGIN_DEFAULT + CANVAS_WIDTH + LIB_UI_PADDING_DEFAULT + SPECTRUM_WIDTH + LIB_UI_PADDING_DEFAULT;
	y = LIB_UI_HEADER_HEIGHT + SAMPLE_HEIGHT + LIB_UI_PADDING_DEFAULT;

	// label: Red
	lib_ui_add_label( ui, x, y, lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) "R:", 2 ), (uint8_t *) "R:", EMPTY, EMPTY );

	// label: Green
	y += LIB_FONT_HEIGHT_pixel + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_label( ui, x, y, lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) "G:", 2 ), (uint8_t *) "G:", EMPTY, EMPTY );

	// label: Blue
	y += LIB_FONT_HEIGHT_pixel + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_label( ui, x, y, lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) "B:", 2 ), (uint8_t *) "B:", EMPTY, EMPTY );

	// move to next column of current row
	x += (LIB_FONT_WIDTH_pixel << STD_SHIFT_2) + LIB_UI_PADDING_DEFAULT;
	y = LIB_UI_HEADER_HEIGHT + SAMPLE_HEIGHT + LIB_UI_PADDING_DEFAULT;

	// input: Red
	ui_id_input_r = lib_ui_add_input( ui, x, y, 32, EMPTY, LIB_UI_ELEMENT_FLAG_disabled, EMPTY );

	// input: Green
	y += LIB_FONT_HEIGHT_pixel + LIB_UI_PADDING_DEFAULT;
	ui_id_input_g = lib_ui_add_input( ui, x, y, 32, EMPTY, LIB_UI_ELEMENT_FLAG_disabled, EMPTY );

	// input: Blue
	y += LIB_FONT_HEIGHT_pixel + LIB_UI_PADDING_DEFAULT;
	ui_id_input_b = lib_ui_add_input( ui, x, y, 32, EMPTY, LIB_UI_ELEMENT_FLAG_disabled, EMPTY );

	// move to next column of current row
	x += 32 + LIB_UI_PADDING_DEFAULT;
	y = LIB_UI_HEADER_HEIGHT + SAMPLE_HEIGHT + LIB_UI_PADDING_DEFAULT;

	// label: Hue
	lib_ui_add_label( ui, x, y, lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) "H:", 2 ), (uint8_t *) "H:", EMPTY, EMPTY );

	// label: Saturation
	y += LIB_FONT_HEIGHT_pixel + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_label( ui, x, y, lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) "S:", 2 ), (uint8_t *) "S:", EMPTY, EMPTY );

	// label: Value
	y += LIB_FONT_HEIGHT_pixel + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_label( ui, x, y, lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) "V:", 2 ), (uint8_t *) "V:", EMPTY, EMPTY );

	// move to next column of current row
	x += (LIB_FONT_WIDTH_pixel << STD_SHIFT_2) + LIB_UI_PADDING_DEFAULT;
	y = LIB_UI_HEADER_HEIGHT + SAMPLE_HEIGHT + LIB_UI_PADDING_DEFAULT;

	// input: Hue
	ui_id_input_h = lib_ui_add_input( ui, x, y, 32, EMPTY, LIB_UI_ELEMENT_FLAG_disabled, EMPTY );

	// input: Saturation
	y += LIB_FONT_HEIGHT_pixel + LIB_UI_PADDING_DEFAULT;
	ui_id_input_s = lib_ui_add_input( ui, x, y, 32, EMPTY, LIB_UI_ELEMENT_FLAG_disabled, EMPTY );

	// input: Value
	y += LIB_FONT_HEIGHT_pixel + LIB_UI_PADDING_DEFAULT;
	ui_id_input_v = lib_ui_add_input( ui, x, y, 32, EMPTY, LIB_UI_ELEMENT_FLAG_disabled, EMPTY );

	// ??????????? :D
	y += LIB_FONT_HEIGHT_pixel + LIB_UI_PADDING_DEFAULT;

	// move to next row of current column (centered)
	x = LIB_UI_MARGIN_DEFAULT + CANVAS_WIDTH + LIB_UI_PADDING_DEFAULT + SPECTRUM_WIDTH + LIB_UI_PADDING_DEFAULT + ((WINDOW_WIDTH - (LIB_UI_MARGIN_DEFAULT + CANVAS_WIDTH + LIB_UI_PADDING_DEFAULT + SPECTRUM_WIDTH + LIB_UI_PADDING_DEFAULT)) >> STD_SHIFT_2) - LIB_FONT_WIDTH_pixel - 32;
	y = WINDOW_HEIGHT - LIB_UI_MARGIN_DEFAULT - LIB_UI_ELEMENT_INPUT_height - ((WINDOW_HEIGHT - y) >> STD_SHIFT_2) + (LIB_UI_ELEMENT_INPUT_height >> STD_SHIFT_2);

	// label: #
	lib_ui_add_label( ui, x, y, lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) "#", 1 ), (uint8_t *) "#", EMPTY, EMPTY );

	// mobe to next column of current row
	x += lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) "#", 1 ) + LIB_UI_PADDING_DEFAULT;

	// input: #
	ui_id_input_rgb = lib_ui_add_input( ui, x, y, 64, EMPTY, LIB_UI_ELEMENT_FLAG_disabled, EMPTY );

	// update canvas/sample and input fields with current values
	palette_ui();

	// application ready
	ui -> window -> flags |= LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_icon | LIB_WINDOW_FLAG_flush;
}
