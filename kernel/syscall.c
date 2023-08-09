/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_syscall_exit( void ) {
	// properties of current task
	struct KERNEL_TASK_STRUCTURE *task = kernel -> task_cpu_address[ kernel_lapic_id() ];

	// mark task as not active and ready to close
	task -> flags &= ~KERNEL_TASK_FLAG_active;
	task -> flags |= KERNEL_TASK_FLAG_close;

	// release left BS/A time
	__asm__ volatile( "int $0x20" );
}

void kernel_syscall_framebuffer( struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER *framebuffer ) {
	// return information about existing framebuffer
	framebuffer -> base_address	= kernel -> framebuffer_base_address;
	framebuffer -> width_pixel	= kernel -> framebuffer_width_pixel;
	framebuffer -> height_pixel	= kernel -> framebuffer_height_pixel;
	framebuffer -> pitch_byte	= kernel -> framebuffer_pitch_byte;

	// change framebuffer owner if possible
	__sync_val_compare_and_swap( &kernel -> framebuffer_owner_pid, EMPTY, kernel_task_pid() );

	// return information about framebuffer owner
	framebuffer -> owner_pid	= kernel -> framebuffer_owner_pid;
}

uintptr_t kernel_syscall_memory_alloc( uint64_t byte) {
	// convert bytes to pages
	uint64_t page = MACRO_PAGE_ALIGN_UP( byte ) >> STD_SHIFT_PAGE;

	// task properties
	struct KERNEL_TASK_STRUCTURE *task = (struct KERNEL_TASK_STRUCTURE *) kernel_task_active();

	// acquire N continuous pages
	uintptr_t allocated = EMPTY;
	if( (allocated = kernel_memory_acquire( task -> memory_map, page )) ) {
		// allocate space inside process paging area
		kernel_page_alloc( (uint64_t *) task -> cr3, KERNEL_EXEC_base_address + (allocated << STD_SHIFT_PAGE), page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | KERNEL_PAGE_FLAG_process );

		// process memory usage
		task -> page += page;

		// return the address of the first page in the collection
		return KERNEL_EXEC_base_address + (allocated << STD_SHIFT_PAGE);
	}

	// no free space
	return EMPTY;
}

void kernel_syscall_memory_release( uintptr_t source, uint64_t page) {
	// task properties
	struct KERNEL_TASK_STRUCTURE *task = (struct KERNEL_TASK_STRUCTURE *) kernel_task_active();

	// release occupied pages
	for( uint64_t i = source >> STD_SHIFT_PAGE; i < (source >> STD_SHIFT_PAGE) + page; i++ ) {
		// remove page from paging structure
		uintptr_t page = kernel_page_remove( (uint64_t *) task -> cr3, i << STD_SHIFT_PAGE );

		// if released
		if( page ) {
			// return page back to stack
			kernel_memory_release_page( page );

			// process memory usage
			task -> page--;
		}

		// release page in binary memory map of process
		kernel_memory_dispose( task -> memory_map, i, 1 );
	}
}