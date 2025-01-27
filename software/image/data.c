/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

MACRO_IMPORT_FILE_AS_ARRAY( interface, "./software/image/interface.json" );

struct LIB_INTERFACE_STRUCTURE			*image_interface;

uint64_t image_width = EMPTY;
uint64_t image_height = EMPTY;

uint32_t *image_pixel = EMPTY;

// uint64_t image_window_width = EMPTY;
// uint64_t image_window_height = EMPTY;

// struct STD_STRUCTURE_WINDOW_DESCRIPTOR *image_descriptor = EMPTY;