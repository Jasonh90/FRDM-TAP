#include "SSD1306.h"
#include <fsl_device_registers.h>
#include "data.h"
#include "milliseconds.h"
#include <stdlib.h>

/* Global Variables for the Game */

volatile uint64_t written[4];  											// arrows written on screen
volatile uint64_t SCORE; 														// score of player
const int REACTION_TIME = 1000;											// 1 sec
const uint64_t GAME_TIME = 30000;										// overall game time (30 secs)
const uint64_t WINNING_SCORE = GAME_TIME * 1 / 4; 	// You win by reacting 75%

/* Lower Level Function */

// Set pin as output
static inline void set_output(PORT_Type* port, GPIO_Type* gpio, uint8_t pin) {
  // Enable GPIO
  port->PCR[pin] = (1 <<  8);
  // Set as output
  gpio->PDDR |= 1 << pin;
}

// Set output pin to high
static inline void set_high(PORT_Type* port, GPIO_Type* gpio, uint8_t pin) {
  gpio->PSOR |= 1 << pin;
}

// Set output pin to low
static inline void set_low(PORT_Type* port, GPIO_Type* gpio, uint8_t pin) {
  gpio->PCOR |= 1 << pin;
}

/* Operational Functions */

// Start writing or reading to the SSD1306
static void open() {
  set_low(SSD1306_CS);
  set_low(SSD1306_CLK);
  set_low(SSD1306_MOSI);
  set_low(SSD1306_DC);
}

// Stop writing to the SSD1306
static void close() {
  set_high(SSD1306_CS);
}

// Reset the SSD1306
static void reset() {
  set_low(SSD1306_RST);
  delay(1000);
  set_high(SSD1306_RST);
  delay(1000);
}

/* SPI Helper Functions */

// Write a byte of data
static void write(uint8_t value) {
  for (uint8_t bit = 0x80; bit > 0; bit >>= 1) {
    if (value & bit) {
      set_high(SSD1306_MOSI);
    }
    set_low(SSD1306_CLK);		// Force CLK low
    set_high(SSD1306_CLK);	// Force CLK high
		set_low(SSD1306_MOSI); 	// data line low
  }
}


/*
// Write 2 bytes of data
static inline void write16(uint16_t value) {
  write((value >> 8) & 0xFF);
  write((value >> 0) & 0xFF);
}

// Write 4 bytes of data
static inline void write32(uint32_t value) {
  write((value >> 24) & 0xFF);
  write((value >> 16) & 0xFF);
  write((value >> 8) & 0xFF);
  write((value >> 0) & 0xFF);
}
*/
// Write a command
static void write_command(uint8_t cmd) {
  set_low(SSD1306_DC);
  write(cmd);
  set_high(SSD1306_DC);
}

// Send multiple commands
static void send_commands(uint32_t data_len, uint8_t* data) {
  for (uint32_t i = 0; i < data_len; i++) {
    write_command(data[i]);
  }
}

void draw(int size, const uint8_t* figure){
	for(int i = 0; i < size; i++){
		write(figure[i]);
	}
}

void draw_animate(int size, const uint8_t* figure){
	for(int i = 0; i < size; i++){
		write(figure[i]);
		delay(75);
	}
}

/* SSD1306 Protocol Functions */

// Adjust where to write data
static void setAddrWindow(int c1, int c2, int p1, int p2, uint8_t dir) {
	send_commands(8, (uint8_t []) {
		0x20, dir, 			// (dir) Addressing Mode
		0x21, c1, c2,		// Set Column Address
		0x22, p1, p2,		// Set Page Address
	});
}

// Clears the screen
static void displayClear(){
	setAddrWindow(0, 127, 0, 7, 0);
	for(int j = 0; j < 1024; j++){
		write(0x00);
	}
}

// Clears the screen with given columns and pages
static void displayClearCustom(int c1, int c2, int p1, int p2, int n){
	setAddrWindow(c1, c2, p1, p2, 0);
	for(int j = 0; j < n; j++){
		write(0x00);
	}
}

static void displayInverse(){
	write_command(0xA7);
}

static void displayNormal(){
	write_command(0xA6);
}

static void displayFlash(int i){
	displayInverse();
	delay(i);
	displayNormal();
	delay(i);
}

/* End SSD1306 Protocol Function */

/* Button Helper Functions */

// Enable all the interrupts for the buttons
static void interrupts_enable(){
	//PORTA->PCR[2] |= PORT_PCR_IRQC(1001);
	//NVIC_EnableIRQ(PORTA_IRQn);
	
	//PORTB->PCR[3] |= PORT_PCR_IRQC(1001);
	//NVIC_EnableIRQ(PORTB_IRQn);
	

	
	//PORTD->PCR[3] |= PORT_PCR_IRQC(1001);
	//NVIC_EnableIRQ(PORTD_IRQn);
}

/* End Button Helper Functions */

/* Exposed SSD1306 Functions */

// Initializes the OLED
void SSD1306_begin() {
  SIM->SCGC5 |= (0xF <<  10);

  set_output(SSD1306_CLK);
  set_output(SSD1306_MOSI);
  set_output(SSD1306_CS);
  set_output(SSD1306_DC);
  set_output(SSD1306_RST);

  reset();
  open();
	
	/* Software Initialization */
	send_commands(18,(uint8_t []){
		0xA8, 0x3F,  	// Set MUX Ratio
		0xD3, 0x00,  	// Set Display Offset
		0x40,  				// Set Display Start Line
		0xA1,  				// Set Segment re-map (SEG0 starts at 127, so the left)
		0xC8,  				// Set COM Output Scan Direction (0 -> 63)
		0xDA, 0b00010010,  	// Set COM Pins hardware config
		0x81, 0x7F, 	// Set Contrast Control, higher -> brighter
		0xA4,  				// Disable Entire Display On
		0xA6,  				// Set Normal Display
		0xD5, 0x80,  	// Set Osc Frequency
		0x8D, 0x14,  	// Enable charge pump regulator
		0xAF, 				// Display on 
	});
  displayClear();
	written[0] = 0;
	written[1] = 0;
	written[2] = 0;
	written[3] = 0;
	SCORE = 0;
	interrupts_enable();

  delay(120);

  close();
}

void Scroll_Setup(int is_right, uint8_t page_Start, uint8_t page_End, uint8_t frequency){
	uint8_t scroll;
	if(is_right)
		scroll = 0x26; // right
	else 
		scroll = 0x27; // left
	
	send_commands(9, (uint8_t []) {
		0x2E,						// Deactivate scroll to avoid corrupting RAM
		scroll, 				// Direction 
		0x00, 					// Dummy
		page_Start, 		// Page Start
		frequency, 			// Time interval (frequency)
		page_End, 			// Page End
		0x00, 					// Dummy
		0xFF,						// Dummy
		0x2F,						// Activate scroll
	});
}

void Scroll_Stop(void){
	write_command(0x2E);
}

void SSD1306_draw(int c1, int c2, int p1, int p2, int size, const uint8_t* figure) {
  open();
	
	Scroll_Stop();
  setAddrWindow(c1, c2, p1, p2, 0); // Horizontal Addressing Mode
	draw(size, figure);
	Scroll_Setup(1, 7, 8, 0); // Left scroll
	
  close();
}

void SSD1306_play(){
	open();
	int r = rand() % 4;
	uint8_t c1, c2, p;
	uint64_t timer = get_milliseconds();
	while(get_milliseconds() - timer < GAME_TIME){ // one minute
		// if one of the positions are written, 
		// then keep generating random int
		while(written[r]){ 
			r = rand() % 4;
		}
		if(r == UP){
			SSD1306_draw(UP_COORDS, 32, arrow_up); 
		} else if(r == LEFT){
			SSD1306_draw(LEFT_COORDS, 32, arrow_left); 
		} else if(r == RIGHT){
			SSD1306_draw(RIGHT_COORDS, 32, arrow_right); 
		} else if(r == DOWN){
			SSD1306_draw(DOWN_COORDS, 32, arrow_down); 
		}
		written[r] = get_milliseconds();
		delay(rand() % 3000);
	}
	close();
}

void SSD1306_hello(){
	open();
	
  //setAddrWindow(44, 84, 3, 3, 0);
	//draw_animate(40, hello);
	//delay(1000);
	//displayClear();
	//delay(1000);
	SSD1306_draw(0, 127, 0, 0, 128, upper_dots);
	SSD1306_draw(0, 127, 7, 7, 128, lower_dots);
	
  close();
}


void SSD1306_done(){
	// print out the end display. 
	// Flash the screen a bit
	for(int i = 1; i > 5 ; i++){
		displayFlash(1000/i);
	}
	displayClear(); // Clear Screen
	setAddrWindow(0, 127, 0, 7, 0); // Full Screen
	if(SCORE >= WINNING_SCORE){
		draw(1024, WINNER);
	} else {
		draw(1024, LOSER);
	}
	while(1){ // Flash indefinitely
		displayFlash(1000);
	}
}

uint64_t SSD1306_get_Score(){
	return SCORE;
}

uint64_t SSD1306_get_Reaction_Time(){
	return REACTION_TIME;
}

void SSD1306_update_Score(uint64_t i){
	SCORE += i;
}

uint64_t SSD1306_get_Arrow_Start_Time(int i){
	return written[i];
}

void SSD1306_clear_arrow(int i){
	written[i] = 0; // reset_Arrow_Start_Time
	
	if(i == UP){
		SSD1306_draw(UP_COORDS, 32, arrow_erase); 
	} else if(i == LEFT){
		SSD1306_draw(LEFT_COORDS, 32, arrow_erase); 
	} else if(i == RIGHT){
		SSD1306_draw(RIGHT_COORDS, 32, arrow_erase); 
	} else if(i == DOWN){
		SSD1306_draw(DOWN_COORDS, 32, arrow_erase); 
	}
}


/* End Exposed SSD1306 Functions */