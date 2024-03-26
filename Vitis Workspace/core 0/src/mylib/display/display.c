#include "display.h"


int _max(int num1, int num2) {
	return (num1>num2)?num1:num2;
}

int _min(int num1, int num2) {
	return (num1<num2)?num1:num2;
}

void updateDisp(int* img){
	memcpy((int*)VGA_BUFF_ADDR, img, DISP_SIZE_BYTES);
	Xil_DCacheFlushRange(VGA_BUFF_ADDR,  DISP_SIZE_BYTES);
}

void drawRectFill(int *img, int x_origin, int y_origin, int width, int height, int color) {
	for(int i = _max(0, y_origin); i < _min(y_origin + height, DISP_HEIGHT); i++){
		for(int j = _max(0, x_origin); j< _min(x_origin +  width, DISP_WIDTH); j++) {
			img[i*DISP_WIDTH + j] = color;
		}
	}
}


void drawRectBorder(int *img, int x_origin, int y_origin, int width, int height, int color, int size) {
	//draw top
	drawRectFill(img, x_origin , y_origin , width, size, color);
	//draw bottom
	drawRectFill(img, x_origin , y_origin + height - size, width, size, color);
	//draw left
	drawRectFill(img, x_origin , y_origin, size, height, color);
	//draw right
	drawRectFill(img, x_origin + width - size , y_origin, size, height, color);
}

void drawCursor(int *img, int x_origin, int y_origin, int width, int height, int color, int size) {
	//draw top
	drawRectFill(img, x_origin - size/2 , y_origin - height  , size, height, color);
	//draw bottom
	drawRectFill(img, x_origin - size/2 , y_origin + 1, size, height, color);
	//draw left
	drawRectFill(img, x_origin - width  , y_origin - size/2, width, size, color);
	//draw right
	drawRectFill(img, x_origin + 1 , y_origin - size/2, width, size, color);
}

void drawChar(int* img, int x_origin, int y_origin, int scale_x, int scale_y, char c, int color) {
	int idx_c = (int)c;
	for(int i = 0; i < FONT_H*scale_y; i++){
		int img_y = _min(_max(0, y_origin+i), DISP_HEIGHT);
		int val = font[idx_c][i/scale_y];
		for(int j = 0; j<FONT_W*scale_x; j++) {
			int img_x = _min(_max(0, x_origin+j), DISP_WIDTH);
			int val1 = (1<<(j/scale_x));
			int res = val & val1;
			img[img_y*DISP_WIDTH + img_x] = res ? color : img[img_y*DISP_WIDTH + img_x];
		}
	}
}

void drawStr(int* img, int x_origin, int y_origin, int scale_x, int scale_y, const char* str, int len, int color) {
	for (int l =0; l < len; l++) {
		drawChar(img, x_origin + l*FONT_W*scale_x, y_origin, scale_x, scale_y, str[l], color);
	}
}

void drawXBM(int* img, int x_origin, int y_origin, const char* logo, int color) {
	for(int i = 0; i < LOGO_H; i++) {
		int img_y = _min(_max(0, y_origin+i), DISP_HEIGHT);
		for(int j = 0; j < LOGO_W/8; j++) {
			int val = logo[i*(LOGO_W/8) + j];
			for(int jj = 0; jj < 8; jj++){
				int img_x = _min(_max(0, x_origin+j*8+jj), DISP_WIDTH);
				int res = val & (1 << (7-jj));
				img[img_y*DISP_WIDTH + img_x] = res ? color : img[img_y*DISP_WIDTH + img_x];
			}
		}
	}
}

int getColor(float val, float min, float max) {
	float tmp = (val - min)/(max - min);
	int i = (int)(tmp * 63);
	i = _max(_min(i, 63), 0);
	int res = colormap[i];
	return res;
}
