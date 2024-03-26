#include "xil_io.h"
#include "xil_types.h"
#include "xparameters.h"

#define COUNTER_BASE XPAR_COUNTER64_0_S00_AXI_BASEADDR
#define CNTRL_REG_OFFSET 8
#define CNT0_REG_OFFSET  0
#define CNT1_REG_OFFSET  4
#define OVRFL_REG_OFFSET 12

#define RESET0_BIT  0
#define ENABLE0_BIT 1
#define RESET1_BIT  2
#define ENABLE1_BIT  3
#define RESET2_BIT  4
#define ENABLE2_BIT  5
#define CASCADE_BIT  6

#define COUNTER_CLK  125000000

void counter64_init(void);
void counter64_start(void);
void counter64_stop(void);
void counter64_reset(void);
u64 counter64_value(void);
