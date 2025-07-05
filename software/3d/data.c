/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct LIB_WINDOW_STRUCTURE *window;

struct LIB_UI_STRUCTURE *ui;

struct LIB_RGL_STRUCTURE *rgl;

uint64_t material_limit	= TRUE;
uint64_t vector_limit	= TRUE;
uint64_t face_limit	= TRUE;

struct LIB_RGL_STRUCTURE_MATERIAL	*material = EMPTY;
vector3f				*vector = EMPTY;
struct LIB_RGL_STRUCTURE_TRIANGLE	*face = EMPTY;

uint64_t microtime;
uint64_t fps = EMPTY;
uint64_t fps_show = EMPTY;
uint64_t fps_avarage = EMPTY;
uint64_t fps_avarage_c = EMPTY;
