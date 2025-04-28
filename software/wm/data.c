/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint64_t wm_pid;

struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER	wm_framebuffer;

struct WM_STRUCTURE_OBJECT			*wm_object_base_address;	uint8_t wm_object_lock = FALSE;
struct WM_STRUCTURE_OBJECT			**wm_list_base_address;
struct WM_STRUCTURE_ZONE			*wm_zone_base_address;

struct WM_STRUCTURE_OBJECT			wm_object_cache = { EMPTY };

struct WM_STRUCTURE_OBJECT			*wm_object_workbench;
struct WM_STRUCTURE_OBJECT			*wm_object_taskbar;
struct WM_STRUCTURE_OBJECT			*wm_object_cursor;

uint64_t wm_list_limit				= 0;
uint64_t wm_zone_limit				= 0;

struct WM_STRUCTURE_OBJECT			*wm_object_active;

int64_t wm_mouse_x				= 0;
int64_t wm_mouse_y				= 0;
int64_t wm_mouse_z				= 0;
