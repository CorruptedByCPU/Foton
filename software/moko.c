/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/font.h"
	#include	"../library/window.h"
	#include	"../library/ui.h"
	// //----------------------------------------------------------------------
	// // static, structures, definitions
	// //----------------------------------------------------------------------
	// #include	"./kuro/config.h"
	// #include	"./kuro/dir.h"
	// #include	"./kuro/icon.h"
	// //----------------------------------------------------------------------
	// // variables
	// //----------------------------------------------------------------------
	// #include	"./kuro/data.c"
	// //----------------------------------------------------------------------
	// // routines, procedures
	// //----------------------------------------------------------------------
	// #include	"./kuro/dir.c"
	// #include	"./kuro/icon.c"
	// #include	"./kuro/init.c"
	// //----------------------------------------------------------------------

#define	WINDOW_WIDTH	LIB_UI_MARGIN_DEFAULT + 256 + LIB_UI_PADDING_DEFAULT + 25 + LIB_UI_PADDING_DEFAULT + 108 + LIB_UI_MARGIN_DEFAULT
#define	WINDOW_HEIGHT	297
// #define	WINDOW_WIDTH	136 + 1
// #define	WINDOW_HEIGHT	146

#define	TEXTAREA_WIDTH	-1
#define	TEXTAREA_HEIGHT	-1

struct LIB_WINDOW_STRUCTURE *window;
struct LIB_UI_STRUCTURE *ui;

uint8_t *document;

uint64_t _main( uint64_t argc, uint8_t *argv[] ) {
	window = lib_window( 606, 289, WINDOW_WIDTH, WINDOW_HEIGHT );
	// window = lib_window( -1, -1, WINDOW_WIDTH, WINDOW_HEIGHT );

	ui = lib_ui( window );

	ui -> icon = lib_image_scale( lib_ui_icon( (uint8_t *) "/var/share/media/icon/default/app/accessories-text-editor.tga" ), 48, 48, 16, 16 );

	// add icon to window properties
	for( uint64_t i = 0; i < 16 * 16; i++ ) window -> icon[ i ] = ui -> icon[ i ];

	lib_ui_clean( ui );

	lib_window_name( ui -> window, (uint8_t *) "Moko - Text Editor" );

	lib_ui_add_control( ui, LIB_UI_ELEMENT_CONTROL_TYPE_close );
	lib_ui_add_control( ui, LIB_UI_ELEMENT_CONTROL_TYPE_max );
	lib_ui_add_control( ui, LIB_UI_ELEMENT_CONTROL_TYPE_min );

	document = (uint8_t *) malloc( TRUE );

	// file properties
	FILE *file = EMPTY;

	// file exist?
	if( (file = fopen( (uint8_t *) "/todo.log", EMPTY )) ) {
		// load file content
		document = (uint8_t *) realloc( document, file -> byte );
		fread( file, document, file -> byte );

		// close file
		fclose( file );
	}

	lib_ui_add_textarea( ui, LIB_UI_MARGIN_DEFAULT, LIB_UI_HEADER_HEIGHT, TEXTAREA_WIDTH, -1, EMPTY, document, LIB_FONT_FAMILY_ROBOTO_MONO );

	lib_ui_flush( ui );

	window -> flags |= LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_icon | LIB_WINDOW_FLAG_resizable | LIB_WINDOW_FLAG_flush;

	while( TRUE ) {
		sleep( TRUE );

		uint16_t key = lib_ui_event( ui );

		// exit?
		if( key == STD_ASCII_ESC ) break;	// yes
	}

	return EMPTY;
}
