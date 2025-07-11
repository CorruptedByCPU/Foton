/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER de_framebuffer;

struct LIB_WINDOW_DESCRIPTOR *de_window_wallpaper = EMPTY;
struct LIB_WINDOW_DESCRIPTOR *de_window_taskbar = EMPTY;
struct LIB_WINDOW_DESCRIPTOR *de_window_cursor = EMPTY;

struct LIB_WINDOW_STRUCTURE_LIST *de_taskbar_base_address = EMPTY;
uint64_t de_taskbar_limit = EMPTY;

uint32_t *de_taskbar_list_base_address;

uint64_t de_clock_sync = EMPTY;
