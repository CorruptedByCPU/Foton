/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define	KERNEL_name		"Foton"
#define	KERNEL_version		"0"
#define	KERNEL_revision		"36"
#define	KERNEL_architecture	"x86_64"
#define	KERNEL_language		"C"

#define	KERNEL_BASE_address	0xFFFFFFFF80000000	// higher half

#define	KERNEL_STACK_address	0xFFFFFFFFFFFFE000	// minimal size, last 2 pages of logical address
#define	KERNEL_STACK_pointer	0xFFFFFFFFFFFFF000

struct KERNEL {
	// variable of Kernel management functions
	volatile uint64_t	cpu_count;

	// variable of GDT management functions
	struct KERNEL_GDT_STRUCTURE_HEADER	gdt_header;

	// variables of Framebuffer functions
	uint32_t	*framebuffer_base_address;
	uint16_t	framebuffer_width_pixel;
	uint16_t	framebuffer_height_pixel;
	uint32_t	framebuffer_pitch_byte;
	int64_t		framebuffer_owner_pid;

	// variable of IDT management functions
	struct KERNEL_IDT_STRUCTURE_HEADER	idt_header;

	// variables of HPET management functions
	volatile struct KERNEL_HPET_STRUCTURE_REGISTER	*hpet_base_address;
	uint64_t	hpet_miliseconds;
	uint8_t		hpet_timers;

	// variables of I/O APIC management functions
	volatile struct KERNEL_IO_APIC_STRUCTURE_REGISTER	*io_apic_base_address;
	uint32_t	io_apic_irq_lines;

	// variables of APIC management functions
	volatile struct KERNEL_LAPIC_STRUCTURE	*lapic_base_address;
	uint64_t	lapic_id_highest;

	// variables of Library management functions
	struct KERNEL_LIBRARY_STRUCTURE	*library_base_address;
	uint32_t	*library_map_address;

	// variables of Memory management functions
	uint32_t	*memory_base_address;
	uint8_t		memory_semaphore;

	// variables of Page management functions
	uint64_t	*page_base_address;
	uint64_t	page_total;
	uint64_t	page_available;
	uint64_t	page_limit;

	// variables of Storage management functions
	struct KERNEL_STORAGE_STRUCTURE	*storage_base_address;
	uint64_t	storage_root_id;
	uint8_t		storage_semaphore;

	// variables of Task management functions
	struct KERNEL_TASK_STRUCTURE	*task_base_address;
	struct KERNEL_TASK_STRUCTURE	**task_cpu_address;	// contains pointers to task inside queue by specified CPU id
	uint8_t		task_cpu_semaphore;
	uint64_t	task_limit;
	uint64_t	task_count;
	int64_t		task_id;

	// variables of TSS management functions
	struct KERNEL_TSS_STRUCTURE	tss_table;
};
