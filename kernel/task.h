/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_TASK
	#define	KERNEL_TASK

	#define	KERNEL_TASK_EFLAGS_cf		0x0001	// 0b000000000000000000000001
	#define	KERNEL_TASK_EFLAGS_df		0x0400	// 0b000000000000010000000000
	#define	KERNEL_TASK_EFLAGS_if		0x0200	// 0b000000000000001000000000
	#define	KERNEL_TASK_EFLAGS_zf		0x0040	// 0b000000000000000001000000
	#define	KERNEL_TASK_EFLAGS_default	KERNEL_TASK_EFLAGS_if

	#define	KERNEL_TASK_FLAG_active		STD_TASK_FLAG_active
	#define	KERNEL_TASK_FLAG_exec		STD_TASK_FLAG_exec
	#define	KERNEL_TASK_FLAG_close		STD_TASK_FLAG_close
	#define	KERNEL_TASK_FLAG_module		STD_TASK_FLAG_module
	#define	KERNEL_TASK_FLAG_thread		STD_TASK_FLAG_thread
	#define	KERNEL_TASK_FLAG_sleep		STD_TASK_FLAG_sleep
	#define	KERNEL_TASK_FLAG_init		STD_TASK_FLAG_init
	#define	KERNEL_TASK_FLAG_secured	STD_TASK_FLAG_secured

	#define	KERNEL_TASK_limit		(STD_PAGE_byte / sizeof( struct KERNEL_STRUCTURE_TASK ))

	#define	KERNEL_TASK_NAME_limit		(STD_PAGE_byte - TRUE)

	struct	KERNEL_STRUCTURE_TASK {
		uint64_t 			*cr3;
		uintptr_t			rsp;
		uint64_t			pid;
		uint64_t			parent;
		volatile uint16_t		flags;
		uint16_t			name_limit;
		uint8_t				*name;
		uint64_t			storage;
		uint64_t			directory;
		uint8_t				page_type;
	};

	// external routine (assembly language)
	extern void kernel_task( void );

	// properties of created task
	struct KERNEL_STRUCTURE_TASK *kernel_task_add( uint8_t *name, uint16_t limit );

	// current task properties
	struct KERNEL_STRUCTURE_TASK *kernel_task_current( void );
#endif
