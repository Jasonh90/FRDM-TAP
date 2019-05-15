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
#	define SSD1306_RST PORTC, PTC, 16
#endif 
#ifndef BUTTON_UP
#	define BUTTON_UP PORTC, PTC, 2
#endif
#ifndef BUTTON_LEFT
#	define BUTTON_LEFT PORTC, PTC, 3
#endif
#ifndef BUTTON_RIGHT
#	define BUTTON_RIGHT PORTC, PTC, 4
#endif
#ifndef BUTTON_DOWN
#	define BUTTON_DOWN PORTC, PTC, 12
#endif 
void SSD1306_begin(void);
uint8_t SSD1306_read_command(uint8_t command);
void SSD1306_draw(int c1, int c2, int p1, int p2, int size, const uint8_t* figure);
void SSD1306_play(void);
void SSD1306_hello(void);
void SSD1306_done(void);

#endif
