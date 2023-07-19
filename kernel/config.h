/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define	KERNEL_name		"Foton"
#define	KERNEL_version		"0"
#define	KERNEL_revision		"9"
#define	KERNEL_architecture	"x86_64"
#define	KERNEL_language		"C"

// location of physical page address as logical
#define	KERNEL_PAGE_mirror	0xFFFF800000000000

struct KERNEL {
	// variables of HPET management functions
	struct KERNEL_HPET_STRUCTURE_REGISTER	*hpet_base_address;
	uint64_t	hpet_microtime;

	// variables of I/O APIC management functions
	struct KERNEL_IO_APIC_STRUCTURE_REGISTER	*io_apic_base_address;

	// variables of APIC management functions
	struct KERNEL_LAPIC_STRUCTURE	*lapic_base_address;

	// variables of Memory management functions
	uint64_t	*memory_base_address;

	// variables of page management functions
	uint64_t	page_total;
	uint64_t	page_available;
	uint64_t	page_limit;
};