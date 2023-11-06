/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

int64_t	wm_pid = EMPTY;

struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER kernel_framebuffer;

struct WM_STRUCTURE_OBJECT *wm_object_base_address = EMPTY;
struct WM_STRUCTURE_OBJECT **wm_list_base_address = EMPTY;
struct WM_STRUCTURE_ZONE *wm_zone_base_address = EMPTY;

uint64_t wm_object_limit = EMPTY;
uint64_t wm_list_limit = EMPTY;
uint64_t wm_zone_limit = EMPTY;

uint8_t wm_object_semaphore = FALSE;
uint8_t wm_list_semaphore = FALSE;

struct WM_STRUCTURE_OBJECT wm_object_cache = { EMPTY };

struct WM_STRUCTURE_OBJECT *wm_object_workbench = EMPTY;
struct WM_STRUCTURE_OBJECT *wm_object_taskbar = EMPTY;
struct WM_STRUCTURE_OBJECT *wm_object_cursor = EMPTY;

struct WM_STRUCTURE_OBJECT *wm_object_selected = EMPTY;
struct WM_STRUCTURE_OBJECT *wm_object_active = EMPTY;

int64_t wm_mouse_x = EMPTY;
int64_t wm_mouse_y = EMPTY;
uint8_t	wm_mouse_button_left_semaphore		= FALSE;
uint8_t	wm_mouse_button_right_semaphore		= FALSE;
uint8_t	wm_mouse_button_middle_semaphore	= FALSE;

uint8_t wm_keyboard_status_alt_left = FALSE;
uint8_t	wm_keyboard_status_shift_left = FALSE;

MACRO_IMPORT_FILE_AS_ARRAY( wallpaper, "./root/system/var/wallpaper.tga" );
MACRO_IMPORT_FILE_AS_ARRAY( cursor, "./root/system/var/cursor.tga" );