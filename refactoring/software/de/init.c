/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint64_t de_init( void ) {
	// obtain information about kernel de_framebuffer
	std_framebuffer( (struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER *) &de_framebuffer );

	//----------------------------------------------------------------------

	// properties of file
	FILE *wallpaper_file = EMPTY;

	// properties of image
	struct LIB_IMAGE_STRUCTURE_TGA *wallpaper_image = EMPTY;

	// retrieve file information
	if( (wallpaper_file = fopen( (uint8_t *) "/usr/share/media/wallpaper/default.tga", EMPTY )) ) {
		// assign area for file
		wallpaper_image = (struct LIB_IMAGE_STRUCTURE_TGA *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( wallpaper_file -> byte ) >> STD_SHIFT_PAGE );

		// load file content
		fread( wallpaper_file, (uint8_t *) wallpaper_image, wallpaper_file -> byte );
	}

	// create wallpaper object
	de_window_wallpaper = lib_window( 0, 0, de_framebuffer.width_pixel, de_framebuffer.height_pixel );

	// properties of wallpaper area content
	uint32_t *wallpaper_pixel = (uint32_t *) ((uintptr_t) de_window_wallpaper + sizeof( struct LIB_WINDOW_DESCRIPTOR ));

	// if default wallpaper file found
	if( wallpaper_image ) {
		// convert image to RGBA
		uint32_t *tmp_wallpaper_image = (uint32_t *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( de_window_wallpaper -> width * de_window_wallpaper -> height * STD_VIDEO_DEPTH_byte ) >> STD_SHIFT_PAGE );
		lib_image_tga_parse( (uint8_t *) wallpaper_image, tmp_wallpaper_image, wallpaper_file -> byte );

		// copy scaled image content to wallpaper object
		float x_scale_factor = (float) ((float) wallpaper_image -> width / (float) de_window_wallpaper -> width);
		float y_scale_factor = (float) ((float) wallpaper_image -> height / (float) de_window_wallpaper -> height);
		for( uint16_t y = 0; y < de_window_wallpaper -> height; y++ )
			for( uint16_t x = 0; x < de_window_wallpaper -> width; x++ )
				wallpaper_pixel[ (y * de_window_wallpaper -> width) + x ] = tmp_wallpaper_image[ (uint64_t) (((uint64_t) (y_scale_factor * y) * wallpaper_image -> width) + (uint64_t) (x * x_scale_factor)) ];

		// release temporary image
		std_memory_release( (uintptr_t) tmp_wallpaper_image, MACRO_PAGE_ALIGN_UP( de_window_wallpaper -> width * de_window_wallpaper -> height * STD_VIDEO_DEPTH_byte ) >> STD_SHIFT_PAGE );

		// release file content
		std_memory_release( (uintptr_t) wallpaper_image, MACRO_PAGE_ALIGN_UP( wallpaper_file -> byte ) >> STD_SHIFT_PAGE );

		// close file
		fclose( wallpaper_file );
	} else
		// fill wallpaper with default color
		for( uint16_t y = 0; y < de_window_wallpaper -> height; y++ )
			for( uint16_t x = 0; x < de_window_wallpaper -> width; x++ )
				wallpaper_pixel[ (y * de_window_wallpaper -> width) + x ] = 0xFF101010;

	// object content ready for display
	de_window_wallpaper -> flags |= STD_WINDOW_FLAG_fixed_z | STD_WINDOW_FLAG_fixed_xy | STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;

	//----------------------------------------------------------------------

	// create taskbar object
	de_window_taskbar = lib_window( 0, de_framebuffer.height_pixel - DE_TASKBAR_HEIGHT_pixel, de_framebuffer.width_pixel, DE_TASKBAR_HEIGHT_pixel );

	// mark object as taskbar and unmovable
	de_window_taskbar -> flags = STD_WINDOW_FLAG_taskbar | STD_WINDOW_FLAG_fixed_z | STD_WINDOW_FLAG_fixed_xy;

	// fill taskbar with default background color
	uint32_t *taskbar_pixel = (uint32_t *) ((uintptr_t) de_window_taskbar + sizeof( struct LIB_WINDOW_DESCRIPTOR ));
	for( uint16_t y = 0; y < de_window_taskbar -> height; y++ )
		for( uint16_t x = 0; x < de_window_taskbar -> width; x++ )
			taskbar_pixel[ (y * de_window_taskbar -> width) + x ] = DE_TASKBAR_BACKGROUND_default;

	// show menu buton on taskbar
	uint8_t test[ 3 ] = "|||";
	lib_font( LIB_FONT_FAMILY_ROBOTO, (uint8_t *) &test, sizeof( test ), STD_COLOR_WHITE, taskbar_pixel + (((DE_TASKBAR_HEIGHT_pixel - LIB_FONT_HEIGHT_pixel) / 2) * de_window_taskbar -> width) + (22 >> STD_SHIFT_2), de_window_taskbar -> width, LIB_FONT_FLAG_ALIGN_center );

	// object content ready for display
	de_window_taskbar -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;

	// set taskbar list pointer
	de_taskbar_list_base_address = taskbar_pixel + DE_TASKBAR_HEIGHT_pixel;

	//----------------------------------------------------------------------

	// execute clock function as thread
	uint8_t de_string_clock[] = "de clock";
	std_thread( (uintptr_t) &de_clock, (uint8_t *) &de_string_clock, sizeof( de_string_clock ) );

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
		de_window_cursor = lib_window( de_framebuffer.width_pixel >> STD_SHIFT_2, de_framebuffer.height_pixel >> STD_SHIFT_2, cursor_image -> width, cursor_image -> height );
	} else
		// create default object
		de_window_cursor = lib_window( de_framebuffer.width_pixel >> STD_SHIFT_2, de_framebuffer.height_pixel >> STD_SHIFT_2, 16, 32 );

	// properties of cursor area content
	uint32_t *cursor_pixel = (uint32_t *) ((uintptr_t) de_window_cursor + sizeof( struct LIB_WINDOW_DESCRIPTOR ));

	// fill cursor with default color
	for( uint16_t y = 0; y < de_window_cursor -> height; y++ )
		for( uint16_t x = 0; x < de_window_cursor -> width; x++ )
			cursor_pixel[ (y * de_window_cursor -> width) + x ] = STD_COLOR_WHITE;

	// if default cursor file found
	if( cursor_image ) {
		// copy image content to cursor object
		lib_image_tga_parse( (uint8_t *) cursor_image, cursor_pixel, cursor_file -> byte );

		// release file content
		std_memory_release( (uintptr_t) cursor_image, MACRO_PAGE_ALIGN_UP( cursor_file -> byte ) >> STD_SHIFT_PAGE );

		// close file
		fclose( cursor_file );
	}

	// mark window as cursor, so Window Manager will treat it different than others
	de_window_cursor -> flags |= STD_WINDOW_FLAG_cursor;

	// yep, any object can be a cursor :) but only 1 with highest internal ID will be treated as it
	// so before you assign a flag to another object, make sure no other object have it

	// object content ready for display
	de_window_cursor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;

	// everything ok
	return EMPTY;
}
