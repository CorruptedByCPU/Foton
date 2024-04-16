/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_PAGE
	#define	KERNEL_PAGE

	// location of physical page address as logical
	#define	KERNEL_PAGE_logical	0xFFFF800000000000

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
	#define	KERNEL_PAGE_FLAG_external		1 << 11

	// overall size of whole page
	#define	KERNEL_PAGE_PML1_byte			0x0000000000200000
	#define	KERNEL_PAGE_PML2_byte			0x0000000040000000
	#define	KERNEL_PAGE_PML3_byte			0x0000008000000000
	#define	KERNEL_PAGE_PML4_byte			0x0001000000000000

	#define	KERNEL_PAGE_ENTRY_stack			223
	#define	KERNEL_PAGE_ENTRY_stack_context		511

	// alloc N pages for process
	uint8_t kernel_page_alloc( uint64_t *pml4, uint64_t address, uint64_t pages, uint16_t flags );

	// disconnects memory area from paging structure (doesn't release it!)
	void kernel_page_detach( uint64_t *pml4, uint64_t address, uint64_t pages );

	// checks if page is empty of entries
	uint8_t kernel_page_empty( uint64_t *page );

	// connect source with target address for definies paging array
	uint8_t kernel_page_map( uint64_t *pml4, uintptr_t source, uintptr_t target, uint64_t N, uint16_t flags );

	// releases N pages for process
	uint8_t kernel_page_release( uint64_t *pml4, uint64_t address, uint64_t pages );
#endif
