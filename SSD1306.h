#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>

#ifndef SSD1306_CLK
#	define SSD1306_CLK PORTD, PTD, 1
#endif
#ifndef SSD1306_MOSI
#	define SSD1306_MOSI PORTD, PTD, 2
#endif
#ifndef SSD1306_CS
#	define SSD1306_CS PORTB, PTB, 19
#endif
#ifndef SSD1306_DC
#	define SSD1306_DC PORTB, PTB, 18
#endif
#ifndef SSD1306_RST
#	define SSD1306_RST PORTB, PTB, 23
#endif

void SSD1306_begin(void);
uint8_t SSD1306_read_command(uint8_t command);
void SSD1306_fill_screen(int c1, int c2, int p1, int p2);

#endif