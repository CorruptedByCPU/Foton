/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define	KERNEL_name		"Foton"
#define	KERNEL_version		"0"
#define	KERNEL_revision		"546"
#define	KERNEL_architecture	"x86_64"
#define	KERNEL_language		"C"

#define	KERNEL_BASE_address	0xFFFFFFFF80000000	// in higher half

#define	KERNEL_MEMORY_mirror	0xFFFF800000000000

#define	KERNEL_STACK_LIMIT_page	2
#define	KERNEL_STACK_address	(uintptr_t) -(KERNEL_STACK_LIMIT_page << STD_SHIFT_PAGE)
#define	KERNEL_STACK_pointer	0xFFFFFFFFFFFFF000

#ifndef	KERNEL_GDT
	#include	"gdt.h"
#endif
#ifndef	KERNEL_TSS
	#include	"tss.h"
#endif

struct KERNEL {
	volatile struct KERNEL_STRUCTURE_APIC		*apic_base_address;
	uint64_t									apic_id_last;

	volatile uint64_t				cpu_limit;

	uint8_t						device_mouse_status;
	uint16_t					device_mouse_x;
	uint16_t					device_mouse_y;
	uint16_t					device_mouse_z;
	void						(*device_mouse)( void );
	uint16_t					device_keyboard[ 8 ];

	uint32_t					*framebuffer_base_address;
	uint16_t					framebuffer_width_pixel;
	uint16_t					framebuffer_height_pixel;
	uint32_t					framebuffer_pitch_byte;
	uint64_t					framebuffer_pid;

	struct KERNEL_STRUCTURE_GDT_HEADER		gdt_header;

	struct KERNEL_STRUCTURE_IDT_HEADER		idt_header;
	void						(*idt_attach)( uint8_t irq, uint16_t type, uintptr_t address );

	volatile struct KERNEL_STRUCTURE_IO_APIC	*io_apic_base_address;
	void						(*io_apic_attach)( uint8_t line, uint32_t io_apic_register );

	struct STD_STRUCTURE_IPC			*ipc_base_address;
	uint8_t						ipc_lock;

	struct KERNEL_STRUCTURE_LIBRARY			*library_base_address;
	uint64_t					library_limit;
	uint32_t					*library_memory_address;

	uint32_t					*memory_base_address;
	uintptr_t					(*memory_alloc)( uint64_t n );
	uintptr_t					(*memory_alloc_low)( uint64_t n );
	void						(*memory_clean)( uint64_t *address, uint64_t n );
	void						(*memory_release)( uintptr_t address, uint64_t n );

	uint64_t					(*module_thread)( uintptr_t function, uint8_t *name, uint64_t length );

	uint64_t					*page_base_address;
	uint64_t					page_available;
	uint64_t					page_limit;
	uint64_t					page_total;
	uint64_t					page_shared;
	uint64_t					page_structure;
	void						(*page_deconstruct)( uint64_t *pml4, uint8_t type );
	uint8_t						(*page_map)( uint64_t *pml4, uintptr_t source, uintptr_t target, uint64_t n, uint16_t flags );

	void						(*serial)( uint8_t *string, ... );

	struct KERNEL_STRUCTURE_STORAGE			*storage_base_address;
	uint64_t					storage_limit;
	uint8_t						storage_lock;

	uint64_t					*task_ap_address;
	struct KERNEL_STRUCTURE_TASK			*task_base_address;
	struct KERNEL_STRUCTURE_TASK			*(*task_by_id)( uint64_t pid );
	uint64_t					task_limit;
	uint8_t						task_lock;
	uint8_t						task_lock_ap;
	uint64_t					task_id;

	// struct LIB_TERMINAL_STRUCTURE			terminal;

	uint32_t					time_hz;
	uint64_t					time_units;
	void						(*time_sleep)( uint64_t u );

	struct KERNEL_STRUCTURE_TSS			tss;

	struct KERNEL_STRUCTURE_VFS			*vfs;
	struct KERNEL_STRUCTURE_VFS_SOCKET		*vfs_base_address;
	uint64_t					vfs_limit;
	uint8_t						vfs_lock;
};
