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

struct KERNEL {
	volatile struct KERNEL_STRUCTURE_APIC		*apic_base_address;

	uint32_t					*framebuffer_base_address;
	uint16_t					framebuffer_width_pixel;
	uint16_t					framebuffer_height_pixel;
	uint32_t					framebuffer_pitch_byte;
	int64_t						framebuffer_pid;

	struct KERNEL_STRUCTURE_GDT_HEADER		gdt_header;

	struct KERNEL_STRUCTURE_IDT_HEADER		idt_header;

	volatile struct KERNEL_STRUCTURE_IO_APIC	*io_apic_base_address;

	struct KERNEL_STRUCTURE_LIBRARY			*library_base_address;
	uint64_t					library_limit;
	uint32_t					*library_memory_address;

	uint32_t					*memory_base_address;

	uint64_t					*page_base_address;
	uint64_t					page_available;
	uint64_t					page_limit;
	uint64_t					page_total;

	struct KERNEL_STRUCTURE_STORAGE			*storage_base_address;
	uint64_t					storage_limit;
	uint8_t						storage_lock;

	uint64_t					*task_ap_address;
	struct KERNEL_STRUCTURE_TASK			*task_base_address;
	uint64_t					task_limit;

	struct LIB_TERMINAL_STRUCTURE			terminal;

	uint32_t					time_hz;
	uint64_t					time_units;

	struct KERNEL_STRUCTURE_TSS			tss;

	struct KERNEL_STRUCTURE_VFS_SOCKET		*vfs_base_address;
	uint64_t					vfs_limit;
	uint8_t						vfs_lock;
};

void kernel_log( uint8_t *string, ... );
