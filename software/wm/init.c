/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t wm_init( void ) {
	// get our PID number
	wm_pid = std_pid();

	// obtain information about kernel framebuffer
	std_framebuffer( (struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER *) &kernel_framebuffer );

	// framebuffer locked?
	if( kernel_framebuffer.pid != wm_pid ) { printf( "WM: Framebuffer is already in use by process with ID %u.", wm_pid ); return FALSE; }

	//----------------------------------------------------------------------

	// prepare area for an array of objects
	wm_object_base_address = (struct WM_STRUCTURE_OBJECT *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct WM_STRUCTURE_OBJECT ) * WM_OBJECT_LIMIT ) >> STD_SHIFT_PAGE );

	// prepare area for a list of objects
	wm_list_base_address = (struct WM_STRUCTURE_OBJECT **) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct WM_STRUCTURE_OBJECT * ) * (WM_LIST_LIMIT + 1) ) >> STD_SHIFT_PAGE );	// blank entry at the end of object list

	// prepare area for a list of zones
	wm_zone_base_address = (struct WM_STRUCTURE_ZONE *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct WM_STRUCTURE_ZONE ) * WM_ZONE_LIMIT ) >> STD_SHIFT_PAGE );

	// prepare area for a taskbar list
	wm_taskbar_base_address = (struct WM_STRUCTURE_OBJECT **) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct WM_STRUCTURE_OBJECT * ) * WM_TASKBAR_LIMIT ) >> STD_SHIFT_PAGE );

	//----------------------------------------------------------------------

	// create cache area

	// as local object
	wm_object_cache.width		= kernel_framebuffer.width_pixel;
	wm_object_cache.height		= kernel_framebuffer.height_pixel;
	wm_object_cache.descriptor	= (struct STD_WINDOW_STRUCTURE_DESCRIPTOR *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( (wm_object_cache.width * wm_object_cache.height * STD_VIDEO_DEPTH_byte) + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR ) ) >> STD_SHIFT_PAGE );

	// leave cache untouched, first objects synchronization will fill it up

	//----------------------------------------------------------------------

	// properties of file
	FILE *workbench_file;

	// properties of image
	struct LIB_IMAGE_TGA_STRUCTURE *workbench_image = EMPTY;

	// retrieve information file
	if( (workbench_file = fopen( (uint8_t *) "/system/var/gfx/wallpapers/default.tga" )) ) {
		// assign area for file
		workbench_image = (struct LIB_IMAGE_TGA_STRUCTURE *) malloc( workbench_file -> byte );

		// load file content
		if( workbench_image ) fread( workbench_file, (uint8_t *) workbench_image, workbench_file -> byte );
	}

	// create workbench object
	wm_object_workbench = wm_object_create( 0, 0, wm_object_cache.width, wm_object_cache.height );

	// mark it as our
	wm_object_workbench -> pid = wm_pid;

	// properties of workbench area content
	uint32_t *workbench_pixel = (uint32_t *) ((uintptr_t) wm_object_workbench -> descriptor + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR ));

	// if default wallpaper file found
	if( workbench_image ) {
		// convert image to RGBA
		uint32_t *tmp_workbench_image = (uint32_t *) malloc( wm_object_workbench -> size_byte );
		lib_image_tga_parse( (uint8_t *) workbench_image, tmp_workbench_image, workbench_file -> byte );

		// copy scaled image content to workbench object
		float x_scale_factor = (float) ((float) workbench_image -> width / (float) wm_object_workbench -> width);
		float y_scale_factor = (float) ((float) workbench_image -> height / (float) wm_object_workbench -> height);
		for( uint16_t y = 0; y < wm_object_workbench -> height; y++ )
			for( uint16_t x = 0; x < wm_object_workbench -> width; x++ )
				workbench_pixel[ (y * wm_object_workbench -> width) + x ] = tmp_workbench_image[ (uint64_t) (((uint64_t) (y_scale_factor * y) * workbench_image -> width) + (uint64_t) (x * x_scale_factor)) ];

		// release temporary image
		// free( tmp_workbench_image );

		// release file content
		// free( workbench_image );

		// close file
		// fclose( workbench_file );
	} else
		// fill workbench with default color
		for( uint16_t y = 0; y < wm_object_workbench -> height; y++ )
			for( uint16_t x = 0; x < wm_object_workbench -> width; x++ )
				workbench_pixel[ (y * wm_object_workbench -> width) + x ] = STD_COLOR_BLACK;

	// show debug information
	uint8_t build_version[] = "System build on "__DATE__" "__TIME__;
	lib_font( LIB_FONT_FAMILY_ROBOTO_MONO, build_version, sizeof( build_version ) - 1, STD_COLOR_GRAY, workbench_pixel, wm_object_workbench -> width, LIB_FONT_ALIGN_right );

	// object content ready for display
	wm_object_workbench -> descriptor -> flags |= STD_WINDOW_FLAG_fixed_z | STD_WINDOW_FLAG_fixed_xy | STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;

	//----------------------------------------------------------------------

	// create taskbar object
	wm_object_taskbar = wm_object_create( 0, wm_object_workbench -> height - WM_OBJECT_TASKBAR_HEIGHT_pixel, wm_object_workbench -> width, WM_OBJECT_TASKBAR_HEIGHT_pixel );

	// mark object as taskbar and unmovable
	wm_object_taskbar -> descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_taskbar | STD_WINDOW_FLAG_fixed_z | STD_WINDOW_FLAG_fixed_xy;

	// fill taskbar with default background color
	uint32_t *taskbar_pixel = (uint32_t *) ((uintptr_t) wm_object_taskbar -> descriptor + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR ));
	for( uint16_t y = 0; y < wm_object_taskbar -> height; y++ )
		for( uint16_t x = 0; x < wm_object_taskbar -> width; x++ )
			taskbar_pixel[ (y * wm_object_taskbar -> width) + x ] = WM_TASKBAR_BG_default;

	// show menu buton on taskbar
	uint8_t test[ 3 ] = "|||";
	lib_font( LIB_FONT_FAMILY_ROBOTO, (uint8_t *) &test, sizeof( test ), 0xFFFFFFFF, taskbar_pixel + (((WM_OBJECT_TASKBAR_HEIGHT_pixel - LIB_FONT_HEIGHT_pixel) / 2) * wm_object_taskbar -> width) + (22 >> STD_SHIFT_2), wm_object_taskbar -> width, LIB_FONT_ALIGN_center );

	// execute taskbar function as thread
	uint8_t wm_string_taskbar[] = "{wm: taskbar}";
	wm_object_taskbar -> pid = std_thread( (uintptr_t) &wm_taskbar, (uint8_t *) &wm_string_taskbar, sizeof( wm_string_taskbar ) );

	//----------------------------------------------------------------------

	// execute clock function as thread
	uint8_t wm_string_clock[] = "{wm: clock}";
	std_thread( (uintptr_t) &wm_clock, (uint8_t *) &wm_string_clock, sizeof( wm_string_clock ) );

	//----------------------------------------------------------------------

	// properties of file
	FILE *cursor_file;

	// properties of image
	struct LIB_IMAGE_TGA_STRUCTURE *cursor_image = EMPTY;

	// retrieve information about module file
	if( (cursor_file = fopen( (uint8_t *) "/system/var/gfx/cursors/default.tga" )) ) {
		// assign area for file
		cursor_image = (struct LIB_IMAGE_TGA_STRUCTURE *) malloc( cursor_file -> byte );

		// load file content
		if( cursor_image ) fread( cursor_file, (uint8_t *) cursor_image, cursor_file -> byte );

		// create cursor object
		wm_object_cursor = wm_object_create( wm_object_workbench -> width >> STD_SHIFT_2, wm_object_workbench -> height >> STD_SHIFT_2, cursor_image -> width, cursor_image -> height );
	} else
		// create default object
		wm_object_cursor = wm_object_create( wm_object_workbench -> width >> STD_SHIFT_2, wm_object_workbench -> height >> STD_SHIFT_2, 16, 32 );

	// mark it as our
	wm_object_cursor -> pid = wm_pid;

	// properties of cursor area content
	uint32_t *cursor_pixel = (uint32_t *) ((uintptr_t) wm_object_cursor -> descriptor + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR ));

	// fill cursor with default color
	for( uint16_t y = 0; y < wm_object_cursor -> height; y++ )
		for( uint16_t x = 0; x < wm_object_cursor -> width; x++ )
			cursor_pixel[ (y * wm_object_cursor -> width) + x ] = STD_COLOR_WHITE;

	// if default cursor file found
	if( cursor_image ) {
		// copy image content to cursor object
		lib_image_tga_parse( (uint8_t *) cursor_image, cursor_pixel, cursor_file -> byte );

		// release file content
		// free( cursor_image );

		// close file
		// fclose( cursor_file );
	}

	// mark window as cursor, so Window Manager will treat it different than others
	wm_object_cursor -> descriptor -> flags |= STD_WINDOW_FLAG_cursor;

	// yep, any object can be a cursor :) but only 1 with highest internal ID will be treated as it
	// so before you assign a flag to another object, make sure no other object have it

	// object content ready for display
	wm_object_cursor -> descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;

	//----------------------------------------------------------------------

	// execute taskbar function as thread
	uint8_t wm_string_release[] = "{wm: release}";
	std_thread( (uintptr_t) &wm_release, (uint8_t *) &wm_string_release, sizeof( wm_string_release ) );

	// debug
	std_exec( (uint8_t *) "console", 7, EMPTY );
	// std_exec( (uint8_t *) "console moko", 12, EMPTY );
	// std_exec( (uint8_t *) "console moko change.log", 23, EMPTY );
	// std_exec( (uint8_t *) "console moko LICENSE.txt", 24, EMPTY );
	// std_exec( (uint8_t *) "console pwd", 11, EMPTY );
	// 
	// FILE *file = fopen( "test.txt" );
	// if( file ) { log( "OK\n" ); fclose( file ); }
	// else log( "FAILED!\n" );

	// Window Manager initialized.
	return TRUE;
}
