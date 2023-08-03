/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void std_syscall_empty( void ) {
	// call syscall of kernel
	__asm__ volatile( "push %%rax\npush %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx\npop %%rax" :: );
}

uint8_t std_syscall_bool() {
	// initialize local variable
	uint8_t rax;

	// call the kernel function
	__asm__ volatile( "push %%rcx\npush %%r11\nsyscall \npop %%r11\npop %%rcx\n" : "=a" (rax) );

	// return TRUE/FALSE
	return rax;
}

int64_t std_syscall_signed() {
	// initialize local variable
	int64_t rax;

	// call the kernel function
	__asm__ volatile( "push %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx\n" : "=a" (rax) );

	// return value
	return rax;
}

uint64_t std_syscall_unsigned() {
	// initialize local variable
	uint64_t rax;

	// call the kernel function
	__asm__ volatile( "push %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx\n" : "=a" (rax) );

	// return unsigned value
	return rax;
}

uintptr_t std_syscall_pointer() {
	// initialize local variable
	uintptr_t rax;

	// call the kernel function
	__asm__ volatile( "push %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx\n" : "=a" (rax) );

	// return a pointer
	return rax;
}