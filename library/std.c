/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

inline void std_syscall_empty( void ) {
	// call syscall of kernel
	__asm__ volatile( "push %%rax\npush %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx\npop %%rax" :: );
}

inline uint8_t std_syscall_bool() {
	// initialize local variable
	uint8_t rax;

	// call the kernel function
	__asm__ volatile( "push %%rcx\npush %%r11\nsyscall \npop %%r11\npop %%rcx\n" : "=a" (rax) );

	// return TRUE/FALSE
	return rax;
}

inline int64_t std_syscall_signed() {
	// initialize local variable
	int64_t rax;

	// call the kernel function
	__asm__ volatile( "push %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx\n" : "=a" (rax) );

	// return value
	return rax;
}

inline uint64_t std_syscall_unsigned() {
	// initialize local variable
	uint64_t rax;

	// call the kernel function
	__asm__ volatile( "push %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx\n" : "=a" (rax) );

	// return unsigned value
	return rax;
}

inline uintptr_t std_syscall_pointer() {
	// initialize local variable
	uintptr_t rax;

	// call the kernel function
	__asm__ volatile( "push %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx\n" : "=a" (rax) );

	// return a pointer
	return rax;
}

// initialization of process environment
static void _entry( void ) {
	// sad hack :|
	__asm__ volatile( "testw $0x08, %sp\nje .+4\npushq $0x00" );

	// execute process flow
	int64_t result;	// initialize local variable
	__asm__ volatile( "call _main" : "=a" (result) );

	// execute leave out routine
	__asm__ volatile( "" :: "a" (STD_SYSCALL_EXIT) );
	std_syscall_empty();
}