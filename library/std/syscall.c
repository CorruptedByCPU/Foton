/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void std_syscall_empty( void ) {
	// call syscall of kernel
	__asm__ volatile( "push %%rax\npush %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx\npop %%rax" :: );
}

uint8_t std_syscall_bool() {
	// initialize local variable
	uint8_t rax = EMPTY;

	// call the kernel function
	__asm__ volatile( "push %%rcx\npush %%r11\nsyscall \npop %%r11\npop %%rcx\n" : "=a" (rax) );

	// return TRUE/FALSE
	return rax;
}

int64_t std_syscall_signed() {
	// initialize local variable
	int64_t rax = EMPTY;

	// call the kernel function
	__asm__ volatile( "push %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx\n" : "=a" (rax) );

	// return value
	return rax;
}

uint64_t std_syscall_unsigned() {
	// initialize local variable
	uint64_t rax = EMPTY;

	// call the kernel function
	__asm__ volatile( "push %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx\n" : "=a" (rax) );

	// return unsigned value
	return rax;
}

uintptr_t std_syscall_pointer() {
	// initialize local variable
	uintptr_t rax = EMPTY;

	// call the kernel function
	__asm__ volatile( "push %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx\n" : "=a" (rax) );

	// return a pointer
	return rax;
}

//------------------------------------------------------------------------------

void std_syscall_framebuffer( struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER *framebuffer ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_FRAMEBUFFER), "D" (framebuffer) );

	// return nothing
	return std_syscall_empty();
}

uintptr_t std_memory_alloc( uint64_t byte ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_MEMORY_ALLOC), "D" (byte) );

	// return pointer
	return std_syscall_pointer();
}

void std_memory_release( uintptr_t source, uint64_t byte ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_MEMORY_RELEASE), "D" (source), "S" (byte) );

	// return nothing
	return std_syscall_empty();
}

uint64_t std_uptime( void ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_UPTIME) );

	// return value
	return std_syscall_unsigned();
}

void std_syscall_log( uint8_t *string, uint64_t length ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_LOG), "D" (string), "S" (length) );

	// return nothing
	return std_syscall_empty();
}