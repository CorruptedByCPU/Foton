/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void std_syscall_empty( void ) {
	// call syscall of kernel
	__asm__ volatile( "push %%rax\npush %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx\npop %%rax" :: );
}

uint8_t std_syscall_bool( void ) {
	// initialize local variable
	uint8_t rax = EMPTY;

	// call the kernel function
	__asm__ volatile( "push %%rcx\npush %%r11\nsyscall \npop %%r11\npop %%rcx\n" : "=a" (rax) );

	// return TRUE/FALSE
	return rax;
}

int64_t std_syscall_value( void ) {
	// initialize local variable
	int64_t rax = EMPTY;

	// call the kernel function
	__asm__ volatile( "push %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx\n" : "=a" (rax) );

	// return value
	return rax;
}

uint64_t std_syscall_value_unsigned( void ) {
	// initialize local variable
	uint64_t rax = EMPTY;

	// call the kernel function
	__asm__ volatile( "push %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx\n" : "=a" (rax) );

	// return unsigned value
	return rax;
}

uintptr_t std_syscall_pointer( void ) {
	// initialize local variable
	uintptr_t rax = EMPTY;

	// call the kernel function
	__asm__ volatile( "push %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx\n" : "=a" (rax) );

	// return pointer
	return rax;
}

//------------------------------------------------------------------------------

void std_framebuffer( struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER *framebuffer ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_FRAMEBUFFER), "D" (framebuffer) );

	// return nothing
	return std_syscall_empty();
}

uintptr_t std_memory_alloc( uint64_t page ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_MEMORY_ALLOC), "D" (page) );

	// return pointer
	return std_syscall_pointer();
}

void std_memory_release( uintptr_t source, uint64_t page ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_MEMORY_RELEASE), "D" (source), "S" (page) );

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

int64_t std_exec( uint8_t *string, uint64_t length, uint8_t stream_flow ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_EXEC), "D" (string), "S" (length), "d" (stream_flow) );

	// return value
	return std_syscall_value();
}

uint8_t std_pid_check( int64_t pid ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_PID_CHECK), "D" (pid) );

	// return boolean
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

	// return boolean
	return std_syscall_bool();
}

uint8_t std_stream_out( uint8_t *string, uint64_t length ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_STREAM_OUT), "D" (string), "S" (length) );

	// return boolean
	return std_syscall_bool();
}

uint64_t std_stream_in( uint8_t *target ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_STREAM_IN), "D" (target) );

	// return unsigned value
	return std_syscall_value_unsigned();
}

uint16_t std_keyboard( void ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_KEYBOARD) );

	// return unsigned value
	return std_syscall_value_unsigned();
}

void std_stream_set( uint8_t *meta, uint8_t stream_type ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_STREAM_SET), "D" (meta), "S" (stream_type) );

	// return nothing
	return std_syscall_empty();
}

uint8_t std_stream_get( uint8_t *target, uint8_t stream_type ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_STREAM_GET), "D" (target), "S" (stream_type) );

	// return TRUE/FALSE
	return std_syscall_bool();
}

void std_memory( struct STD_SYSCALL_STRUCTURE_MEMORY *memory ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_MEMORY), "D" (memory) );

	// return nothing
	return std_syscall_empty();
}

uint64_t std_sleep( uint64_t units ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_SLEEP) );

	// return unsigned value
	return std_syscall_value_unsigned();
}

uint64_t std_file( struct DEPRECATED_STD_FILE_STRUCTURE *file ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_FILE), "D" (file) );

	// return unsigned value
	return std_syscall_value_unsigned();
}

void std_file_read( struct DEPRECATED_STD_FILE_STRUCTURE *file, uintptr_t target ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_FILE_READ), "D" (file), "S" (target) );

	// return nothing
	return std_syscall_empty();
}

uint8_t std_cd( uint8_t *path ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_CD), "D" (path) );

	// return TRUE/FALSE
	return std_syscall_bool();
}

int64_t std_ipc_receive_by_type( uint8_t *data, uint8_t type ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_IPC_RECEIVE_BY_TYPE), "D" (data), "S" (type) );

	// return value
	return std_syscall_value();
}

uint64_t std_microtime( void ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_MICROTIME) );

	// return value unsigned
	return std_syscall_value_unsigned();
}

uint64_t std_time( void ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_TIME) );

	// return value unsigned
	return std_syscall_value_unsigned();
}

int64_t std_file_write( struct DEPRECATED_STD_FILE_STRUCTURE *file, uintptr_t source, uint64_t byte ) {
	// request syscall
	__asm__ volatile( "" :: "a" (STD_SYSCALL_FILE_WRITE), "D" (file), "S" (source), "d" (byte) );

	// return value
	return std_syscall_value();
}

int64_t NEW_std_file_open( uint8_t *path, uint64_t path_length, uint8_t mode ) {
	// request syscall
	__asm__ volatile( "" :: "a" (NEW_STD_SYSCALL_FILE_OPEN), "D" (path), "S" (path_length), "d" (mode) );

	// return value
	return std_syscall_value();
}

void NEW_std_file_close( int64_t socket ) {
	// request syscall
	__asm__ volatile( "" :: "a" (NEW_STD_SYSCALL_FILE_CLOSE), "D" (socket) );

	// return nothing
	return std_syscall_empty();
}

void NEW_std_file( struct NEW_STD_FILE_STRUCTURE *file ) {
	// request syscall
	__asm__ volatile( "" :: "a" (NEW_STD_SYSCALL_FILE), "D" (file) );

	// return nothing
	return std_syscall_empty();
}