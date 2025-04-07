/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_PAGE
	#define	KERNEL_PAGE

	#define	KERNEL_PAGE_FLAG_cache_disable	(1 << 4)
	#define	KERNEL_PAGE_FLAG_length		(1 << 7)
	#define	KERNEL_PAGE_FLAG_present	(1 << 0)
	#define	KERNEL_PAGE_FLAG_user		(1 << 2)
	#define	KERNEL_PAGE_FLAG_write		(1 << 1)
	#define	KERNEL_PAGE_FLAG_write_through	(1 << 3)
	// Cyjon/Foton definied
	#define	KERNEL_PAGE_TYPE_mask		0x0E00	// 0b0000111000000000
	#define	KERNEL_PAGE_TYPE_offset		9
	#define	KERNEL_PAGE_TYPE_KERNEL		0x00
	#define	KERNEL_PAGE_TYPE_MODULE		0x01
	#define	KERNEL_PAGE_TYPE_PROCESS	0x02
	#define	KERNEL_PAGE_TYPE_THREAD		0x03
	#define	KERNEL_PAGE_TYPE_LIBRARY	0x04
	#define	KERNEL_PAGE_TYPE_SHARED		0x05
	// #define	KERNEL_PAGE_TYPE_		0x06
	// #define	KERNEL_PAGE_TYPE_		0x07

	#define	KERNEL_PAGE_PMLx_entry		512

	#define	KERNEL_PAGE_PML1_shift		12
	#define	KERNEL_PAGE_PML2_shift		21
	#define	KERNEL_PAGE_PML3_shift		30
	#define	KERNEL_PAGE_PML4_shift		39

	uintptr_t kernel_page_address( uint64_t *pml4, uintptr_t source );
	uint8_t kernel_page_alloc( uint64_t *pml4, uintptr_t target, uint64_t n, uint16_t flags );
	void kernel_page_deconstruct( uint64_t *pml4, uint8_t type );
	uint8_t kernel_page_disconnect( uint64_t *pml4, uint64_t source, uint64_t n );
	uint8_t kernel_page_map( uint64_t *pml4, uintptr_t source, uintptr_t target, uint64_t n, uint16_t flags );
	void kernel_page_merge( uint64_t *pml4_parent, uint64_t *pml4_child );
#endif
