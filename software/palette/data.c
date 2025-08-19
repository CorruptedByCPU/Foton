/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct LIB_WINDOW_STRUCTURE *window;
struct LIB_UI_STRUCTURE *ui;

double h = 88.3f;
double s = 0.506f;
double v = 0.69f;

uint64_t ui_id_input_r = 133;
uint64_t ui_id_input_g = 175;
uint64_t ui_id_input_b = 86;
uint64_t ui_id_input_h = 88;
uint64_t ui_id_input_s = 50;
uint64_t ui_id_input_v = 68;
uint64_t ui_id_input_rgb = 0x85AF56;

uint64_t canvas_x_axis;
uint64_t canvas_y_axis;
uint64_t spectrum_y_axis;

uint64_t canvas_x_axis_previous;
uint64_t canvas_y_axis_previous;
uint64_t spectrum_y_axis_previous;

uint8_t freeze_canvas = FALSE;
uint8_t freeze_spectrum = FALSE;

uint8_t input[ 9 ] = { EMPTY };
