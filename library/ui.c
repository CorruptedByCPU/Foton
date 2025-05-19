/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#include	"../library/string.h"
#include	"../library/ui.h"

struct LIB_UI_STRUCTURE *lib_ui( struct LIB_WINDOW_STRUCTURE *window ) {
	struct LIB_UI_STRUCTURE *ui = malloc( sizeof( struct LIB_UI_STRUCTURE ) );
	ui -> window = window;

	ui -> limit_label = EMPTY;
	ui -> label = (struct LIB_UI_STRUCTURE_ELEMENT_LABEL **) malloc( ui -> limit_label );

	return ui;
}

uint64_t lib_ui_add_label( struct LIB_UI_STRUCTURE *ui, uint16_t x, uint16_t y, uint8_t *name ) {
	ui -> label = (struct LIB_UI_STRUCTURE_ELEMENT_LABEL **) realloc( ui -> label, sizeof( struct LIB_UI_STRUCTURE_ELEMENT_LABEL ) * (ui -> limit_label + TRUE) );

	ui -> label[ ui -> limit_label ] = (struct LIB_UI_STRUCTURE_ELEMENT_LABEL *) malloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_LABEL ) );
	ui -> label[ ui -> limit_label ] -> standard.x = x;
	ui -> label[ ui -> limit_label ] -> standard.y = y;
	ui -> label[ ui -> limit_label ] -> name = (uint8_t *) malloc( lib_string_length( name ) );
	for( uint64_t i = 0; i <= lib_string_length( name ); i++ ) ui -> label[ ui -> limit_label ] -> name[ i ] = name[ i ];

	return ui -> limit_label++;
}

void lib_ui_sync( struct LIB_UI_STRUCTURE *ui ) {
}
