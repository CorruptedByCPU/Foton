/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct WM_STRUCTURE_OBJECT *wm_object_create( uint16_t x, uint16_t y, uint16_t width, uint16_t height );

struct WM_STRUCTURE_OBJECT *wm_object_find( uint16_t x, uint16_t y, uint8_t hidden );

void wm_object_insert( struct WM_STRUCTURE_OBJECT *object );
