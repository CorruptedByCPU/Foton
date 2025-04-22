/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint64_t wm_pid;

struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER wm_framebuffer;

struct WM_STRUCTURE_OBJECT *wm_object_base_address;
struct WM_STRUCTURE_OBJECT **wm_list_base_address;
struct WM_STRUCTURE_ZONE *wm_zone_base_address;

struct WM_STRUCTURE_OBJECT wm_object_cache = { EMPTY };

struct WM_STRUCTURE_OBJECT *wm_object_workbench;
struct WM_STRUCTURE_OBJECT *wm_object_cursor;

struct WM_STRUCTURE_OBJECT *wm_object_active = EMPTY;
struct WM_STRUCTURE_OBJECT *wm_object_hover = EMPTY;
struct WM_STRUCTURE_OBJECT *wm_object_selected = EMPTY;
struct WM_STRUCTURE_OBJECT *wm_object_resize = EMPTY;

struct WM_STRUCTURE_ZONE wm_zone_modify = { EMPTY };

uint64_t wm_object_limit = EMPTY;
uint64_t wm_list_limit = EMPTY;
uint64_t wm_zone_limit = EMPTY;

int64_t wm_mouse_x = INIT;
int64_t wm_mouse_y = INIT;
int64_t wm_mouse_z = INIT;

uint8_t wm_object_drag_allow = FALSE;
uint8_t wm_object_resize_init = FALSE;

uint8_t wm_mouse_button_left = FALSE;
uint8_t wm_mouse_button_right = FALSE;

uint8_t wm_keyboard_alt_left = FALSE;
