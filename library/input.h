/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_INPUT
	#define	LIB_INPUT

	struct LIB_INPUT_STRUCTURE {
		uint8_t *history;
	};

	uint64_t lib_input( struct LIB_INPUT_STRUCTURE *input, uint8_t *cache, uint64_t length_max, uint64_t length_current, uint8_t *ctrl_semaphore );

	struct LIB_INPUT_STRUCTURE *lib_input_init( void );
#endif