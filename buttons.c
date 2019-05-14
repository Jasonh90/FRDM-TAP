#include "MK64F12.h"
#include "buttons.h"

// Enable all the interrupts for the buttons
static void interrupts_enable(){
	PORTC->PCR[3] |= PORT_PCR_IRQC(1001);
	NVIC_EnableIRQ(PORTC_IRQn);
}