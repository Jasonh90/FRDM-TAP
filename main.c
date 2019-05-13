#include "MK64F12.h"
#include "milliseconds.h"
#include "SSD1306.h"

int main() {
  setup_timer();
  SSD1306_begin();

  delay(1000);

  SSD1306_fill_screen(0, 127, 0, 7); // full
	
	return 0;
}
