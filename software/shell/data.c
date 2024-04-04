/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t *shell_command;

uint8_t shell_keyboard_status_alt_left = FALSE;
uint8_t	shell_keyboard_status_shift_left = FALSE;
uint8_t	shell_keyboard_status_ctrl_left = FALSE;

FILE *dir;

uint8_t *hostname;