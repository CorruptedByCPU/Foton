/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#include	"../../library/font.h"
#include	"../../library/ui.h"

uint64_t wm_menu( void ) {
	// create menu object
	wm -> menu = wm_object_create( 0, wm -> panel -> y - 320, 180, 320, LIB_WINDOW_FLAG_menu | LIB_WINDOW_FLAG_fixed_z | LIB_WINDOW_FLAG_fixed_xy );

	// object name
	uint8_t menu_name[] = "{menu}";
	for( uint8_t i = 0; i < sizeof( menu_name ); i++ ) wm -> menu -> descriptor -> name[ i ] = menu_name[ i ];

	// properties of menu area content
	uint32_t *menu_pixel = (uint32_t *) ((uintptr_t) wm -> menu -> descriptor + sizeof( struct LIB_WINDOW_STRUCTURE ));

	//----------------------------------------------------------------------

	struct LIB_UI_STRUCTURE *ui = lib_ui( wm -> menu -> descriptor );
	lib_ui_clean( ui );


	// debug
	uint8_t title[] = "Foton v0.628";
	lib_font( LIB_FONT_FAMILY_ROBOTO, (uint8_t *) &title, sizeof( title ) - 1, 0xFFFFFFFF, wm -> menu -> descriptor -> pixel + (wm -> menu -> width >> STD_SHIFT_2) + (LIB_UI_MARGIN_DEFAULT * wm -> menu -> width), wm -> menu -> width, LIB_FONT_FLAG_ALIGN_center | LIB_FONT_FLAG_WEIGHT_bold );

	//----------------------------------------------------------------------

	// show object at beginning
	wm -> menu -> descriptor -> flags |= LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_flush;

	while( TRUE ) {
		
	}

	return EMPTY;
}
