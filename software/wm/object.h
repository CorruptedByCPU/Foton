/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct WM_STRUCTURE_OBJECT *wm_object_create( uint16_t x, uint16_t y, uint16_t width, uint16_t height );

void wm_object_activate( void );

struct WM_STRUCTURE_OBJECT *wm_object_find( uint16_t x, uint16_t y, uint8_t hidden );

void wm_object_insert( struct WM_STRUCTURE_OBJECT *object );

void wm_object_move( int16_t x, int16_t y );

uint8_t wm_object_move_up( struct WM_STRUCTURE_OBJECT *object );
