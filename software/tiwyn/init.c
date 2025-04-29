/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint64_t tiwyn_init( void ) {
	// retrieve our process ID
	tiwyn_pid = std_pid();

	// retrieve properties of kernel framebuffer
	std_framebuffer( (struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER *) &tiwyn_framebuffer );

	// kernel framebuffer locked by someone else?
	if( tiwyn_pid != tiwyn_framebuffer.pid ) return STD_ERROR_locked;	// yep

	// assign area for an array of objects
	tiwyn_object_base_address = (struct TIWYN_STRUCTURE_OBJECT *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct TIWYN_STRUCTURE_OBJECT ) * TIWYN_OBJECT_LIMIT ) >> STD_SHIFT_PAGE );

	// assign area for a list of objects
	tiwyn_list_base_address = (struct TIWYN_STRUCTURE_OBJECT **) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct TIWYN_STRUCTURE_OBJECT * ) * (TIWYN_LIST_LIMIT + TRUE) ) >> STD_SHIFT_PAGE );	// blank entry at end list

	// assign area for a list of zones
	tiwyn_zone_base_address = (struct TIWYN_STRUCTURE_ZONE *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct TIWYN_STRUCTURE_ZONE ) * TIWYN_ZONE_LIMIT ) >> STD_SHIFT_PAGE );

	//----------------------------------------------------------------------

	// create cache area
	tiwyn_object_cache.width		= tiwyn_framebuffer.width_pixel;
	tiwyn_object_cache.height		= tiwyn_framebuffer.height_pixel;
	tiwyn_object_cache.descriptor	= (struct LIB_WINDOW_DESCRIPTOR *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( (tiwyn_object_cache.width * tiwyn_object_cache.height * STD_VIDEO_DEPTH_byte) + sizeof( struct LIB_WINDOW_DESCRIPTOR ) ) >> STD_SHIFT_PAGE );

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
	tiwyn_object_workbench = tiwyn_object_create( 0, 0, tiwyn_object_cache.width, tiwyn_object_cache.height );

	// mark it as our
	tiwyn_object_workbench -> pid = tiwyn_pid;

	// properties of workbench area content
	uint32_t *workbench_pixel = (uint32_t *) ((uintptr_t) tiwyn_object_workbench -> descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR ));

	// if default wallpaper file found
	if( workbench_image ) {
		// convert image to RGBA
		uint32_t *tmp_workbench_image = (uint32_t *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( tiwyn_object_workbench -> limit ) >> STD_SHIFT_PAGE );
		lib_image_tga_parse( (uint8_t *) workbench_image, tmp_workbench_image, workbench_file -> byte );

		// copy scaled image content to workbench object
		float x_scale_factor = (float) ((float) workbench_image -> width / (float) tiwyn_object_workbench -> width);
		float y_scale_factor = (float) ((float) workbench_image -> height / (float) tiwyn_object_workbench -> height);
		for( uint16_t y = 0; y < tiwyn_object_workbench -> height; y++ )
			for( uint16_t x = 0; x < tiwyn_object_workbench -> width; x++ )
				workbench_pixel[ (y * tiwyn_object_workbench -> width) + x ] = tmp_workbench_image[ (uint64_t) (((uint64_t) (y_scale_factor * y) * workbench_image -> width) + (uint64_t) (x * x_scale_factor)) ];

		// release temporary image
		std_memory_release( (uintptr_t) tmp_workbench_image, MACRO_PAGE_ALIGN_UP( tiwyn_object_workbench -> limit ) >> STD_SHIFT_PAGE );

		// release file content
		std_memory_release( (uintptr_t) workbench_image, MACRO_PAGE_ALIGN_UP( workbench_file -> byte ) >> STD_SHIFT_PAGE );

		// close file
		fclose( workbench_file );
	} else
		// fill workbench with default color
		for( uint16_t y = 0; y < tiwyn_object_workbench -> height; y++ )
			for( uint16_t x = 0; x < tiwyn_object_workbench -> width; x++ )
				workbench_pixel[ (y * tiwyn_object_workbench -> width) + x ] = 0xFF080808;

	// object content ready for display
	tiwyn_object_workbench -> descriptor -> flags = STD_WINDOW_FLAG_fixed_z | STD_WINDOW_FLAG_fixed_xy | STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;

	//----------------------------------------------------------------------

	// create panel object
	tiwyn_object_panel = tiwyn_object_create( 0, tiwyn_object_cache.height - TIWYN_PANEL_HEIGHT_pixel, tiwyn_object_cache.width, TIWYN_PANEL_HEIGHT_pixel );

	// mark it as our
	tiwyn_object_panel -> pid = tiwyn_pid;

	// properties of panel area content
	uint32_t *panel_pixel = (uint32_t *) ((uintptr_t) tiwyn_object_panel -> descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR ));

	// fill panel with default color
	for( uint16_t y = 0; y < tiwyn_object_panel -> height; y++ )
		for( uint16_t x = 0; x < tiwyn_object_panel -> width; x++ )
			panel_pixel[ (y * tiwyn_object_panel -> width) + x ] = 0xFF101010;

	// show menu button on panel
	uint8_t test[ 3 ] = "|||";
	lib_font( LIB_FONT_FAMILY_ROBOTO, (uint8_t *) &test, sizeof( test ), STD_COLOR_WHITE, panel_pixel + (((tiwyn_object_panel -> height - LIB_FONT_HEIGHT_pixel) / 2) * tiwyn_object_panel -> width) + (tiwyn_object_panel -> height >> STD_SHIFT_2), tiwyn_object_panel -> width, LIB_FONT_ALIGN_center );

	// object content ready for display
	tiwyn_object_panel -> descriptor -> flags = STD_WINDOW_FLAG_panel | STD_WINDOW_FLAG_fixed_z | STD_WINDOW_FLAG_fixed_xy | STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;

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
		tiwyn_object_cursor = tiwyn_object_create( tiwyn_object_workbench -> width >> STD_SHIFT_2, tiwyn_object_workbench -> height >> STD_SHIFT_2, cursor_image -> width, cursor_image -> height );
	} else
		// create default object
		tiwyn_object_cursor = tiwyn_object_create( tiwyn_object_workbench -> width >> STD_SHIFT_2, tiwyn_object_workbench -> height >> STD_SHIFT_2, 16, 32 );

	// mark it as our
	tiwyn_object_cursor -> pid = tiwyn_pid;

	// properties of cursor area content
	uint32_t *cursor_pixel = (uint32_t *) ((uintptr_t) tiwyn_object_cursor -> descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR ));

	// fill cursor with default color
	for( uint16_t y = 0; y < tiwyn_object_cursor -> height; y++ )
		for( uint16_t x = 0; x < tiwyn_object_cursor -> width; x++ )
			cursor_pixel[ (y * tiwyn_object_cursor -> width) + x ] = STD_COLOR_WHITE;

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
	tiwyn_object_cursor -> descriptor -> flags = STD_WINDOW_FLAG_cursor | STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;

	//----------------------------------------------------------------------

	// done
	return EMPTY;
}
