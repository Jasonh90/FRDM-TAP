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
# define UP 0
# define UP_COORDS 56, 72, 1, 2
#endif
#ifndef BUTTON_LEFT
#	define BUTTON_LEFT PORTC, PTC, 3
# define LEFT 1
# define LEFT_COORDS 24, 40, 3, 4
#endif
#ifndef BUTTON_RIGHT
#	define BUTTON_RIGHT PORTC, PTC, 4
# define RIGHT 2
# define RIGHT_COORDS 88, 104, 3, 4
#endif
#ifndef BUTTON_DOWN
#	define BUTTON_DOWN PORTC, PTC, 12
# define DOWN 3
# define DOWN_COORDS 56, 72, 5, 6
#endif 

void SSD1306_begin(void);
void SSD1306_draw(int c1, int c2, int p1, int p2, int size, const uint8_t* figure);
void SSD1306_play(void);
void SSD1306_intro(void);
void SSD1306_done(void);
uint64_t SSD1306_get_Score(void);
void SSD1306_update_Score(uint64_t i);
uint64_t SSD1306_get_Reaction_Time(void);
uint64_t SSD1306_get_Arrow_Start_Time(int i);
void SSD1306_clear_arrow(int i, uint64_t time);

#endif
