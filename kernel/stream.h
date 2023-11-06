/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_STREAM
	#define	KERNEL_STREAM

	#define	KERNEL_STREAM_limit		(KERNEL_TASK_limit << STD_SHIFT_2)

	#define	KERNEL_STREAM_FLAG_data		0b00000001
	#define	KERNEL_STREAM_FLAG_modified	0b00000010
	#define	KERNEL_STREAM_FLAG_null		0b01000000
	#define	KERNEL_STREAM_FLAG_closed	0b10000000

	struct	KERNEL_STREAM_STRUCTURE {
		uint8_t		*base_address;
		uint64_t	lock;
		uint64_t	start;
		uint64_t	end;
		uint64_t	length_byte;
		uint8_t		semaphore;
		uint8_t		flags;
		uint8_t		meta[ STD_STREAM_META_limit ];
	};

	// create new stream and return properties of it
	struct KERNEL_STREAM_STRUCTURE *kernel_stream( void );

	// transfer stream content to process, inform about length of received stream
	uint64_t kernel_stream_in( uint8_t *target );

	// insert provided string to stream, if unable to return FALSE
	uint8_t kernel_stream_out( uint8_t *string, uint64_t length );
#endif