#include "xil_io.h"
#include "xil_cache.h"

#include "../constants.h"
#include "colors.h"
#include "font.h"
#include "logo.h"

#define DISP_WIDTH 1280
#define DISP_HEIGHT 1024
#define DISP_SIZE_BYTES (DISP_WIDTH * DISP_HEIGHT) * sizeof(int)

int _max(int num1, int num2);
int _min(int num1, int num2);
void updateDisp(int* img);
void drawRectFill(int* img, int x_origin, int y_origin, int width, int height, int color);
void drawRectBorder(int* img, int x_origin, int y_origin, int width, int height, int color, int size);
void drawCursor(int* img, int x_origin, int y_origin, int width, int height, int color, int size);
void drawStr(int* img, int x_origin, int y_origin, int scale_x, int scale_y, const char* str, int len, int color);
void drawChar(int* img, int x_origin, int y_origin, int scale_x, int scale_y, char c, int color);
void drawXBM(int* img, int x_origin, int y_origin, const char* logo, int color);
int getColor(float i, float min, float max);
