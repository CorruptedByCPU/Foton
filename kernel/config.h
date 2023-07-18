/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define	KERNEL_name		"Foton"
#define	KERNEL_version		"0"
#define	KERNEL_revision		"7"
#define	KERNEL_architecture	"x86_64"
#define	KERNEL_language		"C"

// location of physical page address as logical
#define	KERNEL_PAGE_mirror	0xFFFF800000000000

struct KERNEL {
	// variables of Memory management functions
	uint64_t	*memory_base_address;
};