/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define	KERNEL_name		"Foton"
#define	KERNEL_version		"0"
#define	KERNEL_revision		"228"
#define	KERNEL_architecture	"x86_64"
#define	KERNEL_language		"C"

#define	KERNEL_BASE_address	0xFFFFFFFF80000000	// higher half

#define	KERNEL_STACK_page	2
#define	KERNEL_STACK_address	-(KERNEL_STACK_page << STD_SHIFT_PAGE)	// minimal size
#define	KERNEL_STACK_pointer	0xFFFFFFFFFFFFF000

#ifndef	KERNEL_GDT
	#include		"./gdt.h"
#endif

#ifndef	KERNEL_TSS
	#include		"./tss.h"
#endif

#ifndef	KERNEL_IDT
	#include		"./idt.h"
#endif

struct KERNEL {
	// variables of Kernel management functions
	volatile uint64_t	cpu_count;

	// variables of Input devices
	uint8_t		device_mouse_status;
	uint16_t	device_mouse_x;
	uint16_t	device_mouse_y;
	uint16_t	device_keyboard[ 8 ];	// cache size of 8 keys

	// variables of Framebuffer functions
	uint32_t	*framebuffer_base_address;
	uint16_t	framebuffer_width_pixel;
	uint16_t	framebuffer_height_pixel;
	uint32_t	framebuffer_pitch_byte;
	int64_t		framebuffer_pid;

	// variables of GDT management functions
	struct KERNEL_GDT_STRUCTURE_HEADER			gdt_header;

	// variables of HPET management functions
	volatile struct KERNEL_HPET_STRUCTURE_REGISTER		*hpet_base_address;
	// volatile uint64_t	hpet_miliseconds;
	uint8_t		hpet_timers;

	// variables of Time management functions
	volatile uint64_t	time_unit;
	// functions of Time management
	void							(*time_sleep)( uint64_t t );	// miliseconds

	// variables of IDT management functions
	struct KERNEL_IDT_STRUCTURE_HEADER			idt_header;
	// functions of IDT management
	void							(*idt_mount)( uint8_t id, uint16_t type, uintptr_t address );

	// variables of I/O APIC management functions
	volatile struct KERNEL_IO_APIC_STRUCTURE_REGISTER	*io_apic_base_address;
	uint32_t	io_apic_irq_lines;
	uint8_t		io_apic_semaphore;
	// functions of I/O APIC management
	uint8_t							(*io_apic_line_acquire)( void );
	void							(*io_apic_connect)( uint8_t line, uint32_t io_apic_register );

	// variables of IPC management functions
	struct STD_IPC_STRUCTURE				*ipc_base_address;
	uint8_t		ipc_semaphore;

	// variables of Log management functions
	void							(*log)( uint8_t *string, ... );

	// variables of APIC management functions
	volatile struct KERNEL_LAPIC_STRUCTURE			*lapic_base_address;
	uint64_t	lapic_id_highest;
	// functions of APIC management
	void							(*lapic_accept)( void );

	// variables of Library management functions
	struct KERNEL_LIBRARY_STRUCTURE				*library_base_address;
	uint32_t	*library_map_address;

	// variables of Memory management functions
	uint32_t	*memory_base_address;
	uint8_t		memory_semaphore;
	// functions of Memory management
	uintptr_t						(*memory_alloc)( uint64_t N );
	uintptr_t						(*memory_alloc_page)( void );
	void							(*memory_release)( uintptr_t address, uint64_t N );
	void							(*memory_release_page)( uintptr_t address );

	// variables of Modules functions
	uint32_t	*module_base_address;

	// variables of Page management functions
	uint64_t	*page_base_address;
	uint64_t	page_total;
	uint64_t	page_available;
	uint64_t	page_limit;
	// functions of Page management
	void							(*page_clean)( uintptr_t address, uint64_t n );
	void							(*page_deconstruct)( uintptr_t *pml4 );

	// variables of Storage management functions
	struct KERNEL_STORAGE_STRUCTURE				*storage_base_address;
	uint64_t	storage_root;
	uint8_t		storage_semaphore;

	// variables of Stream management functions
	struct KERNEL_STREAM_STRUCTURE	*stream_base_address;
	uint8_t		stream_semaphore;
	// functions of Stream management
	void							(*stream_release)( struct KERNEL_STREAM_STRUCTURE *stream );

	// variables of Task management functions
	struct KERNEL_TASK_STRUCTURE				*task_base_address;
	struct KERNEL_TASK_STRUCTURE				**task_cpu_address;	// contains pointers to task inside queue by specified CPU id
	uint8_t		task_cpu_semaphore;
	uint8_t		task_add_semaphore;
	uint64_t	task_limit;
	uint64_t	task_count;
	int64_t		task_id;
	// functions of Task management
	struct KERNEL_TASK_STRUCTURE 				*(*task_active)( void );
	int64_t							(*task_pid)( void );
	struct KERNEL_TASK_STRUCTURE				*(*task_by_id)( int64_t pid );

	// variables of TSS management functions
	struct KERNEL_TSS_STRUCTURE				tss_table;

	// variables of VFS management functions
	struct KERNEL_VFS_STRUCTURE			*vfs_base_address;
	uint64_t	vfs_root;
	uint8_t		vfs_semaphore;
};
