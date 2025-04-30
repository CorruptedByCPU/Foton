/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct TIWYN_STRUCTURE_OBJECT *tiwyn_object_create( uint16_t x, uint16_t y, uint16_t width, uint16_t height );

struct TIWYN_STRUCTURE_OBJECT *tiwyn_object_find( uint16_t x, uint16_t y, uint8_t hidden );

void tiwyn_object_insert( struct TIWYN_STRUCTURE_OBJECT *object );

void tiwyn_object_move( int16_t x, int16_t y );

uint8_t wm_object_move_up( struct TIWYN_STRUCTURE_OBJECT *object );
