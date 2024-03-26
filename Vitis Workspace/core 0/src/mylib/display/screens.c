#include "screens.h"

extern volatile int zoom, panX, panY;
extern volatile int pos;
extern volatile int sel;
extern volatile int iter0, iter1, iter2, iter3, iter4;
extern volatile int usehw;
extern volatile int iterations;
extern volatile int SIM_DONE;

static volatile int* comp_status = (int*)COMP_STATUS;
static volatile int* startR = (int*)START_ROW_IDX;
static volatile int* startC = (int*)START_COL_IDX;
static volatile int* endR = (int*)END_ROW_IDX;
static volatile int* endC = (int*)END_COL_IDX;
extern volatile int selectedBlock;

extern volatile int pointX;
extern volatile int pointY;
extern volatile int stepXY;

float global_max = 0.0;
float global_min = 1000.0;

void drawColorBar(int* image, int x, int y, int w, int h, float min,float max){
	int min_pos, max_pos;
	char buff[10];
	sprintf(buff, "%.2fK", min);
	min_pos = x - ((strlen(buff) + 1) * FONT_W * 2);
	max_pos =  (x + w*64) +  FONT_W * 2;
	drawStr(image, min_pos, y,  2, 4, buff, strlen(buff), white);
	sprintf(buff, "%.2fK", max);
	drawStr(image, max_pos, y,  2, 4, buff, strlen(buff), white);

	for(int j = 0; j < 64; j++) {
		int color = colormap[j];
		drawRectFill(image, x + j*w, y , w, h, color);
	}

	drawRectBorder(image, x, y, w*64, h, white, 4);
}

void show_welcome_screen(int* image) {
	drawRectFill(image, 0, 0, DISP_WIDTH, DISP_HEIGHT, black);
	drawRectBorder(image,  30, 30, DISP_WIDTH - 60, DISP_HEIGHT -  60, white, 10 );
	drawXBM(image, 420, 350, logo, white);
	drawStr(image, 440, 600,  2, 4, "Thermal Simulation Tool", 24, white);
	updateDisp(image);
}

void show_wait_screen(int* image) {
	drawRectFill(image, 0, 0, DISP_WIDTH, DISP_HEIGHT, black);
	drawStr(image, 500, 600,  2, 4, "Waiting for Data...", 19, white);
	updateDisp(image);
}

void show_data_screen(int* image) {
	drawRectFill(image, 0, 0, DISP_WIDTH, DISP_HEIGHT, black);
	char buff[30];
	sprintf(buff, "Data Preview");
	drawStr(image, 520, 300,  2, 4, buff, strlen(buff), green);
	sprintf(buff, "BTN U : View Temperature Data");
	drawStr(image, 400, 400,  2, 4, buff, strlen(buff), white);
	sprintf(buff, "BTN C : View Power Data");
	drawStr(image, 400, 450,  2, 4, buff, strlen(buff), white);
	sprintf(buff, "BTN D : Next");
	drawStr(image, 400, 500,  2, 4, buff, strlen(buff), cyan);
	sprintf(buff, "BTN 8 : Restart");
	drawStr(image, 400, 550,  2, 4, buff, strlen(buff), red);
	updateDisp(image);
}

void show_iter_screen(int* image) {
	drawRectFill(image, 0, 0, DISP_WIDTH, DISP_HEIGHT, black);
	char buff[25];
	sprintf(buff, "Enter Details");
	drawStr(image, 520, 300,  2, 4, buff, strlen(buff), green);
	sprintf(buff, "Iterations : ");
	drawStr(image, 470, 450,  2, 4, buff, strlen(buff), white);
	int startX = 470 + strlen(buff)*FONT_W*2;
	int myArr[5] = {iter4, iter3, iter2, iter1, iter0};
	for (int l =0; l < 5; l++) {
		int color = ((4-l)==pos && !sel)?pink:white;
		char c[5];
		sprintf(c, "%d", myArr[l]);
		drawChar(image, startX + l*FONT_W*2, 450, 2, 4, c[0] , color);
	}
	sprintf(buff, ">");
	if (sel)
		drawStr(image, 440, 500,  4, 4, buff, strlen(buff), white);
	else
		drawStr(image, 440, 450,  4, 4, buff, strlen(buff), white);

	sprintf(buff, "Platform   : ");
	drawStr(image, 470, 500,  2, 4, buff, strlen(buff), white);
	startX = 470 + (strlen(buff) + 3)*FONT_W*2;
	int color = (sel) ? pink : white;
	if (usehw)
		sprintf(buff, "HW");
	else
		sprintf(buff, "SW");

	drawStr(image, startX, 500,  2, 4, buff, strlen(buff), color);
	sprintf(buff, "BTN8: Back \t BTN9: Start");
	drawStr(image, 430, 800,  2, 4, buff, strlen(buff), red);
	updateDisp(image);
}

void show_temp_preview(int* image) {
	float* temp_rcvd =  (float*)TEMP_RECD;
	drawRectFill(image, 0, 0, DISP_WIDTH, DISP_HEIGHT, black);
	char buff[25];
	sprintf(buff, "Temperature Preview");
	drawStr(image, 500, 100,  2, 4, buff, strlen(buff), white);
	int size = 1;
	int nTiles = INPUT_SIZE;
	int posX = 380;
	int posY = 200;
	int border = 4;
	drawRectBorder(image,  posX - border, posY - border, nTiles*size+2*border, nTiles*size+2*border, white, border );
	float min = 1000.0;
	float max = 0.0;
//	xil_printf("Temperature Data\n");
	for(int i = 0; i < nTiles; i++) {
		for(int j = 0; j < nTiles; j++) {
//			xil_printf("%2d\t", (int)temp_in[i * nTiles + j]);
			min = temp_rcvd[i* nTiles +j] < min ? temp_rcvd[i* nTiles +j] : min;
			max = temp_rcvd[i* nTiles +j] > max ? temp_rcvd[i* nTiles +j] : max;
			}
//		xil_printf("\n");
	}
	for(int i = 0; i < nTiles; i++) {
		for(int j = 0; j < nTiles; j++) {
			int color = getColor(temp_rcvd[i* nTiles +j], min, max);
			drawRectFill(image, posX + j*size, posY + i*size , size, size, color);
		}
	}
	sprintf(buff, "BTN C : Go Back");
	drawStr(image, 510, 800,  2, 4, buff, strlen(buff), red);
	updateDisp(image);
}

void show_pow_preview(int* image) {
	float* power_rcvd = (float*)POWER_RECD;
	drawRectFill(image, 0, 0, DISP_WIDTH, DISP_HEIGHT, black);
	char buff[25];
	sprintf(buff, "Power Preview");
	drawStr(image, 540, 100,  2, 4, buff, strlen(buff), white);
	int size = 1;
	int nTiles = INPUT_SIZE;
	int posX = 380;
	int posY = 200;
	int border = 4;
	drawRectBorder(image,  posX - border, posY - border, nTiles*size+2*border, nTiles*size+2*border, white, border );
	float min = 1000.0;
	float max = 0.0;
	xil_printf("Power Data\n");
	for(int i = 0; i < nTiles; i++) {
		for(int j = 0; j < nTiles; j++) {
//			xil_printf("%2d\t", (int)temp_in[i * nTiles + j]);
			min = power_rcvd[i* nTiles +j] < min ? power_rcvd[i* nTiles +j] : min;
			max = power_rcvd[i* nTiles +j] > max ? power_rcvd[i* nTiles +j] : max;
			}
//		xil_printf("\n");
	}
	for(int i = 0; i < nTiles; i++) {
		for(int j = 0; j < nTiles; j++) {
			int color = getColor(power_rcvd[i* nTiles +j], min, max);
			drawRectFill(image, posX + j*size, posY + i*size , size, size, color);
		}
	}
	sprintf(buff, "BTN C : Go Back");
	drawStr(image, 510, 800,  2, 4, buff, strlen(buff), red);
	updateDisp(image);
}

void show_sim_screen(int* image, int i, int sw) {
	int scale, offX, offY;
	scale = zoom;
	offX = panX;
	offY = panY;
	float* temp_rcvd =  usehw?(float*)HW_TEMP_BUFF:(float*)TEMP_IN;
	Xil_DCacheFlushRange((INTPTR)temp_rcvd,  INPUT_SIZE*INPUT_SIZE*sizeof(float));
	drawRectFill(image, 0, 0, DISP_WIDTH, DISP_HEIGHT, black);
	char buff[30];
	sprintf(buff, "Simulation");
	drawStr(image, 560, 60,  2, 4, buff, strlen(buff), white);
	int size = 1 << scale;
	int nTiles = INPUT_SIZE/size;
	int posX = 380;
	int posY = 250;
	int border = 4;
	drawRectBorder(image,  posX - border, posY - border, INPUT_SIZE+2*border, INPUT_SIZE+2*border, white, border );

	int startX, startY, cenX, cenY;
	cenX = (INPUT_SIZE>>1) + offX;
	cenY = (INPUT_SIZE>>1) + offY;
	startX = cenX - (INPUT_SIZE >> (scale+1));
	startY = cenY - (INPUT_SIZE >> (scale+1));

	for(int i = 0; i < INPUT_SIZE; i++) {
		for(int j = 0; j < INPUT_SIZE; j++) {
//			xil_printf("%2d\t", (int)temp_in[i * nTiles + j])
			global_min = temp_rcvd[i* INPUT_SIZE +j] < global_min ? temp_rcvd[i*INPUT_SIZE +j] : global_min;
			global_max = temp_rcvd[i* INPUT_SIZE +j] > global_max ? temp_rcvd[INPUT_SIZE +j] : global_max;
			}

//		xil_printf("\n");
	}
	float min = 1000.0;
	float max = 0.0;
	for(int i = 0; i < nTiles; i++) {
		for(int j = 0; j < nTiles; j++) {
//			xil_printf("%2d\t", (int)temp_in[i * nTiles + j]);
			min = temp_rcvd[(startY +i)* INPUT_SIZE +(startX + j)] < min ? temp_rcvd[(startY +i)*INPUT_SIZE +(startX + j)] : min;
			max = temp_rcvd[(startY +i)* INPUT_SIZE +(startX + j)] > max ? temp_rcvd[(startY +i)*INPUT_SIZE +(startX + j)] : max;
		}
	}

	min = sw?min:global_min;
	max = sw?max:global_max;

	for(int i = 0; i < nTiles; i++) {
		for(int j = 0; j < nTiles; j++) {
			int color = getColor(temp_rcvd[(startY +i)* INPUT_SIZE + (startX + j)], min, max);
			drawRectFill(image, posX + j*size, posY + i*size , size, size, color);
		}
	}


	drawColorBar(image, (DISP_WIDTH - INPUT_SIZE)/2-4, 150, INPUT_SIZE/64, 32, min, max);


	float progress = (i*100.0)/iterations;

	sprintf(buff, "Zoom : %dx\t \t Center : (%d,%d)", size, cenX, cenY);
	drawStr(image, 400, 780,  2, 4, buff, strlen(buff), white);
	sprintf(buff, "Progress: %.1f%%", progress);
	drawStr(image, 510, 850,  2, 4, buff, strlen(buff), green);
//	int* com = (int*)INTER_CPU_FLAG;
//	if (i>=iterations && !(*com)){
//		u64 count = counter64_value();
//		float time = (float)count / COUNTER_CLK;
//		sprintf(buff, "Elapsed Time: %.3f s", time);
//		drawStr(image, 446, 850,  2, 4, buff, strlen(buff), cyan);
//	}
	if (SIM_DONE)
		sprintf(buff, "BTNC: SEND");
	else
		sprintf(buff, "BTNC: PAUSE");
	drawStr(image, 550, 950,  2, 4, buff, strlen(buff), red);
	updateDisp(image);
}

void show_pause_screen(int* image) {
	drawRectFill(image, 0, 0, DISP_WIDTH, DISP_HEIGHT, black);
	char buff[30];
	sprintf(buff, "Simulation Paused");
	drawStr(image, 490, 300,  2, 4, buff, strlen(buff), green);
	sprintf(buff, "BTN U : Power Mode");
	drawStr(image, 480, 400,  2, 4, buff, strlen(buff), white);
	sprintf(buff, "BTN C : Point Mode");
	drawStr(image, 480, 450,  2, 4, buff, strlen(buff), white);
	sprintf(buff, "BTN D : Resume");
	drawStr(image, 480, 500,  2, 4, buff, strlen(buff), cyan);
	sprintf(buff, "BTN 8 : Quit");
	drawStr(image, 480, 550,  2, 4, buff, strlen(buff), red);
	updateDisp(image);
}

void show_end_screen(int* image, int sw) {
	float* temp_rcvd =  (float*)TEMP_SEND;
	Xil_DCacheFlushRange((INTPTR)temp_rcvd,  INPUT_SIZE*INPUT_SIZE*sizeof(float));
	drawRectFill(image, 0, 0, DISP_WIDTH, DISP_HEIGHT, black);
	char buff[30];
	sprintf(buff, "Run Completed");
	drawStr(image, 540, 60,  2, 4, buff, strlen(buff), white);
	int nTiles = INPUT_SIZE;
	int posX = 380;
	int posY = 250;
	int border = 4;
	drawRectBorder(image,  posX - border, posY - border, INPUT_SIZE+2*border, INPUT_SIZE+2*border, white, border );


	float min = 1000.0;
	float max = 0.0;
	for(int i = 0; i < nTiles; i++) {
		for(int j = 0; j < nTiles; j++) {
//			xil_printf("%2d\t", (int)temp_in[i * nTiles + j]);
			min = temp_rcvd[(i)* INPUT_SIZE +(j)] < min ? temp_rcvd[(i)*INPUT_SIZE +(j)] : min;
			max = temp_rcvd[(+i)* INPUT_SIZE +(j)] > max ? temp_rcvd[(i)*INPUT_SIZE +(j)] : max;
		}
	}

	min = sw?min:global_min;
	max = sw?max:global_max;
	for(int i = 0; i < nTiles; i++) {
		for(int j = 0; j < nTiles; j++) {
			int color = getColor(temp_rcvd[(i)* INPUT_SIZE + (j)], min, max);
			drawRectFill(image, posX + j, posY + i , 1, 1, color);
		}
	}


	drawColorBar(image, (DISP_WIDTH - INPUT_SIZE)/2-4, 150, INPUT_SIZE/64, 32, min, max);

	u64 count = counter64_value();
	float time = (float)count / COUNTER_CLK;
	sprintf(buff, "Computation Time: %.3f s", time);
	drawStr(image, 440, 850,  2, 4, buff, strlen(buff), cyan);

	sprintf(buff, "BTNC: FINISH");
	drawStr(image, 550, 950,  2, 4, buff, strlen(buff), red);
	updateDisp(image);
}

void show_point_screen(int* image, int sw) {
	float* temp_rcvd =  usehw?(float*)HW_TEMP_BUFF:(float*)TEMP_IN;
	Xil_DCacheFlushRange((INTPTR)temp_rcvd,  INPUT_SIZE*INPUT_SIZE*sizeof(float));
	drawRectFill(image, 0, 0, DISP_WIDTH, DISP_HEIGHT, black);
	char buff[30];
	sprintf(buff, "Point Mode");
	drawStr(image, 540, 60,  2, 4, buff, strlen(buff), white);
	int nTiles = INPUT_SIZE;
	int posX = 380;
	int posY = 250;
	int border = 4;
	drawRectBorder(image,  posX - border, posY - border, INPUT_SIZE+2*border, INPUT_SIZE+2*border, white, border );

	float min = 1000.0;
	float max = 0.0;
	for(int i = 0; i < nTiles; i++) {
		for(int j = 0; j < nTiles; j++) {
//			xil_printf("%2d\t", (int)temp_in[i * nTiles + j]);
			min = temp_rcvd[(i)* INPUT_SIZE +(j)] < min ? temp_rcvd[(i)*INPUT_SIZE +(j)] : min;
			max = temp_rcvd[(+i)* INPUT_SIZE +(j)] > max ? temp_rcvd[(i)*INPUT_SIZE +(j)] : max;
		}
	}

	min = sw?min:global_min;
	max = sw?max:global_max;
	for(int i = 0; i < nTiles; i++) {
		for(int j = 0; j < nTiles; j++) {
			int color = getColor(temp_rcvd[(i)* INPUT_SIZE + (j)], min, max);
			drawRectFill(image, posX + j, posY + i , 1, 1, color);
		}
	}

	drawColorBar(image, (DISP_WIDTH - INPUT_SIZE)/2-4, 150, INPUT_SIZE/64, 32, min, max);
	drawCursor(image, posX+pointX, posY+pointY, 10, 10, pink, 8);
	sprintf(buff, "Step : %d\t\tPosition : (%d,%d)", stepXY, pointX, pointY);
	drawStr(image, 400, 780,  2, 4, buff, strlen(buff), white);

	float temp = temp_rcvd[pointY * INPUT_SIZE + pointX];
	sprintf(buff, "Temperature: %.3f K", temp);
	float tmp = (temp - min)/(max - min);
	int c_i = (int)(tmp * 63);
	c_i = _max(_min(c_i, 63), 0);
	if (c_i < 16 || c_i >= 48)
		drawRectFill(image, 440 - (FONT_W * 2), 850 - 4 , (strlen(buff) + 2 )*FONT_W*2, (FONT_H + 2)* 4, white);
	drawStr(image, 440, 850,  2, 4, buff, strlen(buff), colormap[c_i]);

	sprintf(buff, "BTNC: BACK");
	drawStr(image, 550, 950,  2, 4, buff, strlen(buff), red);
	updateDisp(image);
}

void show_comp_screen(int* image) {
	float* temp_rcvd =  (float*)POWER_IN;
	Xil_DCacheFlushRange((INTPTR)temp_rcvd,  INPUT_SIZE*INPUT_SIZE*sizeof(float));
	Xil_DCacheFlushRange((INTPTR)comp_status,  COMP_NO*4);
	drawRectFill(image, 0, 0, DISP_WIDTH, DISP_HEIGHT, black);
	char buff[30];
	sprintf(buff, "Power Mode");
	drawStr(image, 540, 60,  2, 4, buff, strlen(buff), white);
	int nTiles = INPUT_SIZE;
	int posX = 680;
	int posY = 250;
	int border = 4;
	drawRectBorder(image,  posX - border, posY - border, INPUT_SIZE+2*border, INPUT_SIZE+2*border, white, border );

	float min = 1000.0;
	float max = 0.0;

	for(int i = 0; i < nTiles; i++) {
		for(int j = 0; j < nTiles; j++) {
//			xil_printf("%2d\t", (int)temp_in[i * nTiles + j]);
			min = temp_rcvd[(i)* INPUT_SIZE +(j)] < min ? temp_rcvd[(i)*INPUT_SIZE +(j)] : min;
			max = temp_rcvd[(+i)* INPUT_SIZE +(j)] > max ? temp_rcvd[(i)*INPUT_SIZE +(j)] : max;
		}
	}

	for(int i = 0; i < nTiles; i++) {
		for(int j = 0; j < nTiles; j++) {
			int color = getColor(temp_rcvd[(i)* INPUT_SIZE + (j)], min, max);
			drawRectFill(image, posX + j, posY + i , 1, 1, color);
		}
	}

	int StartX = 100;
	int StartY = 200;
	int l = 20;
	for(int i = 0; i < COMP_NO; i++) {


		if (i==selectedBlock) {
			drawRectFill(image, StartX - (FONT_W * 2), StartY + (i *(FONT_H + 4)*4) - 4 , (l+10)*FONT_W*2, (FONT_H + 2)* 4, blue);
			int x_o = startC[i];
			int y_o = startR[i];
			int x_e = endC[i];
			int y_e = endR[i];
			int w = x_e - x_o;
			int h = y_e - y_o;
			drawRectBorder(image,  posX + x_o - 5, posY + y_o - border, w+2*border, h+2*border, white, border );
		}

		sprintf(buff, "Power Block %d: ", i);
		drawStr(image, StartX, StartY + (i *(FONT_H+4)*4), 2, 4, buff, strlen(buff), white);
		if (comp_status[i]) {
			sprintf(buff, "ON");
			drawStr(image, StartX + (l+2)*FONT_W*2, StartY + (i *(FONT_H+4)*4), 2, 4, buff, strlen(buff), green);
		}
		else {
			sprintf(buff, "OFF");
			drawStr(image, StartX + (l+1)*FONT_W*2, StartY + (i *(FONT_H+4)*4), 2, 4, buff, strlen(buff), red);
		}
	}

	sprintf(buff, "BTNC: BACK");
	drawStr(image, 550, 950,  2, 4, buff, strlen(buff), red);
	updateDisp(image);
}
