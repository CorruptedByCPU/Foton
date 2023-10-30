/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER kernel_framebuffer;

uint64_t fps = 0;
uint64_t fps_average = 0;
uint64_t fps_count = 1;
uint64_t fps_last = 0;

struct LIB_RGL_STRUCTURE *rgl;

uint8_t string_material[ 6 ] = "newmtl";
uint8_t string_material_change[ 6 ] = "usemtl";
uint8_t	string_kd[ 2 ] = "Kd";

// amount of materials
uint64_t m = 1;
uint64_t mc = 1;

struct LIB_RGL_STRUCTURE_MATERIAL *material;

// amount of vectors and faces inside object file
uint64_t v = 1;
uint64_t vc = 1;
uint64_t f = 1;
uint64_t fc = 1;

vector3f *vector;
vector3f *vr;
vector3f *vp;
struct LIB_RGL_STRUCTURE_TRIANGLE *face;

struct STD_WINDOW_STRUCTURE_DESCRIPTOR *descriptor = EMPTY;

MACRO_IMPORT_FILE_AS_ARRAY( object, "./root/system/var/3d.obj" );
MACRO_IMPORT_FILE_AS_ARRAY( material, "./root/system/var/3d.mtl" );
