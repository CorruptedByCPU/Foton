/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#include	"../library/ui.h"
#include	"../library/window.h"

uint64_t _main( uint64_t argc, uint8_t *argv[] ) {
	struct LIB_WINDOW_STRUCTURE *window = lib_window( 0, 0, 320, 200 );
	struct LIB_UI_STRUCTURE *ui = lib_ui( window );
	lib_ui_add_label( ui, LIB_UI_MARGIN_DEFAULT, LIB_UI_MARGIN_DEFAULT, (uint8_t *) "Label" );

	// main loop
	while( TRUE );

	// end of execution
	return 0;
}
