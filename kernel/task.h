/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_TASK
	#define	KERNEL_TASK

	#define	KERNEL_TASK_limit	(STD_PAGE_byte / sizeof( struct KERNEL_TASK_STRUCTURE ))	// hard limit

	#define	KERNEL_TASK_FLAG_active		0b0000000000000001
	#define	KERNEL_TASK_FLAG_exec		0b0000000000000010
	#define	KERNEL_TASK_FLAG_secured	0b1000000000000000

	struct	KERNEL_TASK_STRUCTURE {
		uintptr_t		cr3;
		uintptr_t		rsp;
		uint64_t		pid;
		uint64_t		sleep;
		volatile uint16_t	flags;
		uint8_t			length;
		uint8_t			name[ 32 ];	// for now its enough
	};

	// external routine (assembly language)
	extern void kernel_task_entry( void );
#endif