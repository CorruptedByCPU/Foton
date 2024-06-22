/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct STD_STREAM_STRUCTURE_META top_stream_meta;

uint8_t top_string_interface[] = "\e[0m\e[2J\e[38;5;0m\e[48;5;34m\e[2K PID  Memory CPU% Application\e[E";

uint64_t top_line_selected = 0;

uint8_t top_hide_modules = TRUE;

uint64_t top_update_limit = 1024;	// ~1 second

uint64_t top_update_next = 0;	// as fast as possible

uint8_t top_units[] = { ' ', 'K', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y' };