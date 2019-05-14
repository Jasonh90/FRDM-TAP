#include "MK64F12.h"
#include "SSD1306.h"
#include "milliseconds.h"


void GPIO_setup(void) {
    SIM->SCGC5 = SIM_SCGC5_PORTC_MASK;        // enable clock to port C
    PORTC->PCR[3] = PORT_PCR_MUX(001);      // enable PTC12 as GPIO
    PTC->PDDR = (0u << 3);                   // enable PTC12 as INPUT
		PORTC->PCR[3] |= PORT_PCR_PE_MASK;
		PORTC->PCR[3] &= ~PORT_PCR_PS_MASK;
}


/* From lab 2 */

void led_setup(void){
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;           //enable the clock to port B
    PORTB->PCR[22] = PORT_PCR_MUX(001);          // set up PTB22 as GPIO
    PTB->PDDR |= (1 << 22);                     //enable PTB22 as an output
}

void LEDRed_Toggle (void) {
    PTB->PTOR = 1 << 22;        /* Red LED Toggle */
}

void LEDRed_On (void) {
    // Save and disable interrupts (for atomic LED change)
    uint32_t m;
    m = __get_PRIMASK();
    __disable_irq();
    
    PTB->PCOR   = 1 << 22;   /* Red LED On*/
    //PTB->PSOR   = 1 << 21;   /* Blue LED Off*/
    //PTE->PSOR   = 1 << 26;   /* Green LED Off*/
    
    // Restore interrupts
    __set_PRIMASK(m);
}

void LED_Off (void) {
    // Save and disable interrupts (for atomic LED change)
    uint32_t m;
    m = __get_PRIMASK();
    __disable_irq();
    
    //PTB->PSOR   = 1 << 22;   /* Green LED Off*/
    PTB->PSOR   = 1 << 21;   /* Red LED Off*/
    //PTE->PSOR   = 1 << 26;   /* Blue LED Off*/
    
    // Restore interrupts
    __set_PRIMASK(m);
}

int main() {
    setup_timer();
    SSD1306_begin();
    
    delay(1000);
    SSD1306_hello();
	
    /* Push-button test */	/*
    led_setup();
    GPIO_setup();
    //LEDRed_On();
    
    while(1) {
        if (PTC->PDIR == (0u << 3)) {            // Switch pressed
            LEDRed_On();
        }
        else if (PTC->PDIR == (1u << 3)) {        // Swtich not pressed
            LED_Off();
        }
    }
    */
    SSD1306_play();
    
		SSD1306_done();
    //Scroll_Setup(1, 0, 1, 7);
    return 0;
}


