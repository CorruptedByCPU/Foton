/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

int64_t	wm_pid = EMPTY;

struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER kernel_framebuffer;

struct WM_STRUCTURE_OBJECT *wm_object_base_address = EMPTY;
struct WM_STRUCTURE_OBJECT **wm_list_base_address = EMPTY;
struct WM_STRUCTURE_ZONE *wm_zone_base_address = EMPTY;
struct WM_STRUCTURE_OBJECT **wm_taskbar_base_address = EMPTY;

uint64_t wm_object_limit = EMPTY;
uint64_t wm_list_limit = EMPTY;
uint64_t wm_zone_limit = EMPTY;
uint64_t wm_taskbar_limit = EMPTY;

volatile uint8_t wm_object_semaphore = FALSE;
volatile uint8_t wm_list_semaphore = FALSE;
volatile uint8_t wm_zone_semaphore = FALSE;
volatile uint8_t wm_taskbar_semaphore = FALSE;

struct WM_STRUCTURE_OBJECT wm_object_cache = { EMPTY };

struct WM_STRUCTURE_OBJECT *wm_object_workbench = EMPTY;
struct WM_STRUCTURE_OBJECT *wm_object_taskbar = EMPTY;
struct WM_STRUCTURE_OBJECT *wm_object_cursor = EMPTY;
struct WM_STRUCTURE_OBJECT *wm_object_menu = EMPTY;
struct WM_STRUCTURE_OBJECT *wm_object_hover = EMPTY;

struct WM_STRUCTURE_OBJECT *wm_object_selected = EMPTY;
struct WM_STRUCTURE_OBJECT *wm_object_active = EMPTY;
struct WM_STRUCTURE_OBJECT *wm_object_modify = EMPTY;

struct WM_STRUCTURE_ZONE wm_zone_modify = { EMPTY };

int64_t wm_mouse_x = EMPTY;
int64_t wm_mouse_y = EMPTY;
uint8_t	wm_mouse_button_left_semaphore		= FALSE;
uint8_t	wm_mouse_button_right_semaphore		= FALSE;
uint8_t	wm_mouse_button_middle_semaphore	= FALSE;

uint8_t wm_object_drag_semaphore		= FALSE;
uint8_t wm_object_hover_semaphore		= FALSE;

uint8_t wm_keyboard_status_alt_left	= FALSE;
uint8_t	wm_keyboard_status_shift_left	= FALSE;
uint8_t	wm_keyboard_status_ctrl_left	= FALSE;

// semaphore used by event/object function which tells us to refresh taskbar object content
uint8_t wm_taskbar_modified = FALSE;

// width of button on taskbar list
uint16_t wm_taskbar_entry_width = EMPTY;

uint64_t wm_taskbar_clock_time = EMPTY;

struct LIB_INTERFACE_STRUCTURE menu_interface;

MACRO_IMPORT_FILE_AS_ARRAY( menu_json, "./software/wm/menu.json" );