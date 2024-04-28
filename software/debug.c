/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#include	"../library/font.h"

#define	WIDTH	320
#define	HEIGHT	200

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER kernel_framebuffer;
	std_framebuffer( (struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER *) &kernel_framebuffer );
	int64_t wm_pid = kernel_framebuffer.pid;
	uint8_t wm_data[ STD_IPC_SIZE_byte ];
	struct STD_IPC_STRUCTURE_WINDOW *request = (struct STD_IPC_STRUCTURE_WINDOW *) &wm_data;
	struct STD_IPC_STRUCTURE_WINDOW_DESCRIPTOR *answer = EMPTY;
	request -> ipc.type = STD_IPC_TYPE_event;
	request -> width = WIDTH;
	request -> height = HEIGHT;
	request -> x = 0;
	request -> y = 0;
	std_ipc_send( wm_pid, (uint8_t *) request );
	uint64_t timeout = std_microtime() + 1024;
	while( ! std_ipc_receive( (uint8_t *) wm_data ) ) if( timeout < std_microtime() ) { print( "Could not connect to Window Manager." ); exit(); }
	answer = (struct STD_IPC_STRUCTURE_WINDOW_DESCRIPTOR *) &wm_data;
	if( ! answer -> descriptor ) { print( "Window Manager denied request." ); exit(); }
	struct STD_WINDOW_STRUCTURE_DESCRIPTOR *window = (struct STD_WINDOW_STRUCTURE_DESCRIPTOR *) answer -> descriptor;

	struct STD_NETWORK_STRUCTURE_INTERFACE eth0;
	std_network_interface( (struct STD_NETWORK_STRUCTURE_INTERFACE *) &eth0 );

	uint32_t *pixel = (uint32_t *) ((uintptr_t) window + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR ));
	for( uint16_t y = 0; y < HEIGHT; y++ ) for( uint16_t x = 0; x < WIDTH; x++ ) pixel[ (y * WIDTH) + x ] = EMPTY;

	uint64_t offset = lib_font( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) "rx_frames: ", 11, 0xFFFFFFF, pixel, WIDTH, EMPTY );
	lib_font_value( LIB_FONT_FAMILY_ROBOTO_MONO, eth0.rx_frame, 10, 0xFFFFFFFF, pixel + offset, WIDTH, EMPTY );
	window -> flags = STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;

	while( TRUE ) {
		std_sleep( TRUE );

		struct STD_NETWORK_STRUCTURE_INTERFACE tmp;
		std_network_interface( (struct STD_NETWORK_STRUCTURE_INTERFACE *) &tmp );
		if( eth0.rx_frame == tmp.rx_frame ) continue;

		for( uint16_t y = 0; y < HEIGHT; y++ ) for( uint16_t x = 0; x < WIDTH; x++ ) pixel[ (y * WIDTH) + x ] = EMPTY;
		uint64_t offset = lib_font( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) "rx_frames: ", 11, 0xFFFFFFF, pixel, WIDTH, EMPTY );
		lib_font_value( LIB_FONT_FAMILY_ROBOTO_MONO, eth0.rx_frame, 10, 0xFFFFFFFF, pixel + offset, WIDTH, EMPTY );
		window -> flags |= STD_WINDOW_FLAG_flush;

		eth0 = tmp;
	}

	return 0;
}