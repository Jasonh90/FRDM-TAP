#include "MK64F12.h"
#include "SSD1306.h"
#include "milliseconds.h"

#include <fsl_device_registers.h>
#include "utils.h"


void GPIO_setup(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;        // enable clock to port C
    PORTC->PRC[3] &= ~PORT_PCR_MUX(111);
    PORTC->PCR[3] = PORT_PCR_MUX(001);      // enable PTC12 as GPIO
    //PTC->PDDR = (0u << 3);                   // enable PTC12 as INPUT
    PTC->PDDR &= ~GPIO_PDDR_PDD(0 << 4);

	PORTC->PCR[3] |= PORT_PCR_PE_MASK;
	PORTC->PCR[3] &= ~PORT_PCR_PS_MASK;
}


int main() {
    setup_timer();
    SSD1306_begin();

    delay(1000);
    SSD1306_hello();

    /* Push-button test */
    LED_Initialize();
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


    SSD1306_play();

		SSD1306_done();
    //Scroll_Setup(1, 0, 1, 7);
    return 0;
}
