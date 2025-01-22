/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define	KERNEL_name		"Foton"
#define	KERNEL_version		"0"
#define	KERNEL_revision		"502"
#define	KERNEL_architecture	"x86_64"
#define	KERNEL_language		"C"

#define	KERNEL_BASE_address	0xFFFFFFFF80000000	// higher half

#define	KERNEL_STACK_page	2
#define	KERNEL_STACK_address	(uint64_t)  -(KERNEL_STACK_page << STD_SHIFT_PAGE)	// minimal size
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

// #ifndef	LIB_TERMINAL
// 	#include		"../library/terminal.h"
// #endif

struct KERNEL {
	// variables of Kernel management functions
	volatile uint64_t					cpu_count;

	// variables of Input devices
	uint8_t							device_mouse_status;
	uint16_t						device_mouse_x;
	uint16_t						device_mouse_y;
	uint16_t						device_mouse_z;
	uint16_t						device_keyboard[ 8 ];	// cache size of 8 keys

	// variables of Framebuffer functions
	uint32_t						*framebuffer_base_address;
	uint16_t						framebuffer_width_pixel;
	uint16_t						framebuffer_height_pixel;
	uint32_t						framebuffer_pitch_byte;
	int64_t							framebuffer_pid;

	// variables of GDT management functions
	struct KERNEL_STRUCTURE_GDT_HEADER			gdt_header;

	// variables of IDT management functions
	struct KERNEL_STRUCTURE_IDT_HEADER			idt_header;
	// functions of IDT management
	void							(*idt_mount)( uint8_t id, uint16_t type, uintptr_t address );

	// variables of Init
	int64_t							init_pid;

	// variables of I/O APIC management functions
	volatile struct KERNEL_STRUCTURE_IO_APIC_REGISTER	*io_apic_base_address;
	uint32_t						io_apic_irq_lines;
	uint8_t							io_apic_semaphore;
	// functions of I/O APIC management
	uint8_t							(*io_apic_line)( uint8_t irq );
	uint8_t							(*io_apic_line_acquire)( void );
	void							(*io_apic_connect)( uint8_t line, uint32_t io_apic_register );

	// variables of IPC management functions
	struct STD_STRUCTURE_IPC				*ipc_base_address;
	uint8_t							ipc_semaphore;

	// variables of Log management functions
	void							(*log)( uint8_t *string, ... );

	// variables of APIC management functions
	volatile struct KERNEL_STRUCTURE_LAPIC			*lapic_base_address;
	uint64_t						lapic_id_highest;
	// functions of APIC management
	void							(*lapic_accept)( void );

	// variables of Library management functions
	struct KERNEL_STRUCTURE_LIBRARY				*library_base_address;
	uint32_t						*library_map_address;

	// variables of Log management functions
	uint8_t							log_semaphore;

	// variables of Memory management functions
	uint32_t						*memory_base_address;
	// functions of Memory management
	uintptr_t						(*memory_alloc)( uint64_t N );
	uintptr_t						(*memory_alloc_low)( uint64_t N );
	uintptr_t						(*memory_alloc_page)( void );
	void							(*memory_clean)( uintptr_t *address, uint64_t n );
	void							(*memory_release)( uintptr_t address, uint64_t N );
	void							(*memory_release_page)( uintptr_t address );

	// variables of Modules management functions
	// uint32_t						*module_map_address;
	// functions of Module management
	int64_t							(*module_thread)( uintptr_t function, uint8_t *name, uint64_t length );

	// variables of Network management functions
	struct STD_STRUCTURE_NETWORK_INTERFACE			network_interface;
	uint64_t						*network_rx_base_address;
	volatile uint64_t					network_rx_limit;
	volatile uint8_t					network_rx_semaphore;
	uint64_t						*network_tx_base_address;
	volatile uint64_t					network_tx_limit;
	volatile uint8_t					network_tx_semaphore;
	struct KERNEL_STRUCTURE_NETWORK_SOCKET			*network_socket_list;
	uint8_t							network_socket_semaphore;
	uint8_t							network_socket_port_semaphore;
	// functions of Network management
	void							(*network_rx)( uintptr_t packet );
	uintptr_t						(*network_tx)( void );
	void							(*network_socket_close_by_pid)( int64_t pid );

	// variables of Page management functions
	uint64_t						*page_base_address;
	uint8_t							page_semaphore;
	uint64_t						page_total;
	uint64_t						page_available;
	uint64_t						page_limit;
	uint64_t						page_structure;
	uint64_t						page_shared;
	// functions of Page management
	void							(*page_deconstruct)( uint64_t *pml4, uint8_t type );
	uint8_t							(*page_map)( uint64_t *pml4, uintptr_t source, uintptr_t target, uint64_t N, uint16_t flags );
	uint8_t							(*page_release)( uint64_t *pml4, uint64_t address, uint64_t pages );

	// variables of Storage management functions
	struct KERNEL_STRUCTURE_STORAGE				*storage_base_address;
	uint64_t						storage_root;
	uint8_t							storage_semaphore;

	// variables of Stream management functions
	struct KERNEL_STRUCTURE_STREAM				*stream_base_address;
	uint8_t							stream_semaphore;
	// functions of Stream management
	void							(*stream_release)( struct KERNEL_STRUCTURE_STREAM *stream );

	// variables of Task management functions
	struct KERNEL_STRUCTURE_TASK				*task_base_address;
	struct KERNEL_STRUCTURE_TASK				**task_cpu_address;	// contains pointers to task inside queue by specified CPU id
	uint8_t							task_cpu_semaphore;
	uint8_t							task_semaphore;
	uint64_t						task_limit;
	uint64_t						task_count;
	int64_t							task_id;
	// functions of Task management
	struct KERNEL_STRUCTURE_TASK 				*(*task_active)( void );
	int64_t							(*task_pid)( void );
	struct KERNEL_STRUCTURE_TASK				*(*task_by_id)( int64_t pid );

#ifdef LIB_TERMINAL
	// variables of Terminal Library management functions
	struct LIB_TERMINAL_STRUCTURE				terminal;
	uint8_t							terminal_semaphore;
#endif

	// variables of Time management functions
	volatile uint64_t					time_rtc;
	volatile uint64_t					time_rdtsc;
	// functions of Time management
	void							(*time_sleep)( uint64_t t );	// Real Time Controller Ticks

	// variables of TSS management functions
	struct KERNEL_STRUCTURE_TSS				tss_table;

	// variables of VFS management functions
	struct KERNEL_STRUCTURE_VFS				*vfs_base_address;
	uint8_t							vfs_semaphore;
};