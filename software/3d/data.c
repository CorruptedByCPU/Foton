/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

// properties of current display
struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER kernel_framebuffer;

// if 3D Viewer is executed directly by Kernel, this value will change to TRUE
uint8_t the_master_of_puppets = FALSE;

// window interface
MACRO_IMPORT_FILE_AS_ARRAY( interface, "./software/3d/interface.json" );
struct LIB_INTERFACE_STRUCTURE	*d3_interface;

// properties of RGL library
struct LIB_RGL_STRUCTURE *rgl;

// amount of materials, vectors and faces
uint64_t material_limit	= TRUE;
uint64_t vector_limit	= TRUE;
uint64_t face_limit	= TRUE;

// list of materials, vectors and faces
struct LIB_RGL_STRUCTURE_MATERIAL	*material = EMPTY;
vector3f				*vector = EMPTY;
struct LIB_RGL_STRUCTURE_TRIANGLE	*face = EMPTY;