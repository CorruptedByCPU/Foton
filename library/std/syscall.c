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

int64_t std_syscall_value() {
	// initialize local variable
	int64_t rax = EMPTY;

	// call the kernel function
	__asm__ volatile( "push %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx\n" : "=a" (rax) );

	// return value
	return rax;
}

uint64_t std_syscall_value_unsigned() {
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

	// return pointer
	return rax;
}

//------------------------------------------------------------------------------

void std_exit( void ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_EXIT) );
}

void std_framebuffer( struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER *framebuffer ) {
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

	// return unsigned value
	return std_syscall_value_unsigned();
}

void std_log( uint8_t *string, uint64_t length ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_LOG), "D" (string), "S" (length) );

	// return nothing
	return std_syscall_empty();
}

int64_t std_thread( uintptr_t function, uint8_t *string, uint64_t length ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_THREAD), "D" (function), "S" (string), "d" (length) );

	// return value
	return std_syscall_value();
}

int64_t std_pid( void ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_PID) );

	// return value
	return std_syscall_value();
}

int64_t std_exec( uint8_t *string, uint64_t length ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_EXEC), "D" (string), "S" (length) );

	// return value
	return std_syscall_value();
}

uint8_t std_pid_check( int64_t pid ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_PID_CHECK), "D" (pid) );

	// return TRUE/FALSE
	return std_syscall_bool();
}

void std_ipc_send( int64_t pid, uint8_t *data ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_IPC_SEND), "D" (pid), "S" (data) );

	// return nothing
	return std_syscall_empty();
}

int64_t std_ipc_receive( uint8_t *data ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_IPC_RECEIVE), "D" (data) );

	// return value
	return std_syscall_value();
}

uintptr_t std_memory_share( int64_t pid, uintptr_t address, uint64_t page ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_MEMORY_SHARE), "D" (pid), "S" (address), "d" (page) );

	// return pointer
	return std_syscall_pointer();
}

void std_mouse( struct STD_SYSCALL_STRUCTURE_MOUSE *mouse ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_MOUSE), "D" (mouse) );

	// return nothing
	return std_syscall_empty();
}

void std_framebuffer_change( struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER *framebuffer ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_FRAMEBUFFER_CHANGE), "D" (framebuffer) );

	// return nothing
	return std_syscall_empty();
}

uint8_t std_ipc_receive_by_pid( uint8_t *data, int64_t pid ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_IPC_RECEIVE_BY_PID), "D" (data), "S" (pid) );

	// return value
	return std_syscall_bool();
}