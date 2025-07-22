/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_init( void ) {
	// Window Manager environment global variables/functions/rountines
	wm = (struct WM_STRUCTURE *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct WM_STRUCTURE ) ) >> STD_SHIFT_PAGE );

	// disable window management, for now
	wm -> enable = FALSE;

	// retrieve our process ID
	wm -> pid = std_pid();

	// retrieve properties of kernel framebuffer
	std_framebuffer( (struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER *) &wm -> framebuffer );

	// are we master of pupets?
	if( wm -> pid != wm -> framebuffer.pid ) exit();	// no

	// assign area for an array of objects
	wm -> object		= (struct WM_STRUCTURE_OBJECT *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct WM_STRUCTURE_OBJECT ) * WM_OBJECT_LIMIT ) >> STD_SHIFT_PAGE );

	// assign area for a list of objects
	wm -> list		= (struct WM_STRUCTURE_OBJECT **) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct WM_STRUCTURE_OBJECT * ) * (WM_LIST_LIMIT + TRUE) ) >> STD_SHIFT_PAGE );	// blank entry at end list
	wm -> list_panel	= (struct WM_STRUCTURE_OBJECT **) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct WM_STRUCTURE_OBJECT * ) * (WM_LIST_LIMIT + TRUE) ) >> STD_SHIFT_PAGE );	// blank entry at end list

	// assign area for a list of zones
	wm -> zone		= (struct WM_STRUCTURE_ZONE *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct WM_STRUCTURE_ZONE ) * WM_ZONE_LIMIT ) >> STD_SHIFT_PAGE );

	//----------------------------------------------------------------------

	// create cache area
	wm -> canvas.width	= wm -> framebuffer.width_pixel;
	wm -> canvas.height	= wm -> framebuffer.height_pixel;
	wm -> canvas.descriptor	= (struct LIB_WINDOW_STRUCTURE *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( (wm -> canvas.width * wm -> canvas.height * STD_VIDEO_DEPTH_byte) + sizeof( struct LIB_WINDOW_STRUCTURE ) ) >> STD_SHIFT_PAGE );

	// additional property, nice to have
	wm -> canvas.descriptor -> pixel = (uint32_t *) ((uintptr_t) wm -> canvas.descriptor + sizeof( struct LIB_WINDOW_STRUCTURE ));

	// leave cache untouched, first object synchronization will fill it up

	//----------------------------------------------------------------------

	// create workbench object
	wm -> workbench = wm_object_create( 0, 0, wm -> canvas.width, wm -> canvas.height, LIB_WINDOW_FLAG_fixed_z | LIB_WINDOW_FLAG_fixed_xy | LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_flush );

	// object name
	uint8_t workbench_name[] = "{workbench}";
	for( uint8_t i = 0; i < sizeof( workbench_name ); i++ ) wm -> workbench -> descriptor -> name[ i ] = workbench_name[ i ];

	// properties of workbench area content
	uint32_t *workbench_pixel = (uint32_t *) ((uintptr_t) wm -> workbench -> descriptor + sizeof( struct LIB_WINDOW_STRUCTURE ));

	// fill workbench with default gradient
	uint8_t r1 = 0x2C, g1 = 0x39, b1 = 0x1D;
	uint8_t r2 = 0x08, g2 = 0x08, b2 = 0x08;
	for( uint16_t y = 0; y < wm -> workbench -> height; y++ ) {
		double ratio = (double) y / (wm -> workbench -> height - 1);
		uint32_t color = ((uint8_t) 0xFF << 24) | (((uint8_t) (r1 + (r2 - r1) * ratio)) << 16) | (((uint8_t) (g1 + (g2 - g1) * ratio)) << 8) | ((uint8_t) (b1 + (b2 - b1) * ratio));

		for( uint16_t x = 0; x < wm -> workbench -> width; x++ ) workbench_pixel[ (y * wm -> workbench -> width) + x ] = color;
	}

	//----------------------------------------------------------------------

	// create panel object
	wm -> panel = wm_object_create( 0, wm -> canvas.height - WM_PANEL_HEIGHT_pixel, wm -> canvas.width, WM_PANEL_HEIGHT_pixel, LIB_WINDOW_FLAG_panel | LIB_WINDOW_FLAG_fixed_z | LIB_WINDOW_FLAG_fixed_xy | LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_flush );

	// object name
	uint8_t panel_name[] = "{panel}";
	for( uint8_t i = 0; i < sizeof( panel_name ); i++ ) wm -> panel -> descriptor -> name[ i ] = panel_name[ i ];

	// properties of panel area content
	uint32_t *panel_pixel = (uint32_t *) ((uintptr_t) wm -> panel -> descriptor + sizeof( struct LIB_WINDOW_STRUCTURE ));

	// fill panel with default color

	// background
	for( uint16_t x = 0; x < wm -> panel -> width; x++ ) panel_pixel[ x ] = LIB_UI_COLOR_BORDER_DEFAULT + LIB_UI_COLOR_INCREASE_LITTLE;
	for( uint16_t y = TRUE; y < wm -> panel -> height; y++ ) for( uint16_t x = 0; x < wm -> panel -> width; x++ ) panel_pixel[ (y * wm -> panel -> width) + x ] = LIB_UI_COLOR_BACKGROUND_DEFAULT;

	// show menu button on panel
	uint8_t test[ 3 ] = "|||";
	lib_font( LIB_FONT_FAMILY_ROBOTO, (uint8_t *) &test, sizeof( test ), STD_COLOR_WHITE, panel_pixel + (((wm -> panel -> height - LIB_FONT_HEIGHT_pixel) / 2) * wm -> panel -> width) + (wm -> panel -> height >> STD_SHIFT_2), wm -> panel -> width, LIB_FONT_FLAG_ALIGN_center );

	// panel list is empty
	wm -> list_limit_panel = EMPTY;

	//----------------------------------------------------------------------

	// properties of file
	FILE *cursor_file = EMPTY;

	// properties of image
	struct LIB_IMAGE_STRUCTURE_TGA *cursor_image = EMPTY;

	// retrieve file information
	if( (cursor_file = fopen( (uint8_t *) "/var/share/media/cursor/default.tga", EMPTY )) ) {
		// assign area for file
		cursor_image = (struct LIB_IMAGE_STRUCTURE_TGA *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( cursor_file -> byte ) >> STD_SHIFT_PAGE );

		// load file content
		if( cursor_image ) fread( cursor_file, (uint8_t *) cursor_image, cursor_file -> byte );

		// create cursor object
		wm -> cursor = wm_object_create( wm -> workbench -> width >> STD_SHIFT_2, wm -> workbench -> height >> STD_SHIFT_2, cursor_image -> width, cursor_image -> height, LIB_WINDOW_FLAG_cursor | LIB_WINDOW_FLAG_transparent | LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_flush );
	} else
		// create default object
		wm -> cursor = wm_object_create( wm -> workbench -> width >> STD_SHIFT_2, wm -> workbench -> height >> STD_SHIFT_2, 16, 32, LIB_WINDOW_FLAG_cursor | LIB_WINDOW_FLAG_transparent | LIB_WINDOW_FLAG_flush );

	// object name
	uint8_t cursor_name[] = "{cursor}";
	for( uint8_t i = 0; i < sizeof( cursor_name ); i++ ) wm -> cursor -> descriptor -> name[ i ] = cursor_name[ i ];

	// properties of cursor area content
	uint32_t *cursor_pixel = (uint32_t *) ((uintptr_t) wm -> cursor -> descriptor + sizeof( struct LIB_WINDOW_STRUCTURE ));

	// fill cursor with default color
	for( uint16_t y = 0; y < wm -> cursor -> height; y++ )
		for( uint16_t x = 0; x < wm -> cursor -> width; x++ )
			cursor_pixel[ (y * wm -> cursor -> width) + x ] = STD_COLOR_WHITE;

	// if default cursor file found
	if( cursor_image ) {
		// copy image content to cursor object
		lib_image_tga_parse( (uint8_t *) cursor_image, cursor_pixel, cursor_file -> byte );

		// release file content
		std_memory_release( (uintptr_t) cursor_image, MACRO_PAGE_ALIGN_UP( cursor_file -> byte ) >> STD_SHIFT_PAGE );

		// close file
		fclose( cursor_file );
	}

	//----------------------------------------------------------------------

	// execute menu function as thread
	uint8_t wm_string_menu[] = "wm menu";
	std_thread( (uintptr_t) &wm_menu, (uint8_t *) &wm_string_menu, sizeof( wm_string_menu ) );

	//----------------------------------------------------------------------

	// debug
	std_exec( (uint8_t *) "kuro", 4, EMPTY, TRUE );
	std_exec( (uint8_t *) "3d /var/share/media/obj/demo.obj", 32, EMPTY, TRUE );
	std_exec( (uint8_t *) "test", 4, EMPTY, TRUE );
	std_exec( (uint8_t *) "palette", 7, EMPTY, TRUE );
	std_exec( (uint8_t *) "moko /moko.txt", 14, EMPTY, TRUE );
}
