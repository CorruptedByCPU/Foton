/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct LIB_TERMINAL_STRUCTURE	console_terminal;
struct LIB_INTERFACE_STRUCTURE	console_interface;

MACRO_IMPORT_FILE_AS_ARRAY( interface, "./software/console/interface.json" );

int64_t console_pid_of_shell = EMPTY;
uint8_t *console_stream_in = EMPTY;