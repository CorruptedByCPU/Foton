/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_page( void ) {
	// allow all BS/A processors to write on read-only pages inside ring0
	__asm__ volatile( "movq %cr0, %rax\nandq $~(1 << 16), %rax\nmovq %rax, %cr0" );

	// alloc 1 page for PML4 kernel environment array
	kernel -> page_base_address = (uint64_t *) kernel_memory_alloc( TRUE );

	// --------------------------------------------------------------------

	// map all memory areas marked as USABLE, KERNEL_AND_MODULES, FRAMEBUFFER, BOOTLOADER_RECLAIMABLE, ACPI_RECLAIMABLE
	for( uint64_t i = 0; i < limine_memmap_request.response -> entry_count; i++ )
		// USABLE, KERNEL_AND_MODULES, FRAMEBUFFER, BOOTLOADER_RECLAIMABLE or ACPI_RECLAIMABLE memory area?
		if( limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_USABLE || limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_KERNEL_AND_MODULES || limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE || limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_FRAMEBUFFER || limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_ACPI_RECLAIMABLE )
			// map memory area to kernel paging arrays
			kernel_page_map( kernel -> page_base_address, limine_memmap_request.response -> entries[ i ] -> base, limine_memmap_request.response -> entries[ i ] -> base | KERNEL_PAGE_mirror, MACRO_PAGE_ALIGN_UP( limine_memmap_request.response -> entries[ i ] -> length ) >> STD_SHIFT_PAGE, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write );

	// --------------------------------------------------------------------

	// map LAPIC controller area
	kernel_page_map( kernel -> page_base_address, (uintptr_t) kernel -> lapic_base_address & ~KERNEL_PAGE_mirror, (uintptr_t) kernel -> lapic_base_address, TRUE, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write );

	// map I/O APIC controller area
	kernel_page_map( kernel -> page_base_address, (uintptr_t) kernel -> io_apic_base_address & ~KERNEL_PAGE_mirror, (uintptr_t) kernel -> io_apic_base_address, TRUE, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write );

	// map HPET controller area
	// kernel_page_map( kernel -> page_base_address, (uintptr_t) kernel -> hpet_base_address & ~KERNEL_PAGE_mirror, (uintptr_t) kernel -> hpet_base_address, TRUE, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write );

	// now something harder ------------------------------------------------

	// kernel file properties
	struct limine_file *local_limine_file = limine_kernel_file_request.response -> kernel_file;

	// kernel file ELF64 properties
	struct LIB_ELF_STRUCTURE *local_limine_file_elf64_header = (struct LIB_ELF_STRUCTURE *) local_limine_file -> address;

	// kernel file ELF64 header properties
	struct LIB_ELF_STRUCTURE_HEADER *local_limine_file_elf64_header_entry = (struct LIB_ELF_STRUCTURE_HEADER *) ((uint64_t) local_limine_file_elf64_header + local_limine_file_elf64_header -> headers_offset);

	// retrieve flags and map kernel file segments in proper place
	for( uint16_t i = 0; i < local_limine_file_elf64_header -> h_entry_count; i++ ) {
		// ignore blank entries
		if( ! local_limine_file_elf64_header_entry[ i ].type || ! local_limine_file_elf64_header_entry[ i ].memory_size ) continue;

		// segment properties
		uintptr_t local_segment_offset = MACRO_PAGE_ALIGN_DOWN( local_limine_file_elf64_header_entry[ i ].virtual_address ) - KERNEL_BASE_address;
		uint64_t local_segment_page = (MACRO_PAGE_ALIGN_UP( local_limine_file_elf64_header_entry[ i ].virtual_address + local_limine_file_elf64_header_entry[ i ].memory_size ) - KERNEL_BASE_address) >> STD_SHIFT_PAGE;

		// default flag, for every segment
		uint16_t local_segment_flags = KERNEL_PAGE_FLAG_present;

		// update with additional flag (if exist)
		if( local_limine_file_elf64_header_entry[ i ].flags & LIB_ELF_FLAG_write ) local_segment_flags |= KERNEL_PAGE_FLAG_write;

		// map kernel memory area
		kernel_page_map( kernel -> page_base_address, limine_kernel_address_request.response -> physical_base + local_segment_offset, KERNEL_BASE_address + local_segment_offset, local_segment_page, local_segment_flags );
	}

	// and last thing, create kernel stack area
	kernel_page_alloc( (uint64_t *) kernel -> page_base_address, KERNEL_STACK_address, KERNEL_STACK_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write );
}
