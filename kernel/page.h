/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_PAGE
	#define	KERNEL_PAGE

	// location of physical page address in logical
	#define	KERNEL_PAGE_mirror	0xFFFF800000000000

	// i really doesn't see any good reason to use 5-level paging for this project
	#define	KERNEL_PAGE_PML5_mask	0xFFFF000000000000

	// standard flags available for paging
	#define	KERNEL_PAGE_FLAG_present		(1 << 0)
	#define	KERNEL_PAGE_FLAG_write			(1 << 1)
	#define	KERNEL_PAGE_FLAG_user			(1 << 2)
	#define	KERNEL_PAGE_FLAG_write_through		(1 << 3)
	#define	KERNEL_PAGE_FLAG_cache_disable		(1 << 4)
	#define	KERNEL_PAGE_FLAG_length			(1 << 7)
	// and foton definied, for easier memory management
	#define	KERNEL_PAGE_TYPE_mask			0b0000111000000000
	#define	KERNEL_PAGE_TYPE_offset			9
	#define	KERNEL_PAGE_TYPE_KERNEL			0b000
	#define	KERNEL_PAGE_TYPE_MODULE			0b001
	#define	KERNEL_PAGE_TYPE_PROCESS		0b010
	#define	KERNEL_PAGE_TYPE_THREAD			0b011
	#define	KERNEL_PAGE_TYPE_LIBRARY		0b100
	#define	KERNEL_PAGE_TYPE_SHARED			0b101
	// #define	KERNEL_PAGE_TYPE_			0b110
	// #define	KERNEL_PAGE_TYPE_			0b111

	#define	KERNEL_PAGE_PML_records			512

	#define	KERNEL_PAGE_PML1_shift			12
	#define	KERNEL_PAGE_PML2_shift			21
	#define	KERNEL_PAGE_PML3_shift			30
	#define	KERNEL_PAGE_PML4_shift			39

	#define	KERNEL_PAGE_PML1_byte			0x0000000000200000
	#define	KERNEL_PAGE_PML2_byte			0x0000000040000000
	#define	KERNEL_PAGE_PML3_byte			0x0000008000000000

	#define	KERNEL_PAGE_ENTRY_stack			223
	#define	KERNEL_PAGE_ENTRY_stack_context		511

	// alloc N pages for process
	uint8_t kernel_page_alloc( uint64_t *pml4, uintptr_t address, uint64_t pages, uint16_t flags );

	// disconnects memory area from paging structure (doesn't release it!)
	uint8_t kernel_page_detach( uint64_t *pml4, uint64_t address, uint64_t pages );

	// checks if page is empty of entries
	uint8_t kernel_page_empty( uint64_t *page, uint64_t N );

	// connect source with target address for definies paging array
	uint8_t kernel_page_map( uint64_t *pml4, uintptr_t source, uintptr_t target, uint64_t N, uint16_t flags );

	// releases N pages for process
	uint8_t kernel_page_release( uint64_t *pml4, uint64_t address, uint64_t N );
#endif
