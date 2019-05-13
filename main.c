#include "MK64F12.h"
#include "SSD1306.h"
#include "milliseconds.h"

int main() {
  setup_timer();
  SSD1306_begin();

  delay(1000);
	//SSD1306_welcome();

  //SSD1306_draw(112, 127, 0, 1, 32, arrow_up); 
	//SSD1306_draw(112, 127, 2, 3, 32, arrow_left); 
  //SSD1306_draw(112, 127, 4, 5, 32, arrow_right); 
	//SSD1306_draw(112, 127, 6, 7, 32, arrow_down); 
	
	SSD1306_play();
	
	//Scroll_Setup(1, 0, 1, 7); 
	return 0;
}
