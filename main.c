#include "MK64F12.h"
#include "SSD1306.h"
#include "milliseconds.h"
#include <fsl_device_registers.h>
#include "utils.h"

void GPIO_setup(PORT_Type* port, GPIO_Type* gpio, int i) {
	port->PCR[i] &= ~PORT_PCR_MUX(111);
	port->PCR[i] = PORT_PCR_MUX(001);      	// enable as GPIO
	gpio->PDDR = (0u << 3);                 // enable as INPUT
	port->PCR[i] |= PORT_PCR_IRQC(1001);		// activate IRQ as rising edge detection
}

int main() {
	/* Push-button & LED Setup */
	LED_Initialize();
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK; // Enable port C
	NVIC_EnableIRQ(PORTC_IRQn);					// Enable port C IRQHandler
	NVIC_SetPriority(PORTC_IRQn, 0);		// Set port C IRQ top priority
	
	// Set up each button
	GPIO_setup(BUTTON_UP);
	GPIO_setup(BUTTON_LEFT);
	GPIO_setup(BUTTON_RIGHT);
	GPIO_setup(BUTTON_DOWN);
	
	setup_timer();
	SSD1306_begin();

	delay(1000);
	
	SSD1306_intro();
	
	// Play the game
	SSD1306_play();
	
	NVIC_DisableIRQ(PORTC_IRQn);	// Disable buttons

	SSD1306_done(); 
	return 0;
}

/* Interrupt Handlers for Push Buttons */

void __interrupt_helper(int i){
	uint64_t start_time = SSD1306_get_Arrow_Start_Time(i);
	if(start_time){
		uint64_t react_time_observed = get_milliseconds() - start_time;
		uint64_t react_time_actual = SSD1306_get_Reaction_Time();
		if(react_time_observed < react_time_actual){
			SSD1306_update_Score(react_time_actual - react_time_observed);
		}
		SSD1306_clear_arrow(i, react_time_observed);
	}
}

void PORTC_IRQHandler(){
	if(PORTC->ISFR & (1 << 2)){ // UP 
		PORTC->PCR[2] |= (1 << 24); // Clear Flag
		LEDRed_Toggle();
		// Check if the first thing in written has something
		// If it is non-zero, then the arrow has been popped up.
		__interrupt_helper(UP);
		
	} else if(PORTC->ISFR & (1 << 3)){ // LEFT
		PORTC->PCR[3] |= (1 << 24); // Clear Flag
		LEDGreen_Toggle();
		__interrupt_helper(LEFT);
		
	} else if(PORTC->ISFR & (1 << 4)){ // RIGHT
		PORTC->PCR[4] |= (1 << 24); // Clear Flag
		LEDBlue_Toggle();
		__interrupt_helper(RIGHT);
		
	} else if(PORTC->ISFR & (1 << 12)){ // DOWN
		PORTC->PCR[12] |= (1 << 24); // Clear Flag
		LEDRed_Toggle();
		LEDBlue_Toggle();
		__interrupt_helper(DOWN);
	}
}

