/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint64_t tiwyn_pid;

struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER	tiwyn_framebuffer;

struct TIWYN_STRUCTURE_OBJECT			*tiwyn_object_base_address;	uint8_t tiwyn_object_lock = FALSE;
struct TIWYN_STRUCTURE_OBJECT			**tiwyn_list_base_address;
struct TIWYN_STRUCTURE_ZONE			*tiwyn_zone_base_address;

struct TIWYN_STRUCTURE_OBJECT			tiwyn_object_cache = { EMPTY };

struct TIWYN_STRUCTURE_OBJECT			*tiwyn_object_workbench;
struct TIWYN_STRUCTURE_OBJECT			*tiwyn_object_panel;
struct TIWYN_STRUCTURE_OBJECT			*tiwyn_object_cursor;

uint64_t tiwyn_list_limit			= 0;
uint64_t tiwyn_zone_limit			= 0;

struct TIWYN_STRUCTURE_OBJECT			*tiwyn_object_active;

int64_t tiwyn_mouse_x				= 0;
int64_t tiwyn_mouse_y				= 0;
int64_t tiwyn_mouse_z				= 0;
