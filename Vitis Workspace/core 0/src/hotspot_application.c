#include "netif/xadapter.h"
#include "mylib/display/screens.h"

#include "mylib/constants.h"
#include "xgpio.h"

int start_application();
void ethernet_init();
void tcp_fasttmr(void);
void tcp_slowtmr(void);

static void my_state_machine();
static void run_ethernet();
static int constrain(int val, int min, int max);

extern volatile int TcpFastTmrFlag;
extern volatile int TcpSlowTmrFlag;
extern struct netif *echo_netif;
extern int ETHERNET_DATA_READY_FLAG;
extern XGpio BTNInst;

extern float global_max;
extern float global_min;

int state;
int prev_state;
int next_state;

volatile int SCREEN_CHANGE_TO;

//simulation screen interrupt variables
volatile int zoom;
volatile int panX;
volatile int panY;

//options screen interrupt variables
volatile int pos;
volatile int sel;
volatile int iter4;
volatile int iter3;
volatile int iter2;
volatile int iter1;
volatile int iter0;
volatile int usehw;
volatile int iterations;

//simulation screen variable
volatile int SIM_DONE;

//point mode variables
volatile int pointX;
volatile int pointY;
volatile int stepXY;

//power mode variable
volatile int selectedBlock;

static int* myScreen = (int*)IMAGE_TEMP_BUFF;
static int limit;
static int curr_i;
static int iter_per_frame;
static int sw_value;
static volatile int* com = (int*)INTER_CPU_FLAG;
static volatile int* num_iter = (int*)NUM_ITER;


void hotspot_application() {
	state = START;
	prev_state = START;
	while(1){
		my_state_machine();
		prev_state = state;
		state = next_state;
	}
}


static void my_state_machine() {
	switch(state) {
	case START:
		show_welcome_screen(myScreen);
		ethernet_init();
		next_state = WAIT;
		break;

	case WAIT:
		if(prev_state!=WAIT){
			ETHERNET_DATA_READY_FLAG = 0;
			show_wait_screen(myScreen);
			start_application();
		}
		run_ethernet();
		if(ETHERNET_DATA_READY_FLAG)
			next_state =  READY;
		break;

	case READY:
		if(prev_state!=READY){
			SCREEN_CHANGE_TO = READY;
		}
		show_data_screen(myScreen);
		next_state = SCREEN_CHANGE_TO;
		break;

	case PREVIEW_TEMP:
		show_temp_preview(myScreen);
		SCREEN_CHANGE_TO = 0;
		while(!SCREEN_CHANGE_TO);
		next_state = SCREEN_CHANGE_TO;
		break;

	case PREVIEW_POWER:
		show_pow_preview(myScreen);
		SCREEN_CHANGE_TO = 0;
		while(!SCREEN_CHANGE_TO);
		next_state = SCREEN_CHANGE_TO;
		break;

	case OPTIONS:
//		printf("prev state = %d, state = %d\n\r", prev_state, state);
		if (prev_state!=OPTIONS) {
			SCREEN_CHANGE_TO = OPTIONS;
			pos = 0;
			sel = 0;
			iter0 = 0;
			iter1 = 0;
			iter2 = 0;
			iter3 = 0;
			iter4 = 0;
			usehw = 0;
		}
		show_iter_screen(myScreen);
		next_state = SCREEN_CHANGE_TO;
		break;

	case SIMULATION:
		if (prev_state!=SIMULATION && prev_state!=PAUSED) {
			(*com) = usehw?IN_HW:IN;
			zoom = 0;
			panX = 0;
			panY = 0;
			iterations = iter0 + iter1*10 + iter2*100 + iter3*1000 + iter4*10000;
			curr_i = 0;
			global_max = 0.0;
			global_min = 1000.0;
			iter_per_frame = usehw?ITER_PER_FRAME_HW:ITER_PER_FRAME;
			SIM_DONE = 0;
			while(*com);
		}
		if(curr_i < iterations) {
			int remaining = iterations - curr_i;
			if (remaining >= iter_per_frame)
				*num_iter = iter_per_frame;
			else
				*num_iter = remaining;
			curr_i += *num_iter;
			*com = usehw?RUN_HW:RUN;
		}
		else
			SIM_DONE = 1;

		limit = ((INPUT_SIZE>>1) - (INPUT_SIZE>>(zoom+1)));
		sw_value = XGpio_DiscreteRead(&BTNInst, 2) & 1;
		panX = constrain(panX, -limit, limit);
		panY = constrain(panY, -limit, limit);
//		printf("%d\n", curr_i);

		show_sim_screen(myScreen, curr_i, sw_value);

		while(*com);
		next_state = SCREEN_CHANGE_TO;
		break;

	case PAUSED:
		if(prev_state!=PAUSED){
			SCREEN_CHANGE_TO = PAUSED;
			if(prev_state==COMP){
				*com = usehw?COMP_UPDATE_HW:COMP_UPDATE_SW;
			}
			while(*com);
		}
		show_pause_screen(myScreen);
		next_state = SCREEN_CHANGE_TO;
		break;

	case POINT:
		if(prev_state!=POINT){
			SCREEN_CHANGE_TO = POINT;
			pointX = 0;
			pointY = 0;
			stepXY = 1;
		}
		sw_value = XGpio_DiscreteRead(&BTNInst, 2) & 1;
		show_point_screen(myScreen, sw_value);
		next_state = SCREEN_CHANGE_TO;
		break;

	case COMP:
		if(prev_state!=COMP){
			SCREEN_CHANGE_TO = COMP;
			selectedBlock = 0;
		}
		show_comp_screen(myScreen);
		next_state = SCREEN_CHANGE_TO;
		break;

	case END:
		if(prev_state!=END){
			SCREEN_CHANGE_TO = END;
			(*com) = usehw?OUT_HW:OUT;
			sw_value = XGpio_DiscreteRead(&BTNInst, 2) & 1;
			show_end_screen(myScreen, sw_value);
			start_application();
			while(*com);
		}
		int sw = XGpio_DiscreteRead(&BTNInst, 2) & 1;
		if (sw!=sw_value) {
			show_end_screen(myScreen, sw);
			sw_value=sw;
		}
		run_ethernet();
		next_state = SCREEN_CHANGE_TO;
		break;

	default:
		//reset
		next_state = START;
		break;
	}

}

static void run_ethernet() {
	if (TcpFastTmrFlag) {
		tcp_fasttmr();
		TcpFastTmrFlag = 0;
	}
	if (TcpSlowTmrFlag) {
		tcp_slowtmr();
		TcpSlowTmrFlag = 0;
	}
	xemacif_input(echo_netif);
}

static int constrain(int val, int min, int max){
	return (val < min) ? min : (val > max) ? max : val;
}
