/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct STD_STRUCTURE_STREAM_META stream_meta;
FILE *file = EMPTY;

uint8_t *file_path = EMPTY;
uint8_t *save_as = EMPTY;

uint8_t *document_name = EMPTY;

uint8_t *document_area = EMPTY;
uint64_t document_size = 0;

uint64_t document_line_location = 0;		// index: line beginning inside document
uint64_t document_line_pointer = 0;		// index: current character inside line
uint64_t document_line_pointer_saved = 0;
uint64_t document_line_indicator = 0;		// index: show line from selected character
uint64_t document_line_indicator_saved = 0;
uint64_t document_line_number = 0;		// show document from this line number
uint64_t document_line_count = 0;
uint64_t document_line_size = 0;

uint64_t document_cursor_x = 0;
uint64_t document_cursor_y = 0;

uint8_t document_modified_semaphore = FALSE;

uint8_t menu_height_line = 2;
uint8_t string_menu[] = "\e[48;5;15m\e[38;5;0m^x\e[0m Exit %s^o\e[0m Save";

uint8_t string_color_default[] = "\e[0m";
uint8_t string_color_modified[] = "\e[48;5;9m\e[38;5;15m";
uint8_t string_color_shortcut[] = "\e[48;5;15m\e[38;5;0m";

uint8_t key_ctrl_semaphore = FALSE;

uint8_t string_cursor_at_menu[ 42 + 4 + 1 ] = { EMPTY };
uint8_t string_cursor_at_interaction[ 42 + 4 + 1 ] = { EMPTY };