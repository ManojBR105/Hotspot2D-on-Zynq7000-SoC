#include <stdio.h>
#include "../../platform.h"
#include "xparameters.h"
#include "xgpio.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include "xgpiops.h"
#include "xtime_l.h"
#include "xil_cache.h"

#include "../constants.h"

#define GPIO_INTERRUPT_ID XPS_GPIO_INT_ID

#define pb0 50
#define pb1 51
#define pbC 0
#define pbD 1
#define pbL 2
#define pbR 3
#define pbU 4

#define DEBOUNCE_DELAY 300 //in ms
#define DEBOUNCE_COUNT (DEBOUNCE_DELAY * (XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ / 2000))

#define BTNS_DEVICE_ID		XPAR_AXI_GPIO_0_DEVICE_ID
#define INTC_GPIO_INTERRUPT_ID XPAR_FABRIC_AXI_GPIO_0_IP2INTC_IRPT_INTR

#define BTN_INT 			XGPIO_IR_CH1_MASK

void IntrInit(XScuGic *GicInstancePtr);
void GpioPsIntrHandler(void *CallBackRef, int Bank, u32 Status);
void SetupInterruptSystem(XScuGic *GicInstancePtr, XGpioPs *Gpio, u16 GpioIntrId);
void BTN_Intr_Handler(void *baseaddr_p);
int IntcInitFunction(XScuGic *GicInstancePtr, XGpio *GpioInstancePtr);
