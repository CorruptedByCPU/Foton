/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#include	"../library/ui.h"

struct LIB_UI_STRUCTURE *lib_ui( struct LIB_WINDOW_STRUCTURE *window ) {
	struct LIB_UI_STRUCTURE *ui = malloc( sizeof( struct LIB_UI_STRUCTURE ) );
	ui -> window = window;

	return ui;
}
