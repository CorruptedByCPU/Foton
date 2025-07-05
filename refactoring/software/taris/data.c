/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

MACRO_IMPORT_FILE_AS_ARRAY( interface, "./software/taris/interface.json" )

struct LIB_INTERFACE_STRUCTURE	taris_interface;

struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *taris_points;
struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *taris_lines;
struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *taris_level;
struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *taris_game_over;
struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *taris_options;

struct LIB_RGL_STRUCTURE *taris_rgl = EMPTY;

uint64_t taris_brick[ 7 ] = {
	0x0262007202320270,	// T
	0x0226007103220470,	// J
	0x0264063002640630,	// Z
	0x0330033003300330,	// O
	0x0462036004620360,	// S
	0x0622007402230170,	// L
	0x022220F00222200F0	// I
};

uint32_t taris_color[ 8 ] = {
	0xFFFF0000,	// T
	0xFFFFDB00,	// J
	0xFF49FF00,	// Z
	0xFF00FF92,	// O
	0xFF0092FF,	// S
	0xFF4900FF,	// L
	0xFFFF00DB,	// I
	0x80000000
};

uint16_t taris_speed[] = { 819, 734, 649, 563, 478, 393, 307, 222, 137, 102, 85, 85, 85, 68, 68, 68, 51, 51, 51, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 17 };

uint16_t taris_score[ 4 ] = { 40, 100, 300, 1200 };

uint16_t taris_difficult[] = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 100, 100, 100, 100, 100, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200, 200, 200, 200 };

uint16_t taris_playground[ TARIS_PLAYGROUND_HEIGHT_brick ] = { TARIS_PLAYGROUND_empty, TARIS_PLAYGROUND_empty, TARIS_PLAYGROUND_empty, TARIS_PLAYGROUND_empty, TARIS_PLAYGROUND_empty, TARIS_PLAYGROUND_empty, TARIS_PLAYGROUND_empty, TARIS_PLAYGROUND_empty, TARIS_PLAYGROUND_empty, TARIS_PLAYGROUND_empty, TARIS_PLAYGROUND_empty, TARIS_PLAYGROUND_empty, TARIS_PLAYGROUND_empty, TARIS_PLAYGROUND_empty, TARIS_PLAYGROUND_empty, TARIS_PLAYGROUND_empty, TARIS_PLAYGROUND_empty, TARIS_PLAYGROUND_empty, TARIS_PLAYGROUND_empty, TARIS_PLAYGROUND_empty, TARIS_PLAYGROUND_empty, 0xFFFF };

uint8_t taris_playground_color[ TARIS_PLAYGROUND_WIDTH_bit * TARIS_PLAYGROUND_HEIGHT_brick ];

uint64_t taris_brick_selected;
uint8_t taris_brick_selected_id;
uint64_t taris_brick_selected_color;
int64_t taris_brick_selected_x;
int64_t taris_brick_selected_y;

uint8_t taris_play = TRUE;
