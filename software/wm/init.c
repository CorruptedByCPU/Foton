/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_init( void ) {
	// Tiwyn environment global variables/functions/rountines
	wm = (struct WM_STRUCTURE *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct WM_STRUCTURE ) ) >> STD_SHIFT_PAGE );

	// disable window management, for now
	wm -> enable = FALSE;

	// retrieve our process ID
	wm -> pid = std_pid();

	// retrieve properties of kernel framebuffer
	std_framebuffer( (struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER *) &wm -> framebuffer );

	// assign area for an array of objects
	wm -> object		= (struct WM_STRUCTURE_OBJECT *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct WM_STRUCTURE_OBJECT ) * WM_OBJECT_LIMIT ) >> STD_SHIFT_PAGE );

	// assign area for a list of objects
	wm -> list		= (struct WM_STRUCTURE_OBJECT **) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct WM_STRUCTURE_OBJECT * ) * (WM_LIST_LIMIT + TRUE) ) >> STD_SHIFT_PAGE );	// blank entry at end list
	wm -> list_panel	= (struct WM_STRUCTURE_OBJECT **) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct WM_STRUCTURE_OBJECT * ) * (WM_LIST_LIMIT + TRUE) ) >> STD_SHIFT_PAGE );	// blank entry at end list

	// assign area for a list of zones
	wm -> zone		= (struct WM_STRUCTURE_ZONE *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct WM_STRUCTURE_ZONE ) * WM_ZONE_LIMIT ) >> STD_SHIFT_PAGE );

	//----------------------------------------------------------------------

	// create cache area
	wm -> canvas.width		= wm -> framebuffer.width_pixel;
	wm -> canvas.height		= wm -> framebuffer.height_pixel;
	wm -> canvas.descriptor	= (struct LIB_WINDOW_STRUCTURE_DESCRIPTOR *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( (wm -> canvas.width * wm -> canvas.height * STD_VIDEO_DEPTH_byte) + sizeof( struct LIB_WINDOW_STRUCTURE_DESCRIPTOR ) ) >> STD_SHIFT_PAGE );

	// leave cache untouched, first object synchronization will fill it up

	//----------------------------------------------------------------------

	// properties of file
	FILE *workbench_file = EMPTY;

	// properties of image
	struct LIB_IMAGE_STRUCTURE_TGA *workbench_image = EMPTY;

	// retrieve file information
	if( (workbench_file = fopen( (uint8_t *) "/usr/share/media/wallpaper/default.tga", EMPTY )) ) {
		// assign area for file
		workbench_image = (struct LIB_IMAGE_STRUCTURE_TGA *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( workbench_file -> byte ) >> STD_SHIFT_PAGE );

		// load file content
		fread( workbench_file, (uint8_t *) workbench_image, workbench_file -> byte );
	}

	// create workbench object
	wm -> workbench = wm_object_create( 0, 0, wm -> canvas.width, wm -> canvas.height );

	// properties of workbench area content
	uint32_t *workbench_pixel = (uint32_t *) ((uintptr_t) wm -> workbench -> descriptor + sizeof( struct LIB_WINDOW_STRUCTURE_DESCRIPTOR ));

	// if default wallpaper file found
	if( workbench_image ) {
		// convert image to RGBA
		uint32_t *tmp_workbench_image = (uint32_t *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( wm -> workbench -> limit ) >> STD_SHIFT_PAGE );
		lib_image_tga_parse( (uint8_t *) workbench_image, tmp_workbench_image, workbench_file -> byte );

		// copy scaled image content to workbench object
		float x_scale_factor = (float) ((float) workbench_image -> width / (float) wm -> workbench -> width);
		float y_scale_factor = (float) ((float) workbench_image -> height / (float) wm -> workbench -> height);
		for( uint16_t y = 0; y < wm -> workbench -> height; y++ )
			for( uint16_t x = 0; x < wm -> workbench -> width; x++ )
				workbench_pixel[ (y * wm -> workbench -> width) + x ] = tmp_workbench_image[ (uint64_t) (((uint64_t) (y_scale_factor * y) * workbench_image -> width) + (uint64_t) (x * x_scale_factor)) ];

		// release temporary image
		std_memory_release( (uintptr_t) tmp_workbench_image, MACRO_PAGE_ALIGN_UP( wm -> workbench -> limit ) >> STD_SHIFT_PAGE );

		// release file content
		std_memory_release( (uintptr_t) workbench_image, MACRO_PAGE_ALIGN_UP( workbench_file -> byte ) >> STD_SHIFT_PAGE );

		// close file
		fclose( workbench_file );
	} else
		// fill workbench with default color
		for( uint16_t y = 0; y < wm -> workbench -> height; y++ )
			for( uint16_t x = 0; x < wm -> workbench -> width; x++ )
				workbench_pixel[ (y * wm -> workbench -> width) + x ] = STD_COLOR_GRAY;

	// object content ready for display
	wm -> workbench -> descriptor -> flags = LIB_WINDOW_FLAG_fixed_z | LIB_WINDOW_FLAG_fixed_xy | LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_flush;

	//----------------------------------------------------------------------

	// create panel object
	wm -> panel = wm_object_create( 0, wm -> canvas.height - WM_PANEL_HEIGHT_pixel, wm -> canvas.width, WM_PANEL_HEIGHT_pixel );

	// properties of panel area content
	uint32_t *panel_pixel = (uint32_t *) ((uintptr_t) wm -> panel -> descriptor + sizeof( struct LIB_WINDOW_STRUCTURE_DESCRIPTOR ));

	// fill panel with default color

	// overlight
	for( uint16_t y = FALSE; y < TRUE; y++ ) for( uint16_t x = 0; x < wm -> panel -> width; x++ ) panel_pixel[ x ] = 0x20FFFFFF;
	// background
	for( uint16_t y = TRUE; y < wm -> panel -> height; y++ ) for( uint16_t x = 0; x < wm -> panel -> width; x++ ) panel_pixel[ (y * wm -> panel -> width) + x ] = WM_PANEL_COLOR_default;

	// show menu button on panel
	uint8_t test[ 3 ] = "|||";
	lib_font( LIB_FONT_FAMILY_ROBOTO, (uint8_t *) &test, sizeof( test ), STD_COLOR_WHITE, panel_pixel + (((wm -> panel -> height - LIB_FONT_HEIGHT_pixel) / 2) * wm -> panel -> width) + (wm -> panel -> height >> STD_SHIFT_2), wm -> panel -> width, LIB_FONT_ALIGN_center );

	// panel list is empty
	wm -> list_limit_panel = EMPTY;

	// object content ready for display
	wm -> panel -> descriptor -> flags = LIB_WINDOW_FLAG_panel | LIB_WINDOW_FLAG_fixed_z | LIB_WINDOW_FLAG_fixed_xy | LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_flush;

	//----------------------------------------------------------------------

	// properties of file
	FILE *cursor_file = EMPTY;

	// properties of image
	struct LIB_IMAGE_STRUCTURE_TGA *cursor_image = EMPTY;

	// retrieve file information
	if( (cursor_file = fopen( (uint8_t *) "/usr/share/media/cursor/default.tga", EMPTY )) ) {
		// assign area for file
		cursor_image = (struct LIB_IMAGE_STRUCTURE_TGA *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( cursor_file -> byte ) >> STD_SHIFT_PAGE );

		// load file content
		if( cursor_image ) fread( cursor_file, (uint8_t *) cursor_image, cursor_file -> byte );

		// create cursor object
		wm -> cursor = wm_object_create( wm -> workbench -> width >> STD_SHIFT_2, wm -> workbench -> height >> STD_SHIFT_2, cursor_image -> width, cursor_image -> height );
	} else
		// create default object
		wm -> cursor = wm_object_create( wm -> workbench -> width >> STD_SHIFT_2, wm -> workbench -> height >> STD_SHIFT_2, 16, 32 );

	// properties of cursor area content
	uint32_t *cursor_pixel = (uint32_t *) ((uintptr_t) wm -> cursor -> descriptor + sizeof( struct LIB_WINDOW_STRUCTURE_DESCRIPTOR ));

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

	// object content ready for display
	wm -> cursor -> descriptor -> flags = LIB_WINDOW_FLAG_cursor | LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_flush;

	//----------------------------------------------------------------------

	// debug
	std_exec( (uint8_t *) "so", 2, EMPTY, TRUE );
}
