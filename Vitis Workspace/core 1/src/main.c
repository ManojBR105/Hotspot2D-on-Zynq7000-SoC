/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xparameters.h"
#include "xil_printf.h"
#include "sleep.h"
#include "xil_cache.h"
#include "xaxidma.h"
#include "counter/counter.h"
#include "hotspot_computation.h"

#define ARR_SIZE_BYTES (INPUT_SIZE * INPUT_SIZE * sizeof(float))

static volatile int* com = (int*)INTER_CPU_FLAG;
static volatile int* num_iter = (int*)NUM_ITER;

static volatile int* comp_status = (int*)COMP_STATUS;
static volatile int* startX = (int*)START_ROW_IDX;
static volatile int* startY = (int*)START_COL_IDX;
static volatile int* endX = (int*)END_ROW_IDX;
static volatile int* endY = (int*)END_COL_IDX;
XAxiDma_Config *myDmaConfig_0;
XAxiDma myDma_0;

void compute(int);
void copyIn();
void copyOut();

void copyInHW();
void copyOutHW();
void constantsUpdate();
void computeHW(int);
void updateDispBuffHW(int);
void updateDispBuff(int);
void modifyPower_sw();
void modifyPower_hw();
u32 checkDmaHalted(u32 baseaddr, u32 offset);

int main()
{
    init_platform();
//    Xil_DCacheDisable();
    counter64_init();

    *com = DONE;

    print("Core 1 Running\n\r");
//    sprintf(buff, "cc = %f, cns = %f, cwe = %f, c_amb = %f, cap_1 = %f\n", cc, cns, cwe, camb, Cap_1);
//    printf("%s", buff);
    int i;
    while(1) {
    	switch(*com) {
    		case DONE:
    			break;
    		case RUN:
    			//do one iteration of simulation
//    			printf("Computing\r\n");
    			counter64_start();
    			for(int j = 0; j <(*num_iter); j++)
    				compute(i++);
    			counter64_stop();
    			*com = DONE;
    			break;
    		case IN:
    			//copy data from recvd buffers to in/out buffers
    			printf("Copying In\n");
    			i = 0;
    			copyIn();
    			counter64_reset();
    			*com = DONE;
    			break;
    		case OUT:
    			//copy data from in/out buffers to out buffers
    			printf("Copying Out\n");
    			copyOut();
    			*com = DONE;
    			break;

    		case RUN_HW:
    			//do one iteration of simulation
//    			printf("Computing\r\n");
    			counter64_start();
    			for(int j = 0; j <(*num_iter); j++)
    				computeHW(i++);
    			counter64_stop();
    			updateDispBuffHW(i);
				*com = DONE;
    			break;

    		case IN_HW:
    			//copy data from recvd buffers to in/out buffers
    			printf("Copying In HW\n");
    			i = 0;
    			copyInHW();
    			constantsUpdate();
    			counter64_reset();
    			*com = DONE;
    			break;

    		case OUT_HW:
    			//copy data from in/out buffers to out buffers
    			printf("Copying Out HW\n");
    			copyOutHW();
    			*com = DONE;
    			break;

    		case COMP_UPDATE_SW:
    			//switch on/off components for SW run
    			printf("Update component power data in SW run\r\n");
    			modifyPower_sw();
    			*com = DONE;
				break;

    		case COMP_UPDATE_HW:
				//switch on/off components for HW run
				printf("Update component power data in HW run\r\n");
				modifyPower_hw();
				*com = DONE;
				break;

    		default:
    			break;
    	}

    }

    cleanup_platform();
    return 0;
}

void compute(int i) {
	float* tempIn;
	float* powerIn;
	float* tempOut;
	powerIn = (float*)POWER_IN;

	if(i%2) {
		tempIn = (float*)TEMP_OUT;
		tempOut = (float*)TEMP_IN;
	}
	else {
		tempIn = (float*)TEMP_IN;
		tempOut = (float*)TEMP_OUT;
	}
	single_iteration(tempOut, tempIn, powerIn);
	Xil_DCacheFlushRange((INTPTR)tempOut,  ARR_SIZE_BYTES);
}

void copyIn(){
	float* src;
	float* dst;
	src = (float*)TEMP_RECD;
	dst = (float*)TEMP_IN;
	memcpy(dst, src, ARR_SIZE_BYTES);
	Xil_DCacheFlushRange((INTPTR)dst,  ARR_SIZE_BYTES);
	src = (float*)POWER_RECD;
	dst = (float*)POWER_IN;
	memcpy(dst, src, ARR_SIZE_BYTES);
	Xil_DCacheFlushRange((INTPTR)dst,  ARR_SIZE_BYTES);
    for(int j = 0; j < COMP_NO; j++)
    	comp_status[j] = 0xFFFFFFFF;
    Xil_DCacheFlushRange((INTPTR)comp_status,  COMP_NO*4);
}

void copyOut() {
	float* src;
	float* dst;
	src = (float*)TEMP_IN;
	dst = (float*)TEMP_SEND;
	memcpy(dst, src, ARR_SIZE_BYTES);
	Xil_DCacheFlushRange((INTPTR)dst,  ARR_SIZE_BYTES);
}

void single_iteration(float *result, float *temp, float *power)
{
    int i, j;

    for(i=0; i<row; i++){
		for(j=0; j<col; j++){
			int c = i*col+j;
			int n = (i==0)?c:c-col;
			int s = (i==row-1)?c:c+col;
			int w = (j==0)?c:c-1;
			int e = (j==col-1)?c:c+1;

			result[c] = temp[c]*cc + cns*(temp[n]+temp[s]) + cwe*(temp[e]+temp[w]) + camb + (Cap_1 * power[c]);

		}
    }
}

//void single_iteration_HW(u32 out, u32 in)
//{
//
//	}
//}

void computeHW(int i) {
	if(i%2 == 0) {
		u32 status;
		status = XAxiDma_SimpleTransfer(&myDma_0, (u32)TEMP_OUT, 2*ARR_SIZE_BYTES, XAXIDMA_DEVICE_TO_DMA);
		status = XAxiDma_SimpleTransfer(&myDma_0, (u32)TEMP_IN, 2*(ARR_SIZE_BYTES + INPUT_SIZE * sizeof(float)), XAXIDMA_DMA_TO_DEVICE);
		status = checkDmaHalted(XPAR_AXI_DMA_0_BASEADDR, 0x4);
		while(status!=1){
		status = checkDmaHalted(XPAR_AXI_DMA_0_BASEADDR, 0x4);
		}
		status = checkDmaHalted(XPAR_AXI_DMA_0_BASEADDR, 0x34);
		while(status!=1){
//		print("wait inside receive while loop\n");
		status = checkDmaHalted(XPAR_AXI_DMA_0_BASEADDR, 0x34);
		}
		XAxiDma_Reset(&myDma_0);
		status = XAxiDma_ResetIsDone(&myDma_0);
		while(status!=1){
		status = XAxiDma_ResetIsDone(&myDma_0);
		}
	}

	else {

		u32 status;
		status = XAxiDma_SimpleTransfer(&myDma_0, (u32)TEMP_IN, 2*ARR_SIZE_BYTES, XAXIDMA_DEVICE_TO_DMA);
		status = XAxiDma_SimpleTransfer(&myDma_0, (u32)TEMP_OUT, 2*(ARR_SIZE_BYTES + INPUT_SIZE * sizeof(float)), XAXIDMA_DMA_TO_DEVICE);
		status = checkDmaHalted(XPAR_AXI_DMA_0_BASEADDR, 0x4);
		while(status!=1){
		status = checkDmaHalted(XPAR_AXI_DMA_0_BASEADDR, 0x4);
		}
		status = checkDmaHalted(XPAR_AXI_DMA_0_BASEADDR, 0x34);
		while(status!=1){
//		print("wait inside receive while loop\n");
		status = checkDmaHalted(XPAR_AXI_DMA_0_BASEADDR, 0x34);
		}
		XAxiDma_Reset(&myDma_0);
		status = XAxiDma_ResetIsDone(&myDma_0);
		while(status!=1){
		status = XAxiDma_ResetIsDone(&myDma_0);
		}
	}

}


void copyInHW(){
	float* src0;
	float* src1;
	float* dst;
	src0 = (float*)TEMP_RECD;
	src1 = (float*)POWER_RECD;
	dst = (float*)TEMP_IN;
	for (int i = 0; i < INPUT_SIZE; i++) {
		for (int j = 0; j < INPUT_SIZE; j++) {
			dst[(i*INPUT_SIZE + j)*2] = src0[i*INPUT_SIZE + j];
			dst[(i*INPUT_SIZE + j)*2 + 1] = src1[i*INPUT_SIZE + j];
		}
	}
	Xil_DCacheFlushRange((INTPTR)dst,  2*ARR_SIZE_BYTES);
	memcpy((void*)TEMP_OUT, (void*)dst, 2*ARR_SIZE_BYTES);
	Xil_DCacheFlushRange((INTPTR)TEMP_OUT,  2*ARR_SIZE_BYTES);
	dst = (float*)POWER_IN;
	memcpy(dst, src1, ARR_SIZE_BYTES);
	Xil_DCacheFlushRange((INTPTR)dst,  ARR_SIZE_BYTES);
	updateDispBuffHW(0);
    for(int j = 0; j < COMP_NO; j++)
    	comp_status[j] = 0xFFFFFFFF;
    Xil_DCacheFlushRange((INTPTR)comp_status,  COMP_NO*4);

}

void updateDispBuff(int i){
	float* src;
	float* dst;
	src = (i%2)?(float*)TEMP_IN:(float*)TEMP_OUT;
	dst = (float*)HW_TEMP_BUFF;
	memcpy(dst, src, ARR_SIZE_BYTES);
	Xil_DCacheFlushRange((INTPTR)dst,  ARR_SIZE_BYTES);
}

void updateDispBuffHW(int i){
	float* src;
	float* dst;
	src = (i%2)?(float*)TEMP_IN:(float*)TEMP_OUT;
	dst = (float*)HW_TEMP_BUFF;
	for (int i = 0; i < INPUT_SIZE*INPUT_SIZE; i++) {
		dst[i] = src[2*i];
	}
	Xil_DCacheFlushRange((INTPTR)dst,  ARR_SIZE_BYTES);
}

void copyOutHW() {
	float* src;
	float* dst;
	src = (float*)TEMP_IN;
	dst = (float*)TEMP_SEND;
	for (int i = 0; i < INPUT_SIZE; i++) {
		for (int j = 0; j < INPUT_SIZE; j++) {
			dst[i*INPUT_SIZE + j] = src[(i*INPUT_SIZE + j)*2];
		}
	}
	Xil_DCacheFlushRange((INTPTR)dst,  ARR_SIZE_BYTES);
}

void constantsUpdate() {
//	const u32 CC = 0x5126E979;
//	const u32 CNS = 0x2BB0D017;
//	const u32 CWE = 0x2BB0D017;
//	const u32 CAMB = 0x06D3A06D;
//	const u32 CAP_1 = 0x2BB0CF88;

	const u64 scalar = ((u64)1<<32);

	u32 CC = (u32)(cc*scalar);
	u32 CNS = (u32)(cns*scalar);
	u32 CWE = (u32)(cwe*scalar);
	u32 CAMB = (u32)(camb*scalar);
	u32 CAP_1 =(u32)(Cap_1*scalar);

	const int CONSTS_BASE = XPAR_CONST_COEFFS_0_S00_AXI_BASEADDR;
	const int CC_OFFSET = 0;
	const int CNS_OFFSET = 4;
	const int CWE_OFFSET = 8;
	const int CAMB_OFFSET = 12;
	const int CAP_OFFSET = 16;

	Xil_Out32((UINTPTR)(CONSTS_BASE + CC_OFFSET), CC);
	Xil_Out32((UINTPTR)(CONSTS_BASE + CNS_OFFSET), CNS);
	Xil_Out32((UINTPTR)(CONSTS_BASE + CWE_OFFSET), CWE);
	Xil_Out32((UINTPTR)(CONSTS_BASE + CAMB_OFFSET), CAMB);
	Xil_Out32((UINTPTR)(CONSTS_BASE + CAP_OFFSET), CAP_1);

//    char buff[100];
//    sprintf(buff, "cc = %ld, cns = %ld, cwe = %ld, c_amb = %ld, cap_1 = %ld\n", CC, CNS, CWE, CAMB, CAP_1);
//    printf("%s", buff);

	u32 status;
	myDmaConfig_0 = XAxiDma_LookupConfigBaseAddr(XPAR_AXI_DMA_0_BASEADDR);
	status = XAxiDma_CfgInitialize(&myDma_0, myDmaConfig_0);
	if(status!=XST_SUCCESS){
	print("DMA_0 config initialization failed\n");
	}
	else{
	print("DMA_0 config initialization success\n");
	}
}

void modifyPower_sw (){
	float* powerIn = (float*)POWER_IN;
	float* powerRef = (float*)POWER_RECD;
    for(int i=0; i<COMP_NO; i++){
        if(comp_status[i]==0){ //comp switched off. replace with 0s
            for(int r=startX[i]; r<=endX[i]; r++){
                for(int c=startY[i]; c<=endY[i]; c++){
                    int idx = r*INPUT_SIZE+c;
                    powerIn[idx] = 0.0;
                }
            }
        }
        else{   //comp is not switched off. replace data from ref
            for(int r=startX[i]; r<=endX[i]; r++){
                for(int c=startY[i]; c<=endY[i]; c++){
                	int idx = r*INPUT_SIZE+c;
                	powerIn[idx] = powerRef[idx];
                }
            }
        }
    }
    Xil_DCacheFlushRange((INTPTR)powerIn,  ARR_SIZE_BYTES);
}

void modifyPower_hw() {
	float* powerIn = (float*)TEMP_IN;
	float* powerIn1 = (float*)TEMP_OUT;
	float* powerIn2 = (float*)POWER_IN;
	float* powerRef = (float*)POWER_RECD;
    for(int i=0; i<COMP_NO; i++){
        if(comp_status[i]==0){ //comp switched off. replace with 0s
            for(int r=startX[i]; r<=endX[i]; r++){
                for(int c=startY[i]; c<=endY[i]; c++){
                    int idx = r*INPUT_SIZE+c;
                    powerIn[2*idx+1] = 0.0;
                    powerIn1[2*idx+1] = 0.0;
                    powerIn2[idx] = 0.0;
                }
            }
        }
        else{   //comp is not switched off. replace data from ref
            for(int r=startX[i]; r<=endX[i]; r++){
                for(int c=startY[i]; c<=endY[i]; c++){
                	int idx = r*INPUT_SIZE+c;
                	powerIn[2*idx+1] = powerRef[idx];
                	powerIn1[2*idx+1] = powerRef[idx];
                	powerIn2[idx] = powerRef[idx];
                }
            }
        }
    }
    Xil_DCacheFlushRange((INTPTR)powerIn,  2*ARR_SIZE_BYTES);
    Xil_DCacheFlushRange((INTPTR)powerIn1,  2*ARR_SIZE_BYTES);
    Xil_DCacheFlushRange((INTPTR)powerIn2,  ARR_SIZE_BYTES);
}

u32 checkDmaHalted(u32 baseaddr, u32 offset){
	u32 status;
	status = (XAxiDma_ReadReg(baseaddr, offset))&(XAXIDMA_HALTED_MASK);
	return status;
}
