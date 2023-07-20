/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define	KERNEL_name		"Foton"
#define	KERNEL_version		"0"
#define	KERNEL_revision		"12"
#define	KERNEL_architecture	"x86_64"
#define	KERNEL_language		"C"

#define	KERNEL_BASE_address	0xFFFFFFFF80000000	// higher half

#define	KERNEL_STACK_address	0xFFFFFFFFFFFFE000	// minimal size, last 2 pages of logical address
#define	KERNEL_STACK_pointer	0xFFFFFFFFFFFFF000

struct KERNEL {
	// variables of HPET management functions
	struct KERNEL_HPET_STRUCTURE_REGISTER	*hpet_base_address;
	uint64_t	hpet_microtime;

	// variables of I/O APIC management functions
	struct KERNEL_IO_APIC_STRUCTURE_REGISTER	*io_apic_base_address;

	// variables of APIC management functions
	struct KERNEL_LAPIC_STRUCTURE	*lapic_base_address;

	// variables of Memory management functions
	uint32_t	*memory_base_address;
	uint8_t		memory_semaphore;

	// variables of page management functions
	uint64_t	*page_base_address;
	uint64_t	page_total;
	uint64_t	page_available;
	uint64_t	page_limit;
};