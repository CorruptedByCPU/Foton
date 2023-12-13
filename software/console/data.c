/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER kernel_framebuffer;

struct LIB_TERMINAL_STRUCTURE	console_terminal;
struct LIB_INTERFACE_STRUCTURE	console_interface;

MACRO_IMPORT_FILE_AS_ARRAY( interface, "./software/console/interface.json" );

int64_t	console_pid = EMPTY;
int64_t console_pid_of_shell = EMPTY;

uint8_t *console_stream_in = EMPTY;

uint8_t	console_the_master_of_puppets = FALSE;

struct STD_STREAM_STRUCTURE_META console_stream_meta = { EMPTY };