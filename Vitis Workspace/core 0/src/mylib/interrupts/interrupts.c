#include "interrupts.h"

extern int state;
extern volatile int SCREEN_CHANGE_TO;
extern volatile int SIM_DONE;
extern volatile int zoom;
extern volatile int panX;
extern volatile int panY;

extern volatile int pos;
extern volatile int sel;
extern volatile int iter0, iter1, iter2, iter3, iter4;
extern volatile int usehw;
extern volatile int iterations;


extern volatile int pointX;
extern volatile int pointY;
extern volatile int stepXY;

volatile int comp;
extern volatile int selectedBlock;
 /* The Instance of the Interrupt Controller Driver */
static XGpioPs Gpio; /* The driver instance for GPIO Device. */
XGpio BTNInst;

volatile XTime prev_push0 = 0;
volatile XTime prev_push1 = 0;
volatile XTime prev_pushU = 0;
volatile XTime prev_pushR = 0;
volatile XTime prev_pushC = 0;
volatile XTime prev_pushL = 0;
volatile XTime prev_pushD = 0;
static volatile int limit;
static volatile int step;

void GpioPsIntrHandler(void *CallBackRef, int Bank, u32 Status)
{
	u32 DataRead0, DataRead1;
	XTime tCurr;
	XTime_GetTime(&tCurr);
	DataRead0 = XGpioPs_ReadPin(&Gpio, pb0);
	DataRead1 = XGpioPs_ReadPin(&Gpio, pb1);
	if (DataRead0) {
		if ((tCurr - prev_push0)>=DEBOUNCE_COUNT){
			// code for when  button 8 is pushed
			switch(state){
				case SIMULATION:
					zoom = (zoom==0)?0:zoom-1;
					break;

				case READY:
					SCREEN_CHANGE_TO = WAIT;
					break;

				case OPTIONS:
				case PAUSED:
					SCREEN_CHANGE_TO = READY;
					break;

				case POINT:
					stepXY = (stepXY<=1)?1:(stepXY>>1);
					break;

				default:
					break;
			}
			//
			prev_push0 = tCurr;
		}
	}
	if (DataRead1){
		if ((tCurr - prev_push1)>=DEBOUNCE_COUNT){
			// code for when  button 9 is pushed
			switch(state){
				case SIMULATION:
					zoom = (zoom==3)?3:zoom+1;
					break;

				case OPTIONS:
					SCREEN_CHANGE_TO = SIMULATION;
					break;

				case POINT:
					stepXY = (stepXY>=(INPUT_SIZE/2))?(INPUT_SIZE/2):(stepXY<<1);
					break;

				default:
					break;
			}
			prev_push1 = tCurr;
		}
	}

}

void BTN_Intr_Handler(void *InstancePtr) {
	// Disable GPIO interrupts
	XGpio_InterruptDisable(&BTNInst, BTN_INT);
	// Ignore additional button presses
	int btn_value = XGpio_DiscreteRead(&BTNInst, 1);
	// Increment counter based on button value
	// Reset if centre button pressed
	XTime tCurr;
	XTime_GetTime(&tCurr);

	if (btn_value & (1 << pbC)) {
		if ((tCurr - prev_pushC)>=DEBOUNCE_COUNT){
			// code for when Center button is pushed
			switch(state){
				case READY:
					SCREEN_CHANGE_TO = PREVIEW_POWER;
					break;

				case PREVIEW_TEMP:
				case PREVIEW_POWER:
					SCREEN_CHANGE_TO = READY;
					break;

				case OPTIONS:
					sel = ~sel;
					break;

				case SIMULATION:
					SCREEN_CHANGE_TO = SIM_DONE?END:PAUSED;
					break;

				case END:
					SCREEN_CHANGE_TO = READY;
					break;

				case PAUSED:
					SCREEN_CHANGE_TO = POINT;
					break;

				case COMP:
				case POINT:
					SCREEN_CHANGE_TO = PAUSED;
					break;

				default:
					break;
			}
			//
			prev_pushC = tCurr;
		}
	}
	if (btn_value & (1 << pbD)) {
		if ((tCurr - prev_pushD)>=DEBOUNCE_COUNT){
			// code for when Down button is pushed
			switch(state){
				case READY:
					SCREEN_CHANGE_TO = OPTIONS;
					break;

				case SIMULATION:
					limit = (INPUT_SIZE>>1) - (INPUT_SIZE>>(zoom+1));
					step = INPUT_SIZE>>(zoom+2);
					panY = (panY+step >= limit) ? limit : (panY+step);
					break;

				case OPTIONS:
					if (!sel)
						switch(pos){
						case 0:
							iter0 = iter0 == 0 ? 9 : iter0 - 1;
							break;
						case 1:
							iter1 = iter1 == 0 ? 9 : iter1 - 1;
							break;
						case 2:
							iter2 = iter2 == 0 ? 9 : iter2 - 1;
							break;
						case 3:
							iter3 = iter3 == 0 ? 9 : iter3 - 1;
							break;
						case 4:
							iter4 = iter4 == 0 ? 9 : iter4 - 1;
							break;
						}
					break;

				case PAUSED:
					SCREEN_CHANGE_TO = SIMULATION;
					break;

				case POINT:
					pointY = (pointY+stepXY >= (INPUT_SIZE-1))?(INPUT_SIZE-1):(pointY+stepXY);
					break;

				case COMP:
					selectedBlock = (selectedBlock==(COMP_NO-1))?(COMP_NO-1):(selectedBlock+1);
					break;


				default:
					break;
			}
			//
			prev_pushD = tCurr;
		}
	}
	if (btn_value & (1 << pbL)) {
		if ((tCurr - prev_pushL)>=DEBOUNCE_COUNT){
			// code for when Left button is pushed
			switch(state){
			case SIMULATION:
				limit = (INPUT_SIZE>>1) - (INPUT_SIZE>>(zoom+1));
				step = INPUT_SIZE>>(zoom+2);
				panX = (panX-step <= -limit) ? -limit : (panX-step);
				break;

			case OPTIONS:
				if (!sel)
					pos = (pos+1)%5;
				else
					usehw = ~usehw;
				break;

			case POINT:
				pointX = (pointX-stepXY <= 0)?0:(pointX-stepXY);
				break;

			case COMP:
				comp = COMP_STATUS + (selectedBlock*4);
				(*(int*)comp) = ~(*(int*)comp);
				break;

			default:
				break;
			}
			//
			prev_pushL = tCurr;
		}
	}
	if (btn_value & (1 << pbR)) {
		if ((tCurr - prev_pushR)>=DEBOUNCE_COUNT){
			// code for when Right button is pushed
			switch(state){
			case SIMULATION:
				limit = (INPUT_SIZE>>1) - (INPUT_SIZE>>(zoom+1));
				step = INPUT_SIZE>>(zoom+2);
				panX = (panX+step >= limit) ? limit : (panX+step);
				break;

			case OPTIONS:
				if(!sel)
					pos = (pos==0) ?  4: (pos-1)%5;
				else
					usehw = ~usehw;
				break;

			case POINT:
				pointX = (pointX+stepXY >= (INPUT_SIZE-1))?(INPUT_SIZE-1):(pointX+stepXY);
				break;

			case COMP:
				comp = COMP_STATUS + (selectedBlock*4);
				(*(int*)comp) = ~(*(int*)comp);
				break;

			default:
				break;
			}
			//
			prev_pushR = tCurr;
		}
	}
	if (btn_value & (1 << pbU)) {
		if ((tCurr - prev_pushU)>=DEBOUNCE_COUNT){
			// code for when Up button is pushed
			switch(state){
				case READY:
					SCREEN_CHANGE_TO = PREVIEW_TEMP;
					break;

				case SIMULATION:
					limit = (INPUT_SIZE>>1) - (INPUT_SIZE>>(zoom+1));
					step = INPUT_SIZE>>(zoom+2);
					panY = (panY-step <= -limit) ? -limit : (panY-step);
					break;

				case OPTIONS:
					if (!sel) {
						switch(pos){
						case 0:
							iter0 = (iter0 + 1)%10;
							break;
						case 1:
							iter1 = (iter1 + 1)%10;
							break;
						case 2:
							iter2 = (iter2 + 1)%10;
							break;
						case 3:
							iter3 = (iter3 + 1)%10;
							break;
						case 4:
							iter4 = (iter4 + 1)%10;
							break;
						}
					}
					break;

				case PAUSED:
					SCREEN_CHANGE_TO = COMP;
					break;

				case POINT:
					pointY = (pointY-stepXY <= 0)?0:(pointY-stepXY);
					break;

				case COMP:
					selectedBlock = (selectedBlock==0)?0:(selectedBlock-1);
					break;

				default:
					break;
			}
			//
			prev_pushU = tCurr;
		}
	}

	(void) XGpio_InterruptClear(&BTNInst, BTN_INT);
	// Enable GPIO interrupts
	XGpio_InterruptEnable(&BTNInst, BTN_INT);
}


void IntrInit(XScuGic *GicInstancePtr) {

	int Status;
	XGpioPs_Config *GPIOConfigPtr;
	GPIOConfigPtr = XGpioPs_LookupConfig(XPAR_XGPIOPS_0_DEVICE_ID);
	Status = XGpioPs_CfgInitialize(&Gpio, GPIOConfigPtr,GPIOConfigPtr->BaseAddr);
	if (Status != XST_SUCCESS) {
		print("GPIO INIT FAILED\n\r");
	}
	XGpioPs_SetDirectionPin(&Gpio, pb0, 0x0);
	XGpioPs_SetDirectionPin(&Gpio, pb1, 0x0);


	//
	XGpio_Initialize(&BTNInst, BTNS_DEVICE_ID);

	// Set all buttons direction to inputs
	XGpio_SetDataDirection(&BTNInst, 1, 0xFF);

	// Initialize interrupt controller
	SetupInterruptSystem(GicInstancePtr, &Gpio, GPIO_INTERRUPT_ID);
	IntcInitFunction(GicInstancePtr, &BTNInst);
}

void SetupInterruptSystem(XScuGic *GicInstancePtr, XGpioPs *Gpio, u16 GpioIntrId)
{

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,(Xil_ExceptionHandler)XScuGic_InterruptHandler, GicInstancePtr);

	XScuGic_Connect(GicInstancePtr, GpioIntrId, (Xil_ExceptionHandler)XGpioPs_IntrHandler, (void *)Gpio);

	XGpioPs_SetIntrTypePin(Gpio, pb0, XGPIOPS_IRQ_TYPE_EDGE_RISING);
	XGpioPs_SetIntrTypePin(Gpio, pb1, XGPIOPS_IRQ_TYPE_EDGE_RISING);

	XGpioPs_SetCallbackHandler(Gpio, (void *)Gpio, (XGpioPs_Handler)GpioPsIntrHandler);

	//Enable the GPIO interrupts of Bank 0.
	XGpioPs_IntrEnablePin(Gpio, pb0);
	XGpioPs_IntrEnablePin(Gpio, pb1);

	XScuGic_Enable(GicInstancePtr, GpioIntrId);
	Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);
}


int IntcInitFunction(XScuGic *GicInstancePtr, XGpio *GpioInstancePtr) {
	int status;

	XGpio_InterruptEnable(&BTNInst, BTN_INT);
	XGpio_InterruptGlobalEnable(&BTNInst);

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler) XScuGic_InterruptHandler, GicInstancePtr);
	Xil_ExceptionEnable();

	// Connect GPIO interrupt to handler
	status = XScuGic_Connect(GicInstancePtr, INTC_GPIO_INTERRUPT_ID,
			(Xil_ExceptionHandler) BTN_Intr_Handler, (void *) GpioInstancePtr);
	if (status != XST_SUCCESS)
		return XST_FAILURE;

	// Enable GPIO interrupts interrupt
	XGpio_InterruptEnable(GpioInstancePtr, 1);
	XGpio_InterruptGlobalEnable(GpioInstancePtr);

	// Enable GPIO and timer interrupts in the controller
	XScuGic_Enable(GicInstancePtr, INTC_GPIO_INTERRUPT_ID);

	return XST_SUCCESS;
}
