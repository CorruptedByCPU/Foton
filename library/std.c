/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//-----------------------------------------------------------------------
	// variables, structures, definitions
	//-----------------------------------------------------------------------
	#include	"../std.h"

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