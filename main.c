#include "MK64F12.h"
#include "SSD1306.h"
#include "milliseconds.h"
#include <fsl_device_registers.h>
#include "utils.h"


void GPIO_setup(PORT_Type* port, GPIO_Type* gpio, int i, int irq, int mask) {
	SIM->SCGC5 |= mask;        // enable clock to port C
	port->PCR[i] &= ~PORT_PCR_MUX(111);
	port->PCR[i] = PORT_PCR_MUX(001);      	 // enable PTC3 as GPIO
	gpio->PDDR = (0u << 3);                   // enable PTC3 as INPUT
	//PTC->PDDR &= ~GPIO_PDDR_PDD(0 << 4);

	PORTC->PCR[i] |= PORT_PCR_PE_MASK;
	PORTC->PCR[i] &= ~PORT_PCR_PS_MASK;	
	PORTC->PCR[i] |= PORT_PCR_IRQC(1001);
	NVIC_EnableIRQ(irq);
}


int main() {
		/* Push-button Setup */
		LED_Initialize();
		SIM->SCGC5 |= (0xF <<  10);
    GPIO_setup(PORTC, PTC, 3, PORTC_IRQn, SIM_SCGC5_PORTC_MASK);
    GPIO_setup(PORTA, PTA, 2, PORTA_IRQn, SIM_SCGC5_PORTA_MASK);
	
    setup_timer();
    SSD1306_begin();
		delay(1000);
		
    SSD1306_hello();
		
    while(1);

    SSD1306_play();

		SSD1306_done();
    return 0;
}

void PORTC_IRQHandler(){
	PORTC->PCR[3] |= (1 << 24);
	LEDRed_Toggle();	

}
void PORTA_IRQHandler(){
	PORTA->PCR[2] |= (1 << 24);
	LEDGreen_Toggle();	

}
