/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_TASK
	#define	KERNEL_TASK

	#define	KERNEL_TASK_limit	(STD_PAGE_byte / sizeof( struct KERNEL_TASK_STRUCTURE ))	// hard limit

	#define	KERNEL_TASK_FLAG_active		0b0000000000000001
	#define	KERNEL_TASK_FLAG_exec		0b0000000000000010
	#define	KERNEL_TASK_FLAG_close		0b0000000000000100
	#define	KERNEL_TASK_FLAG_service	0b0000000000001000
	#define	KERNEL_TASK_FLAG_init		0b0100000000000000
	#define	KERNEL_TASK_FLAG_secured	0b1000000000000000

	#define	KERNEL_TASK_EFLAGS_cf		0b000000000000000000000001
	#define	KERNEL_TASK_EFLAGS_zf		0b000000000000000001000000
	#define	KERNEL_TASK_EFLAGS_if		0b000000000000001000000000
	#define	KERNEL_TASK_EFLAGS_df		0b000000000000010000000000
	#define	KERNEL_TASK_EFLAGS_default	KERNEL_TASK_EFLAGS_if

	#define	KERNEL_TASK_NAME_limit		255

	#define	KERNEL_TASK_STACK_pointer	KERNEL_LIBRARY_base_address

	struct	KERNEL_TASK_STRUCTURE {
		uintptr_t		cr3;
		uintptr_t		rsp;
		int64_t			pid;
		uint64_t		sleep;
		uint64_t		storage;
		uint64_t		page;	// amount of pages assigned to process
		uint64_t		stack;	// size of stack in Pages
		volatile uint16_t	flags;
		uint8_t			length;
		uint8_t			name[ KERNEL_TASK_NAME_limit ];
	};

	// external routine (assembly language)
	extern void kernel_task_entry( void );

	// returns current task PID number
	int64_t kernel_task_pid();
#endif