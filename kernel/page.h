/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

#ifndef	KERNEL_PAGE
	#define	KERNEL_PAGE

	// location of physical page address as logical
	#define	KERNEL_PAGE_mirror	0xFFFF800000000000

	// i really doesn't see any good reason to use 5-level paging for this project
	#define	KERNEL_PAGE_PML5_mask	0xFFFF000000000000

	// standard flags available for paging
	#define	KERNEL_PAGE_FLAG_present		1 << 0
	#define	KERNEL_PAGE_FLAG_write			1 << 1
	#define	KERNEL_PAGE_FLAG_user			1 << 2
	#define	KERNEL_PAGE_FLAG_write_through		1 << 3
	#define	KERNEL_PAGE_FLAG_cache_disable		1 << 4
	#define	KERNEL_PAGE_FLAG_length			1 << 7
	// and foton definied, for easier memory management
	#define	KERNEL_PAGE_FLAG_process		1 << 9
	#define	KERNEL_PAGE_FLAG_shared			1 << 10
	#define	KERNEL_PAGE_FLAG_library		1 << 11

	// overall size of whole page
	#define	KERNEL_PAGE_PML1_byte			0x0000000000200000
	#define	KERNEL_PAGE_PML2_byte			0x0000000040000000
	#define	KERNEL_PAGE_PML3_byte			0x0000008000000000
	#define	KERNEL_PAGE_PML4_byte			0x0001000000000000
#endif
