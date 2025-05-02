/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../../library/color.h"
	#include	"../../library/font.h"
	#include	"../../library/image.h"
	#include	"../../library/integer.h"
	//----------------------------------------------------------------------

#define	TIWYN_DESKTOP_PANEL_COLOR_default	0xFF101010
#define	TIWYN_DESKTOP_PANEL_CLOCK_WIDTH_pixel	50
#define	TIWYN_DESKTOP_PANEL_HEIGHT_pixel	22

void tiwyn_desktop_panel_clock( void );

uint64_t tiwyn_desktop_pid = EMPTY;

struct TIWYN_STRUCTURE_OBJECT	*tiwyn_desktop_object_workbench;
struct TIWYN_STRUCTURE_OBJECT	*tiwyn_desktop_object_panel;

uint64_t tiwyn_clock_state = EMPTY;

uint64_t tiwyn_desktop_panel_task_limit_old = EMPTY;

void tiwyn_desktop_panel( void ) {
	// check clock status
	tiwyn_desktop_panel_clock();

	// properties of task list area
	uint32_t *task_pixel = (uint32_t *) ((uintptr_t) tiwyn_desktop_object_panel -> descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR ));

	// empty panel by default
	uint64_t task_limit = EMPTY;

	// nothing changed
	uint8_t task_altered = FALSE;

	// count amount of object to show
	for( uint64_t i = 0; i < tiwyn -> list_limit; i++ ) {
		// it's not our object? and visible?
		if( tiwyn -> list[ i ] -> pid != tiwyn_desktop_pid && tiwyn -> list[ i ] -> descriptor -> flags & LIB_WINDOW_FLAG_visible ) task_limit++;	// yes, count it
		
		// altered object?
		if( tiwyn -> list[ i ] -> descriptor -> flags & LIB_WINDOW_FLAG_altered ) task_altered = TRUE;
	}

	// nothing to show?
	if( task_limit == tiwyn_desktop_panel_task_limit_old && ! task_altered ) return;	// done

	// update task limit
	tiwyn_desktop_panel_task_limit_old = task_limit;

	// clean'up panel with default color
	for( uint16_t y = 0; y < tiwyn_desktop_object_panel -> height; y++ )
		for( uint16_t x = TIWYN_DESKTOP_PANEL_HEIGHT_pixel; x < tiwyn_desktop_object_panel -> width - TIWYN_DESKTOP_PANEL_CLOCK_WIDTH_pixel; x++ )
			task_pixel[ (y * tiwyn_desktop_object_panel -> width) + x ] = TIWYN_DESKTOP_PANEL_COLOR_default;

	// entry width
	uint64_t task_width_pixel = (tiwyn_desktop_object_panel -> width - (TIWYN_DESKTOP_PANEL_HEIGHT_pixel + TIWYN_DESKTOP_PANEL_CLOCK_WIDTH_pixel)) / task_limit;

	// move pointer to first entry of panel task list
	task_pixel += TIWYN_DESKTOP_PANEL_HEIGHT_pixel;

	// first entry position
	uint16_t x = 0;

	// count amount of object to show
	for( uint64_t i = 0; i < tiwyn -> list_limit; i++ ) {	
		// it's our object? or invisible?
		if( tiwyn -> list[ i ] -> pid == tiwyn_desktop_pid || ! (tiwyn -> list[ i ] -> descriptor -> flags & LIB_WINDOW_FLAG_visible) ) continue;	// yes, omit

		// show entry name
		lib_font( LIB_FONT_FAMILY_ROBOTO, tiwyn -> list[ i ] -> descriptor -> name, tiwyn -> list[ i ] -> descriptor -> name_length, 0xFFFFFFFF, task_pixel + (4 * tiwyn_desktop_object_panel -> width) + 4, tiwyn_desktop_object_panel -> width, LIB_FONT_ALIGN_left );

		// object parsed
		tiwyn -> list[ i ] -> descriptor -> flags &= ~LIB_WINDOW_FLAG_altered;
	}	
}

void tiwyn_desktop_panel_clock( void ) {
	// check current date and time
	uint64_t time = std_time();

	// it's different than previous?
	if( time == tiwyn_clock_state ) return;	// no

	// preserve current date and time
	tiwyn_clock_state = time;

	// properties of current time
	uint8_t hours = (uint8_t) (time >> 16);
	uint8_t minutes = (uint8_t) (time >> 8);
	uint8_t seconds = (uint8_t) (time);

	// clock template
	uint8_t clock_string[ 5 ] = "00 00";

	// fill clock area with default background color
	uint32_t *panel_pixel = (uint32_t *) ((uintptr_t) tiwyn_desktop_object_panel -> descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR ));
	uint32_t *clock_pixel = (uint32_t *) ((uintptr_t) tiwyn_desktop_object_panel -> descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR )) + (tiwyn_desktop_object_panel -> width - TIWYN_DESKTOP_PANEL_CLOCK_WIDTH_pixel);
	for( uint16_t y = 0; y < tiwyn_desktop_object_panel -> height; y++ )
		for( uint16_t x = 0; x < TIWYN_DESKTOP_PANEL_CLOCK_WIDTH_pixel; x++ )
			clock_pixel[ (y * tiwyn_desktop_object_panel -> width) + x ] = TIWYN_DESKTOP_PANEL_COLOR_default;

	// hour
	if( hours < 10 ) { clock_string[ 0 ] = STD_ASCII_SPACE; lib_integer_to_string( hours, 10, (uint8_t *) &clock_string[ 1 ] ); }
	else lib_integer_to_string( hours, 10, (uint8_t *) &clock_string );

	// minute
	if( minutes < 10 ) { clock_string[ 3 ] = STD_ASCII_DIGIT_0; lib_integer_to_string( minutes, 10, (uint8_t *) &clock_string[ 4 ] ); }
	else lib_integer_to_string( minutes, 10, (uint8_t *) &clock_string[ 3 ] );

	// show clock on taskbar
	lib_font( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) &clock_string, sizeof( clock_string ), STD_COLOR_WHITE, clock_pixel + ((((TIWYN_DESKTOP_PANEL_HEIGHT_pixel - LIB_FONT_HEIGHT_pixel) / 2) + TRUE) * tiwyn_desktop_object_panel -> width) + (TIWYN_DESKTOP_PANEL_CLOCK_WIDTH_pixel >> STD_SHIFT_2), tiwyn_desktop_object_panel -> width, LIB_FONT_ALIGN_center );

	// colon animation
	if( seconds % 2 ) lib_font( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) ":", TRUE, STD_COLOR_GRAY, clock_pixel + ((((TIWYN_DESKTOP_PANEL_HEIGHT_pixel - LIB_FONT_HEIGHT_pixel) / 2)) * tiwyn_desktop_object_panel -> width) + (TIWYN_DESKTOP_PANEL_CLOCK_WIDTH_pixel >> STD_SHIFT_2), tiwyn_desktop_object_panel -> width, LIB_FONT_ALIGN_center );
	else lib_font( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) ":", TRUE, STD_COLOR_WHITE, clock_pixel + ((((TIWYN_DESKTOP_PANEL_HEIGHT_pixel - LIB_FONT_HEIGHT_pixel) / 2)) * tiwyn_desktop_object_panel -> width) + (TIWYN_DESKTOP_PANEL_CLOCK_WIDTH_pixel >> STD_SHIFT_2), tiwyn_desktop_object_panel -> width, LIB_FONT_ALIGN_center );

	// update taskbar content on screen
	tiwyn_desktop_object_panel -> descriptor -> flags |= LIB_WINDOW_FLAG_flush;
}

void tiwyn_desktop_init( void ) {
	// acquire our PID number
	tiwyn_desktop_pid = std_pid();

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
	tiwyn_desktop_object_workbench = tiwyn_object_create( 0, 0, tiwyn -> canvas.width, tiwyn -> canvas.height );

	// mark object as own
	tiwyn_desktop_object_workbench -> pid = tiwyn_desktop_pid;

	// properties of workbench area content
	uint32_t *workbench_pixel = (uint32_t *) ((uintptr_t) tiwyn_desktop_object_workbench -> descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR ));

	// if default wallpaper file found
	if( workbench_image ) {
		// convert image to RGBA
		uint32_t *tmp_workbench_image = (uint32_t *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( tiwyn_desktop_object_workbench -> limit ) >> STD_SHIFT_PAGE );
		lib_image_tga_parse( (uint8_t *) workbench_image, tmp_workbench_image, workbench_file -> byte );

		// copy scaled image content to workbench object
		float x_scale_factor = (float) ((float) workbench_image -> width / (float) tiwyn_desktop_object_workbench -> width);
		float y_scale_factor = (float) ((float) workbench_image -> height / (float) tiwyn_desktop_object_workbench -> height);
		for( uint16_t y = 0; y < tiwyn_desktop_object_workbench -> height; y++ )
			for( uint16_t x = 0; x < tiwyn_desktop_object_workbench -> width; x++ )
				workbench_pixel[ (y * tiwyn_desktop_object_workbench -> width) + x ] = tmp_workbench_image[ (uint64_t) (((uint64_t) (y_scale_factor * y) * workbench_image -> width) + (uint64_t) (x * x_scale_factor)) ];

		// release temporary image
		std_memory_release( (uintptr_t) tmp_workbench_image, MACRO_PAGE_ALIGN_UP( tiwyn_desktop_object_workbench -> limit ) >> STD_SHIFT_PAGE );

		// release file content
		std_memory_release( (uintptr_t) workbench_image, MACRO_PAGE_ALIGN_UP( workbench_file -> byte ) >> STD_SHIFT_PAGE );

		// close file
		fclose( workbench_file );
	} else
		// fill workbench with default color
		for( uint16_t y = 0; y < tiwyn_desktop_object_workbench -> height; y++ )
			for( uint16_t x = 0; x < tiwyn_desktop_object_workbench -> width; x++ )
				workbench_pixel[ (y * tiwyn_desktop_object_workbench -> width) + x ] = TIWYN_DESKTOP_PANEL_COLOR_default;

	// object content ready for display
	tiwyn_desktop_object_workbench -> descriptor -> flags = LIB_WINDOW_FLAG_fixed_z | LIB_WINDOW_FLAG_fixed_xy | LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_flush;

	//----------------------------------------------------------------------

	// create panel object
	tiwyn_desktop_object_panel = tiwyn_object_create( 0, tiwyn -> canvas.height - TIWYN_DESKTOP_PANEL_HEIGHT_pixel, tiwyn -> canvas.width, TIWYN_DESKTOP_PANEL_HEIGHT_pixel );

	// mark object as own
	tiwyn_desktop_object_panel -> pid = tiwyn_desktop_pid;

	// properties of panel area content
	uint32_t *panel_pixel = (uint32_t *) ((uintptr_t) tiwyn_desktop_object_panel -> descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR ));

	// fill panel with default color
	for( uint16_t y = 0; y < tiwyn_desktop_object_panel -> height; y++ )
		for( uint16_t x = 0; x < tiwyn_desktop_object_panel -> width; x++ )
			panel_pixel[ (y * tiwyn_desktop_object_panel -> width) + x ] = TIWYN_DESKTOP_PANEL_COLOR_default;

	// show menu button on panel
	uint8_t test[ 3 ] = "|||";
	lib_font( LIB_FONT_FAMILY_ROBOTO, (uint8_t *) &test, sizeof( test ), STD_COLOR_WHITE, panel_pixel + (((tiwyn_desktop_object_panel -> height - LIB_FONT_HEIGHT_pixel) / 2) * tiwyn_desktop_object_panel -> width) + (tiwyn_desktop_object_panel -> height >> STD_SHIFT_2), tiwyn_desktop_object_panel -> width, LIB_FONT_ALIGN_center );

	// object content ready for display
	tiwyn_desktop_object_panel -> descriptor -> flags = LIB_WINDOW_FLAG_panel | LIB_WINDOW_FLAG_fixed_z | LIB_WINDOW_FLAG_fixed_xy | LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_flush;

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
		tiwyn -> cursor = tiwyn_object_create( tiwyn_desktop_object_workbench -> width >> STD_SHIFT_2, tiwyn_desktop_object_workbench -> height >> STD_SHIFT_2, cursor_image -> width, cursor_image -> height );
	} else
		// create default object
		tiwyn -> cursor = tiwyn_object_create( tiwyn_desktop_object_workbench -> width >> STD_SHIFT_2, tiwyn_desktop_object_workbench -> height >> STD_SHIFT_2, 16, 32 );

	// mark object as own
	tiwyn -> cursor -> pid = tiwyn_desktop_pid;

	// properties of cursor area content
	uint32_t *cursor_pixel = (uint32_t *) ((uintptr_t) tiwyn -> cursor -> descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR ));

	// fill cursor with default color
	for( uint16_t y = 0; y < tiwyn -> cursor -> height; y++ )
		for( uint16_t x = 0; x < tiwyn -> cursor -> width; x++ )
			cursor_pixel[ (y * tiwyn -> cursor -> width) + x ] = STD_COLOR_WHITE;

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
	tiwyn -> cursor -> descriptor -> flags = LIB_WINDOW_FLAG_cursor | LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_flush;
}

void tiwyn_desktop( void ) {
	// create default windows
	tiwyn_desktop_init();

	// enable window management
	tiwyn -> enable = TRUE;

	//----------------------------------------------------------------------

	// debug
	std_exec( (uint8_t *) "so", 2, EMPTY, TRUE );

	// hold the door
	while( TRUE ) {
		// check panel occuring events
		tiwyn_desktop_panel();

		// release CPU
		sleep( TRUE );
	}
}
