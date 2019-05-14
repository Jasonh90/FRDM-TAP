#include "MK64F12.h"
#include "SSD1306.h"
#include "milliseconds.h"

int main() {
  setup_timer();
  SSD1306_begin();

  delay(1000);
	SSD1306_hello();

	
	
	SSD1306_play();
	
	//Scroll_Setup(1, 0, 1, 7); 
	return 0;
}
