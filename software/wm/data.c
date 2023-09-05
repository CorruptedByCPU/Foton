/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

int64_t	wm_pid = EMPTY;

struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER framebuffer;

struct WM_STRUCTURE_OBJECT *wm_object_base_address = EMPTY;
struct WM_STRUCTURE_OBJECT **wm_list_base_address = EMPTY;
struct WM_STRUCTURE_ZONE *wm_zone_base_address = EMPTY;

uint64_t wm_object_limit = EMPTY;
uint64_t wm_list_limit = EMPTY;
uint64_t wm_zone_limit = EMPTY;

uint8_t wm_object_semaphore = FALSE;
uint8_t wm_list_semaphore = FALSE;
uint8_t	wm_framebuffer_semaphore = FALSE;

struct WM_STRUCTURE_OBJECT *wm_object_workbench = EMPTY;
struct WM_STRUCTURE_OBJECT *wm_object_cursor = EMPTY;
struct WM_STRUCTURE_OBJECT *wm_object_active = EMPTY;