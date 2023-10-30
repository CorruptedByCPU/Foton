/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// show prompt
	print( "\033[92;40m$\033[39;49m " );

	// new prompt loop
	while( TRUE ) {
		// incomming message
		uint8_t data[ STD_IPC_SIZE_byte ];
		if( std_ipc_receive( (uint8_t *) &data ) )
			// debug
			std_stream_out( (uint8_t *) &data[ 1 ], 1 );
	}
}
