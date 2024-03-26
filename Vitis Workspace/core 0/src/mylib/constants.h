//DDR address for common spaces
#define VGA_BUFF_ADDR 0x09000000
#define IMAGE_TEMP_BUFF 0x09500000
#define TEMP_RECD 0x09A00000
#define POWER_RECD 0x09E00000
#define TEMP_SEND 0x0A200000
#define TEMP_IN 0x0A600000
#define POWER_IN 0x0AA00000
#define TEMP_OUT 0x0AE00000
#define POWER_OUT 0x0B200000
#define HW_TEMP_BUFF 0x0B600000

#define INTER_CPU_FLAG 0x0BA00000
#define NUM_ITER 0x0BA00004

#define START_ROW_IDX 0x0BA00100
#define START_COL_IDX 0x0BA00200
#define END_ROW_IDX 0x0BA00300
#define END_COL_IDX 0x0BA00400
#define COMP_STATUS 0x0BA00500

#define COMP_NO 8

#define ITER_PER_FRAME 4
#define ITER_PER_FRAME_HW 100

#define INPUT_SIZE 512
//different state for sw statemachine
//define different possible states
#define START 0
#define WAIT 1
#define READY  2
#define PREVIEW_TEMP  3
#define PREVIEW_POWER  4
#define OPTIONS 5
#define SIMULATION  6
#define PAUSED 7
#define POINT 8
#define COMP 9
#define END 10

//different values of INTER_CPU_FLAG indicate different functions for cpu1
#define DONE 0
#define RUN 1
#define IN 2
#define OUT 3
#define RUN_HW 4
#define IN_HW 5
#define OUT_HW 6
#define COMP_UPDATE_SW 7
#define COMP_UPDATE_HW 8


