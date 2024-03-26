#include "display.h"
#include "stdio.h"
#include "../counter/counter.h"

void show_welcome_screen(int* image);
void show_wait_screen(int* image);
void show_data_screen(int* image);
void show_iter_screen(int* image);
void show_temp_preview(int* image);
void show_pow_preview(int* image);
void show_sim_screen(int* image, int i, int sw);
void show_pause_screen(int* image);
void show_comp_screen(int* image);
void show_point_screen(int* image, int sw);
void show_end_screen(int* image, int sw);
