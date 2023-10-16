/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/font.h"
	#include	"../library/terminal.h"
	//----------------------------------------------------------------------
	// variables, structures, definitions of Graphical User Interface
	//----------------------------------------------------------------------
	#include	"./gui/config.h"
	#include	"./wm/config.h"

#define	CONSOLE_WINDOW_WIDTH_char	40
#define	CONSOLE_WINDOW_HEIGHT_char	12
#define	CONSOLE_WINDOW_WIDTH_pixel	CONSOLE_WINDOW_WIDTH_char * LIB_FONT_WIDTH_pixel
#define	CONSOLE_WINDOW_HEIGHT_pixel	CONSOLE_WINDOW_HEIGHT_char * LIB_FONT_HEIGHT_pixel
struct LIB_TERMINAL_STRUCTURE terminal;

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER kernel_framebuffer;
	std_framebuffer( &kernel_framebuffer );
	int64_t gui_pid = kernel_framebuffer.pid;
	uint8_t gui_data[ STD_IPC_SIZE_byte ];
	struct STD_WINDOW_STRUCTURE_REQUEST *gui_request = (struct STD_WINDOW_STRUCTURE_REQUEST *) &gui_data;
	struct STD_WINDOW_STRUCTURE_ANSWER *gui_answer = EMPTY;
	gui_request -> x	= CONSOLE_WINDOW_WIDTH_pixel >> STD_SHIFT_2;
	gui_request -> y	= CONSOLE_WINDOW_HEIGHT_pixel >> STD_SHIFT_2;
	gui_request -> width	= CONSOLE_WINDOW_WIDTH_pixel;
	gui_request -> height	= CONSOLE_WINDOW_HEIGHT_pixel;
	std_ipc_send( gui_pid, (uint8_t *) gui_request );
	while( ! std_ipc_receive( (uint8_t *) gui_data ) );
	gui_answer = (struct STD_WINDOW_STRUCTURE_ANSWER *) &gui_data;
	if( ! gui_answer -> descriptor ) return FALSE;
	struct WM_STRUCTURE_DESCRIPTOR *console_window = (struct WM_STRUCTURE_DESCRIPTOR *) gui_answer -> descriptor;
	terminal.width			= CONSOLE_WINDOW_WIDTH_pixel;
	terminal.height			= CONSOLE_WINDOW_HEIGHT_pixel;
	terminal.base_address		= (uint32_t *) ((uintptr_t) console_window + sizeof( struct WM_STRUCTURE_DESCRIPTOR ));
	terminal.scanline_pixel		= CONSOLE_WINDOW_WIDTH_pixel;
	terminal.color_foreground	= STD_COLOR_WHITE;
	terminal.color_background	= STD_COLOR_RED_light;
	lib_terminal( &terminal );
	lib_terminal_printf( &terminal, (uint8_t *) "\n  Window created by Test." );
	console_window -> flags |= WM_OBJECT_FLAG_visible | WM_OBJECT_FLAG_flush;
	while( TRUE );
}