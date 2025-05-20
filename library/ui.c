/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#include	"../library/font.h"
#include	"../library/string.h"
#include	"../library/ui.h"

void lib_ui_fill_rectangle( uint32_t *pixel, uint64_t scanline, uint8_t r, uint64_t width, uint64_t height, uint32_t color ) {
	for( uint64_t y = 0; y < height; y++ )
		for( uint64_t x = 0; x < width; x++ ) {
			// no round corners?
			if( ! r ) { pixel[ (y * scanline) + x ] = color; continue; }

			// inner content of rectangle?
			if( (x >= r && x < width - r) || (y >= r && y < height - r) ) { pixel[ (y * scanline) + x ] = color; continue; }

			// check if x,y is inside circle
			int64_t dx, dy;

			// left-top corner
			dx = x - r; dy = y - r;
			if( x < r && y < r && ((dx * dx) + (dy * dy) < (r * r)) ) { pixel[ (y * scanline) + x ] = color; continue; }

			// right-top corner
			dx = x - (width - r - 1); dy = y - r;
			if( x >= width - r && y < r && ((dx * dx) + (dy * dy) < (r * r)) ) { pixel[ (y * scanline) + x ] = color; continue; }

			// right-botom corner
			dx = x - r; dy = y - (height - r - 1);
			if( x < r && y >= height - r && ((dx * dx) + (dy * dy) < (r * r)) ) { pixel[ (y * scanline) + x ] = color; continue; }
			
			// left-bottom corner
			dx = x - (width - r - 1); dy = y - (height - r - 1);
			if( x >= width - r && y >= height - r && ((dx * dx) + (dy * dy) < (r * r)) ) { pixel[ (y * scanline) + x ] = color; continue; }
		}
}

struct LIB_UI_STRUCTURE *lib_ui( struct LIB_WINDOW_STRUCTURE *window ) {
	// initialize structure
	struct LIB_UI_STRUCTURE *ui = calloc( sizeof( struct LIB_UI_STRUCTURE ) );

	// preserve properties of already existing window
	ui -> window = window;

	// no elements by default
	ui -> limit_button = ui -> limit_checkbox = ui -> limit_label = ui -> limit_radio = EMPTY;

	// prepare area for ui elements
	ui -> button = (struct LIB_UI_STRUCTURE_ELEMENT_BUTTON **) malloc( ui -> limit_button );
	ui -> checkbox = (struct LIB_UI_STRUCTURE_ELEMENT_CHECKBOX **) malloc( ui -> limit_checkbox );
	ui -> label = (struct LIB_UI_STRUCTURE_ELEMENT_LABEL **) malloc( ui -> limit_label );
	ui -> radio = (struct LIB_UI_STRUCTURE_ELEMENT_RADIO **) malloc( ui -> limit_radio );

	// return ui properties
	return ui;
}

uint64_t lib_ui_add_button( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *name ) {
	ui -> button = (struct LIB_UI_STRUCTURE_ELEMENT_BUTTON **) realloc( ui -> button, sizeof( struct LIB_UI_STRUCTURE_ELEMENT_BUTTON ) * (ui -> limit_button + TRUE) );

	ui -> button[ ui -> limit_button ] = (struct LIB_UI_STRUCTURE_ELEMENT_BUTTON *) malloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_BUTTON ) );
	ui -> button[ ui -> limit_button ] -> standard.x = x;
	ui -> button[ ui -> limit_button ] -> standard.y = y;
	ui -> button[ ui -> limit_button ] -> standard.width = width;
	ui -> button[ ui -> limit_button ] -> standard.height = height;
	ui -> button[ ui -> limit_button ] -> name = (uint8_t *) malloc( lib_string_length( name ) );
	for( uint64_t i = 0; i <= lib_string_length( name ); i++ ) ui -> button[ ui -> limit_button ] -> name[ i ] = name[ i ];

	return ui -> limit_button++;
}

uint64_t lib_ui_add_checkbox( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint8_t *name ) {
	ui -> checkbox = (struct LIB_UI_STRUCTURE_ELEMENT_CHECKBOX **) realloc( ui -> checkbox, sizeof( struct LIB_UI_STRUCTURE_ELEMENT_CHECKBOX ) * (ui -> limit_checkbox + TRUE) );

	ui -> checkbox[ ui -> limit_checkbox ] = (struct LIB_UI_STRUCTURE_ELEMENT_CHECKBOX *) malloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_CHECKBOX ) );
	ui -> checkbox[ ui -> limit_checkbox ] -> standard.x = x;
	ui -> checkbox[ ui -> limit_checkbox ] -> standard.y = y;
	ui -> checkbox[ ui -> limit_checkbox ] -> standard.width = width;
	ui -> checkbox[ ui -> limit_checkbox ] -> standard.height = LIB_FONT_HEIGHT_pixel;
	ui -> checkbox[ ui -> limit_checkbox ] -> name = (uint8_t *) malloc( lib_string_length( name ) );
	for( uint64_t i = 0; i <= lib_string_length( name ); i++ ) ui -> checkbox[ ui -> limit_checkbox ] -> name[ i ] = name[ i ];
	ui -> checkbox[ ui -> limit_checkbox ] -> set = FALSE;

	return ui -> limit_checkbox++;
}

uint64_t lib_ui_add_radio( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint16_t width, uint8_t group, uint8_t *name ) {
	ui -> radio = (struct LIB_UI_STRUCTURE_ELEMENT_RADIO **) realloc( ui -> radio, sizeof( struct LIB_UI_STRUCTURE_ELEMENT_RADIO ) * (ui -> limit_radio + TRUE) );

	ui -> radio[ ui -> limit_radio ] = (struct LIB_UI_STRUCTURE_ELEMENT_RADIO *) malloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_RADIO ) );
	ui -> radio[ ui -> limit_radio ] -> standard.x = x;
	ui -> radio[ ui -> limit_radio ] -> standard.y = y;
	ui -> radio[ ui -> limit_radio ] -> standard.width = width;
	ui -> radio[ ui -> limit_radio ] -> standard.height = LIB_FONT_HEIGHT_pixel;
	ui -> radio[ ui -> limit_radio ] -> name = (uint8_t *) malloc( lib_string_length( name ) );
	for( uint64_t i = 0; i <= lib_string_length( name ); i++ ) ui -> radio[ ui -> limit_radio ] -> name[ i ] = name[ i ];
	ui -> radio[ ui -> limit_radio ] -> set = FALSE;
	ui -> radio[ ui -> limit_radio ] -> group = group;

	return ui -> limit_radio++;
}

uint64_t lib_ui_add_label( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint8_t *name ) {
	ui -> label = (struct LIB_UI_STRUCTURE_ELEMENT_LABEL **) realloc( ui -> label, sizeof( struct LIB_UI_STRUCTURE_ELEMENT_LABEL ) * (ui -> limit_label + TRUE) );

	ui -> label[ ui -> limit_label ] = (struct LIB_UI_STRUCTURE_ELEMENT_LABEL *) malloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_LABEL ) );
	ui -> label[ ui -> limit_label ] -> standard.x = x;
	ui -> label[ ui -> limit_label ] -> standard.y = y;
	ui -> label[ ui -> limit_label ] -> standard.width = (ui -> window -> width - x) - LIB_UI_MARGIN_DEFAULT;
	ui -> label[ ui -> limit_label ] -> standard.height = LIB_FONT_HEIGHT_pixel;
	ui -> label[ ui -> limit_label ] -> name = (uint8_t *) malloc( lib_string_length( name ) );
	for( uint64_t i = 0; i <= lib_string_length( name ); i++ ) ui -> label[ ui -> limit_label ] -> name[ i ] = name[ i ];

	return ui -> limit_label++;
}

void lib_ui_clean( struct LIB_UI_STRUCTURE *ui ) {
	lib_ui_fill_rectangle( ui -> window -> pixel, ui -> window -> width, LIB_UI_RADIUS_DEFAULT, ui -> window -> width, ui -> window -> height, LIB_UI_COLOR_BACKGROUND_DEFAULT );
}

void lib_ui_event( struct LIB_UI_STRUCTURE *ui ) {
	uint8_t flush = FALSE;

	for( uint64_t i = 0; i < ui -> limit_button; i++ ) {
		if( ui -> window -> x < ui -> button[ i ] -> standard.x || ui -> window -> x > (ui -> button[ i ] -> standard.x + ui -> button[ i ] -> standard.width) || ui -> window -> y < ui -> button[ i ] -> standard.y || (ui -> window -> y > ui -> button[ i ] -> standard.y + ui -> button[ i ] -> standard.height) ) {
			if( ! (ui -> button[ i ] -> standard.flag & LIB_UI_FLAG_hover) ) continue;
			ui -> button[ i ] -> standard.flag &= ~LIB_UI_FLAG_hover;
			lib_ui_show_button( ui, i );
			flush = TRUE;
			continue;
		}

		if( ui -> button[ i ] -> standard.flag & LIB_UI_FLAG_hover ) continue;

		ui -> button[ i ] -> standard.flag |= LIB_UI_FLAG_hover;
		lib_ui_show_button( ui, i );
		flush = TRUE;
	}

	for( uint64_t i = 0; i < ui -> limit_checkbox; i++ ) {
		if( ui -> window -> x < ui -> checkbox[ i ] -> standard.x || ui -> window -> x > (ui -> checkbox[ i ] -> standard.x + ui -> checkbox[ i ] -> standard.width) || ui -> window -> y < ui -> checkbox[ i ] -> standard.y || (ui -> window -> y > ui -> checkbox[ i ] -> standard.y + ui -> checkbox[ i ] -> standard.height) ) {
			if( ! (ui -> checkbox[ i ] -> standard.flag & LIB_UI_FLAG_hover) ) continue;
			ui -> checkbox[ i ] -> standard.flag &= ~LIB_UI_FLAG_hover;
			lib_ui_show_checkbox( ui, i );
			flush = TRUE;
			continue;
		}

		if( ui -> checkbox[ i ] -> standard.flag & LIB_UI_FLAG_hover ) continue;

		ui -> checkbox[ i ] -> standard.flag |= LIB_UI_FLAG_hover;
		lib_ui_show_checkbox( ui, i );
		flush = TRUE;
	}

	for( uint64_t i = 0; i < ui -> limit_radio; i++ ) {
		if( ui -> window -> x < ui -> radio[ i ] -> standard.x || ui -> window -> x > (ui -> radio[ i ] -> standard.x + ui -> radio[ i ] -> standard.width) || ui -> window -> y < ui -> radio[ i ] -> standard.y || (ui -> window -> y > ui -> radio[ i ] -> standard.y + ui -> radio[ i ] -> standard.height) ) {
			if( ! (ui -> radio[ i ] -> standard.flag & LIB_UI_FLAG_hover) ) continue;
			ui -> radio[ i ] -> standard.flag &= ~LIB_UI_FLAG_hover;
			lib_ui_show_radio( ui, i );
			flush = TRUE;
			continue;
		}

		if( ui -> radio[ i ] -> standard.flag & LIB_UI_FLAG_hover ) continue;

		ui -> radio[ i ] -> standard.flag |= LIB_UI_FLAG_hover;
		lib_ui_show_radio( ui, i );
		flush = TRUE;
	}

	if( flush) ui -> window -> flags |= LIB_WINDOW_FLAG_flush;
}

void lib_ui_show_button( struct LIB_UI_STRUCTURE *ui, uint64_t id ) {
	// set pointer location of element inside window
	uint32_t *pixel = ui -> window -> pixel + (ui -> button[ id ] -> standard.y * ui -> window -> width) + ui -> button[ id ] -> standard.x;

	uint32_t color = LIB_UI_COLOR_BACKGROUND_BUTTON;
	if( ui -> button[ id ] -> standard.flag & LIB_UI_FLAG_hover ) color += LIB_UI_COLOR_INCREASE;

	lib_ui_fill_rectangle( pixel, ui -> window -> width, LIB_UI_RADIUS_DEFAULT, ui -> button[ id ] -> standard.width, ui -> button[ id ] -> standard.height, color );

	if( ui -> button[ id ] -> standard.height > LIB_FONT_HEIGHT_pixel ) pixel += ((ui -> button[ id ] -> standard.height - LIB_FONT_HEIGHT_pixel) >> 1) * ui -> window -> width;

	lib_font( LIB_FONT_FAMILY_ROBOTO, ui -> button[ id ] -> name, lib_string_length( ui -> button[ id ] -> name ), 0xFF000000, pixel + (ui -> button[ id ] -> standard.width >> 1), ui -> window -> width, LIB_FONT_ALIGN_center );
}

void lib_ui_show_checkbox( struct LIB_UI_STRUCTURE *ui, uint64_t id ) {
	// set pointer location of element inside window
	uint32_t *pixel = ui -> window -> pixel + (ui -> checkbox[ id ] -> standard.y * ui -> window -> width) + ui -> checkbox[ id ] -> standard.x;

	uint32_t color = LIB_UI_COLOR_BACKGROUND_CHECKBOX;
	if( ui -> checkbox[ id ] -> standard.flag & LIB_UI_FLAG_hover ) color += LIB_UI_COLOR_INCREASE;

	// clean area
	lib_ui_fill_rectangle( pixel, ui -> window -> width, 0, ui -> checkbox[ id ] -> standard.width, ui -> checkbox[ id ] -> standard.height, LIB_UI_COLOR_BACKGROUND_DEFAULT );

	// show checkbox
	lib_ui_fill_rectangle( pixel, ui -> window -> width, 2, ui -> checkbox[ id ] -> standard.height, ui -> checkbox[ id ] -> standard.height, color );

	// show description
	lib_font( LIB_FONT_FAMILY_ROBOTO, ui -> checkbox[ id ] -> name, lib_string_length( ui -> checkbox[ id ] -> name ), LIB_UI_COLOR_DEFAULT, pixel + ui -> checkbox[ id ] -> standard.height + 4, ui -> window -> width, LIB_FONT_ALIGN_left );
}

void lib_ui_show_label( struct LIB_UI_STRUCTURE *ui, uint64_t id ) {
	// set pointer location of element inside window
	uint32_t *pixel = ui -> window -> pixel + (ui -> label[ id ] -> standard.y * ui -> window -> width) + ui -> label[ id ] -> standard.x;

	if( ui -> label[ id ] -> standard.height > LIB_FONT_HEIGHT_pixel ) pixel += ((ui -> label[ id ] -> standard.height - LIB_FONT_HEIGHT_pixel) >> 1) * ui -> window -> width;

	// show description
	lib_font( LIB_FONT_FAMILY_ROBOTO, ui -> label[ id ] -> name, lib_string_length( ui -> label[ id ] -> name ), LIB_UI_COLOR_DEFAULT, pixel, ui -> window -> width, LIB_FONT_ALIGN_left );
}

void lib_ui_show_radio( struct LIB_UI_STRUCTURE *ui, uint64_t id ) {
	// set pointer location of element inside window
	uint32_t *pixel = ui -> window -> pixel + (ui -> radio[ id ] -> standard.y * ui -> window -> width) + ui -> radio[ id ] -> standard.x;

	uint32_t color = LIB_UI_COLOR_BACKGROUND_RADIO;
	if( ui -> radio[ id ] -> standard.flag & LIB_UI_FLAG_hover ) color += LIB_UI_COLOR_INCREASE;

	// clean area
	lib_ui_fill_rectangle( pixel, ui -> window -> width, 0, ui -> radio[ id ] -> standard.width, ui -> radio[ id ] -> standard.height, LIB_UI_COLOR_BACKGROUND_DEFAULT );

	// show radio
	lib_ui_fill_rectangle( pixel, ui -> window -> width, ui -> radio[ id ] -> standard.height >> STD_SHIFT_2, ui -> radio[ id ] -> standard.height, ui -> radio[ id ] -> standard.height, color );

	// show description
	lib_font( LIB_FONT_FAMILY_ROBOTO, ui -> radio[ id ] -> name, lib_string_length( ui -> radio[ id ] -> name ), LIB_UI_COLOR_DEFAULT, pixel + ui -> radio[ id ] -> standard.height + 4, ui -> window -> width, LIB_FONT_ALIGN_left );
}
