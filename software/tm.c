/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/color.h"
	#include	"../library/font.h"
	#include	"../library/image.h"
	#include	"../library/integer.h"
	#include	"../library/window.h"
	#include	"../library/ui.h"
	//----------------------------------------------------------------------

#define	WINDOW_WIDTH	(LIB_UI_MARGIN_DEFAULT + 256 + LIB_UI_MARGIN_DEFAULT)
#define	WINDOW_HEIGHT	(LIB_UI_HEADER_HEIGHT + 256 + LIB_UI_MARGIN_DEFAULT)

uint64_t cols = 3;
uint64_t rows = EMPTY;
struct LIB_UI_STRUCTURE_ELEMENT_TABLE_ENTRY *row;

void tm_refresh( void ) {
	struct STD_STRUCTURE_SYSCALL_TASK *task = (struct STD_STRUCTURE_SYSCALL_TASK *) std_task();

	if( rows ) {
		// release old table content
		for( uint64_t y = 0; y < rows; y++ ) {
			for( uint64_t x = 0; x < cols; x++ )
				// name
				if( row[ y ].cell[ x ].name ) free( row[ y ].cell[ x ].name );

			// and cell itself
			free( row[ y ].cell );
		}

		// no entries by default
		rows = EMPTY;

		// truncate table content
		row = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE_ENTRY *) realloc( row, FALSE );
	}

	while( task[ rows ].flags ) {
		row = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE_ENTRY *) realloc( row, sizeof( struct LIB_UI_STRUCTURE_ELEMENT_TABLE_ENTRY ) * (rows + 1) );

		row[ rows ].flag = EMPTY;
		row[ rows ].reserved = EMPTY;
		row[ rows ].cell = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE_CELL *) calloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_TABLE_CELL ) * cols );

		row[ rows ].cell[ 0 ].icon = EMPTY;
		row[ rows ].cell[ 0 ].name = (uint8_t *) calloc( task[ rows ].name_length + TRUE );
		for( uint64_t i = 0; i < task[ rows ].name_length; i++ ) row[ rows ].cell[ 0 ].name[ i ] = task[ rows ].name[ i ];

		row[ rows ].cell[ 2 ].icon = EMPTY;
		row[ rows ].cell[ 2 ].flag = LIB_FONT_FLAG_ALIGN_right;
		row[ rows ].cell[ 2 ].name = (uint8_t *) calloc( lib_integer_digit_count( task[ rows ].page << STD_SHIFT_4, STD_NUMBER_SYSTEM_decimal ) + TRUE );
		lib_integer_to_string( task[ rows ].page << STD_SHIFT_4, STD_NUMBER_SYSTEM_decimal, row[ rows ].cell[ 2 ].name );
	
		rows++;
	}

	std_memory_release( (uintptr_t) task, MACRO_PAGE_ALIGN_UP( sizeof( struct STD_STRUCTURE_SYSCALL_TASK ) * (rows + 1) ) >> STD_SHIFT_PAGE );
}

uint64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// create Palette window
	struct LIB_WINDOW_STRUCTURE *window = lib_window( -1, -1, WINDOW_WIDTH, WINDOW_HEIGHT );	// at default location

	// name window
	lib_window_name( window, (uint8_t *) "Task Manager" );

	// initialize UI for that window
	struct LIB_UI_STRUCTURE *ui = lib_ui( window );

	// add Palette icon
	ui -> icon = lib_image_scale( lib_icon_icon( (uint8_t *) "/var/share/media/icon/default/app/utilities-system-monitor.tga" ), 48, 48, 16, 16 ); for( uint64_t i = 0; i < 16 * 16; i++ ) window -> icon[ i ] = ui -> icon[ i ];

	// control buttons
	lib_ui_add_control( ui, LIB_UI_ELEMENT_CONTROL_TYPE_close );
	lib_ui_add_control( ui, LIB_UI_ELEMENT_CONTROL_TYPE_max );
	lib_ui_add_control( ui, LIB_UI_ELEMENT_CONTROL_TYPE_min );

	//---------------------------------------------------------------------

	struct LIB_UI_STRUCTURE_ELEMENT_TABLE_HEADER *header = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE_HEADER *) calloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_TABLE_HEADER ) * cols );

	uint8_t column_name[] = "Name";
	header[ 0 ].cell.name = (uint8_t *) malloc( sizeof( column_name ) );
	for( uint8_t i = 0; i < sizeof( column_name ); i++ ) header[ 0 ].cell.name[ i ] = column_name[ i ];
	header[ 0 ].cell.flag = LIB_FONT_FLAG_WEIGHT_bold;
	uint8_t column_cpu[] = "CPU";
	header[ 1 ].cell.name = (uint8_t *) malloc( sizeof( column_cpu ) );
	for( uint8_t i = 0; i < sizeof( column_cpu ); i++ ) header[ 1 ].cell.name[ i ] = column_cpu[ i ];
	header[ 1 ].cell.flag = LIB_FONT_FLAG_WEIGHT_bold;
	uint8_t column_memory[] = "Memory";
	header[ 2 ].cell.name = (uint8_t *) malloc( sizeof( column_memory ) );
	for( uint8_t i = 0; i < sizeof( column_memory ); i++ ) header[ 2 ].cell.name[ i ] = column_memory[ i ];
	header[ 2 ].cell.flag = LIB_FONT_FLAG_ALIGN_right | LIB_FONT_FLAG_WEIGHT_bold;

	row = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE_ENTRY *) malloc( FALSE );

	tm_refresh();

	uint64_t ui_table_id = lib_ui_add_table( ui, LIB_UI_MARGIN_DEFAULT, LIB_UI_HEADER_HEIGHT, -1, -1, header, row, cols, rows );

	//---------------------------------------------------------------------

	// application ready
	ui -> window -> flags |= LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_resizable | LIB_WINDOW_FLAG_icon | LIB_WINDOW_FLAG_flush;

	lib_ui_flush( ui );

	uint64_t microtime = std_microtime() + 512;

	// infinite loop
	while( TRUE ) {
		if( microtime < std_microtime() ) {
			tm_refresh();
			lib_ui_update_table( ui, ui_table_id, row, rows );
			lib_ui_flush( ui );

			microtime = std_microtime() + 512;
		}

		// check for events
		uint16_t key = lib_ui_event( ui );

		// exit?
		if( key == STD_ASCII_ESC ) return EMPTY;	// yes

		// release CPU time
		sleep( TRUE );
	}

	// close application
	return EMPTY;
}
