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
	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#include	"./console/config.h"
	//----------------------------------------------------------------------
	// variables, routines, procedures
	//----------------------------------------------------------------------
	// #include	"./console/data.c"
	// #include	"./console/init.c"

// terminal properties
struct LIB_TERMINAL_STRUCTURE terminal;

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// obtain information about kernel framebuffer
	struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER kernel_framebuffer;
	std_framebuffer( &kernel_framebuffer );

	// remember Graphical User Interface PID
	int64_t gui_pid = kernel_framebuffer.pid;

	// allocate gui data container
	uint8_t gui_data[ STD_IPC_SIZE_byte ];

	// prepeare new window request
	struct STD_WINDOW_STRUCTURE_REQUEST *gui_request = (struct STD_WINDOW_STRUCTURE_REQUEST *) &gui_data;
	struct STD_WINDOW_STRUCTURE_ANSWER *gui_answer = EMPTY;	// answer will be in here

	//----------------------------------------------------------------------

	// wallpaper window properties
	gui_request -> x = 0;
	gui_request -> y = 0;
	gui_request -> width = CONSOLE_WINDOW_WIDTH_pixel;
	gui_request -> height = CONSOLE_WINDOW_HEIGHT_pixel;

	// send request to Graphical User Interface
	std_ipc_send( gui_pid, (uint8_t *) gui_request );

	// wait for answer
	while( ! std_ipc_receive( (uint8_t *) gui_data ) );

	// window assigned?
	gui_answer = (struct STD_WINDOW_STRUCTURE_ANSWER *) &gui_data;
	if( ! gui_answer -> descriptor ) return FALSE;	// no

	// properties of console window
	struct WM_STRUCTURE_DESCRIPTOR *console_window = (struct WM_STRUCTURE_DESCRIPTOR *) gui_answer -> descriptor;

terminal.width			= CONSOLE_WINDOW_WIDTH_pixel;
terminal.height			= CONSOLE_WINDOW_HEIGHT_pixel;
terminal.base_address		= (uint32_t *) ((uintptr_t) console_window + sizeof( struct WM_STRUCTURE_DESCRIPTOR ));
terminal.scanline_pixel		= CONSOLE_WINDOW_WIDTH_pixel;
terminal.color_foreground	= STD_COLOR_BLACK_light;
terminal.color_background	= STD_COLOR_WHITE;
lib_terminal( &terminal );
lib_terminal_printf( &terminal, (uint8_t *) "\n  Window created by console." );
console_window -> flags |= WM_OBJECT_FLAG_visible | WM_OBJECT_FLAG_flush;

	// hold the door
	while( TRUE );
}