#include "display.h"
#include "MK64F12.h"
#include "SSD1306.h"

volatile uint64_t display_counter;

// Setup display timer
void display_setup() {
  // Initialize PIT timer
  SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
  PIT->MCR = 0x0;

  // If the timer is already enabled, turn it off first
  if (PIT->CHANNEL[1].TCTRL == 0x3) {
    PIT->CHANNEL[1].TCTRL = 0x1;
  }

  // 20970 cycles per millisecond
  PIT->CHANNEL[1].LDVAL = 20970;
  PIT->CHANNEL[1].TCTRL = 0x3;
	
	/* enable PIT1 Interrupts */
	NVIC_EnableIRQ(PIT0_IRQn); 
	
}

void PIT1_IRQHandler(void) {
  // Clear
  PIT->CHANNEL[1].TFLG |= 1;
  // Disable
  PIT->CHANNEL[1].TCTRL &= ~3;
  
  display_counter++;
	
	// Enable
  PIT->CHANNEL[1].TCTRL |= 3;
}
