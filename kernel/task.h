/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_TASK
	#define	KERNEL_TASK

	#define	KERNEL_TASK_IRQ_software	0x40

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

	#define	KERNEL_TASK_TYPE_KERNEL		KERNEL_PAGE_TYPE_KERNEL
	#define	KERNEL_TASK_TYPE_MODULE		KERNEL_PAGE_TYPE_MODULE
	#define	KERNEL_TASK_TYPE_PROCESS	KERNEL_PAGE_TYPE_PROCESS
	#define	KERNEL_TASK_TYPE_THREAD		KERNEL_PAGE_TYPE_THREAD

	#define	KERNEL_TASK_limit		256

	#define	KERNEL_TASK_NAME_limit		(STD_PAGE_byte - TRUE)

	#define	KERNEL_TASK_STACK_pointer	KERNEL_LIBRARY_base_address
	#define	KERNEL_TASK_STACK_limit		STD_PAGE_byte

	struct	KERNEL_STRUCTURE_TASK {
		uint64_t 			*cr3;
		uintptr_t			rsp;
		uint8_t				type;
		uint64_t			pid;
		uint64_t			parent;
		volatile uint16_t		flags;
		uint16_t			name_limit;
		uint8_t				*name;
		uint64_t			storage;
		uint64_t			directory;
		uint64_t			stack_page;
		uint32_t			*memory;
		uint64_t			page;
	};

	// external routine (assembly language)
	extern void kernel_task( void );

	// properties of created task
	struct KERNEL_STRUCTURE_TASK *kernel_task_add( uint8_t *name, uint16_t limit );

	// properties of task identified by ID
	struct KERNEL_STRUCTURE_TASK *kernel_task_by_id( uint64_t pid );

	// current task properties
	struct KERNEL_STRUCTURE_TASK *kernel_task_current( void );
#endif
