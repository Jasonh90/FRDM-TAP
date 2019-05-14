#include "MK64F12.h"
#include "SSD1306.h"
#include "milliseconds.h"

#include <fsl_device_registers.h>
#include "utils.h"


void GPIO_setup(void) {
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;        // enable clock to port C
	PORTC->PCR[3] &= ~PORT_PCR_MUX(111);
	PORTC->PCR[3] = PORT_PCR_MUX(001);      	 // enable PTC3 as GPIO
	PTC->PDDR = (0u << 3);                   // enable PTC3 as INPUT
	//PTC->PDDR &= ~GPIO_PDDR_PDD(0 << 4);

	PORTC->PCR[3] |= PORT_PCR_PE_MASK;
	PORTC->PCR[3] &= ~PORT_PCR_PS_MASK;
	PORTC->PCR[3] |= PORT_PCR_IRQC(1001);
	NVIC_EnableIRQ(PORTC_IRQn);
	
}


int main() {
		/* Push-button Setup */
    LED_Initialize();
    GPIO_setup();
	
    setup_timer();
    SSD1306_begin();
		delay(1000);
		
    SSD1306_hello();
		
    while(1);

    SSD1306_play();

		SSD1306_done();
    //Scroll_Setup(1, 0, 1, 7);
    return 0;
}
/*
void PORTC_IRQHandler(){
	PORTC->PCR[3] |= (1 << 24);
	LEDRed_Toggle();	

}
*/