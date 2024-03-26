#include "counter.h"

void counter64_init() {
	int cntrl_reg = 0;
	cntrl_reg = (1 << CASCADE_BIT);
	Xil_Out32((UINTPTR)COUNTER_BASE + CNTRL_REG_OFFSET, cntrl_reg);
	counter64_reset();
}
void counter64_start() {
	u32 cntrl_reg = Xil_In32((UINTPTR)COUNTER_BASE + CNTRL_REG_OFFSET);
    cntrl_reg &= ~(1 << RESET2_BIT);
    cntrl_reg |= (1 << ENABLE2_BIT);
    Xil_Out32((UINTPTR)COUNTER_BASE + CNTRL_REG_OFFSET, cntrl_reg);
    Xil_Out32((UINTPTR)COUNTER_BASE + CNTRL_REG_OFFSET, cntrl_reg);
}
void counter64_stop() {
	u32 cntrl_reg = Xil_In32((UINTPTR)COUNTER_BASE + CNTRL_REG_OFFSET);
    cntrl_reg &= ~(1 << ENABLE2_BIT);
    Xil_Out32((UINTPTR)COUNTER_BASE + CNTRL_REG_OFFSET, cntrl_reg);
}
void counter64_reset() {
	u32 cntrl_reg = Xil_In32((UINTPTR)COUNTER_BASE + CNTRL_REG_OFFSET);
    cntrl_reg |= (1 << RESET2_BIT);
    Xil_Out32((UINTPTR)COUNTER_BASE + CNTRL_REG_OFFSET, cntrl_reg);
}

u64 counter64_value() {
    u32 count0 = Xil_In32((UINTPTR)COUNTER_BASE + CNT0_REG_OFFSET);
    u32 count1 = Xil_In32((UINTPTR)COUNTER_BASE + CNT1_REG_OFFSET);
    u64 count = ((u64)count1 << 32) | count0;
    return count;
}
