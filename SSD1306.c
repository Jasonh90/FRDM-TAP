#include "SSD1306.h"
#include <fsl_device_registers.h>
#include "data.h"
#include "milliseconds.h"

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

/* End Lower Level Functions */


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

/* End Operational Functions */


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

/* original
static void write(uint8_t value) {
  for (uint8_t bit = 0x80; bit > 0; bit >>= 1) {
    if ((value & bit) == bit) {
      set_high(SSD1306_MOSI);
    }
    else {
      set_low(SSD1306_MOSI);
    }
    set_low(SSD1306_CLK);
    set_high(SSD1306_CLK);
  }
}

// mine
static void write(uint8_t value) {
  for (uint8_t bit = 0x80; bit > 0; bit >>= 1) {
    if (value & bit) {
      set_high(SSD1306_MOSI);
    }
    set_low(SSD1306_CLK);
    set_high(SSD1306_CLK);
		set_low(SSD1306_MOSI);
  }
}
*/


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

/* End SPI Helper Functions */

/* SSD1306 Protocol Functions */

static void setAddrWindow(int c1, int c2, int p1, int p2, uint8_t dir) {
	send_commands(8, (uint8_t []) {
		0x20, dir, 			// (dir) Addressing Mode
		0x21, c1, c2,		// Set Column Address
		0x22, p1, p2,		// Set Page Address
	});
}

static void clear_screen(){
	setAddrWindow(0, 127, 0, 7, 0);
	for(int j = 0; j < 1024; j++){
		write(0x00);
	}
}

/* End SSD1306 Protocol Function */


/* Exposed SSD1306 Functions */

void SSD1306_begin() {
  // Enable ports B D
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
  clear_screen();
	
  delay(120);

  close();
}

// NOTE: WHEN THINGS SHOW UP TOP LEFT CORNER AS ORIGIN, 
// THE THINGS ARE UPSIDE DOWN. SO, 0b00100000 is really 0b00000100.

void draw(int size, const uint8_t* figure){
	for(int i = 0; i < size; i++){
		write(figure[i]);
	}
}

void draw_animate(int size, const uint8_t* figure){
	for(int i = 0; i < size; i++){
		write(figure[i]);
		delay(100);
	}
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

void SSD1306_draw(int c1, int c2, int p1, int p2, int size, const uint8_t* figure) {
  open();
	
  setAddrWindow(c1, c2, p1, p2, 0); // Horizontal Addressing Mode
	
	draw(size, figure);
	//Scroll_Setup(1,0,1,7);
  close();
}

void SSD1306_play(){
	open();
	setAddrWindow(0, 15, 0, 7, 0);
	draw(32, arrow_up);
	draw(32, arrow_left);
	draw(32, arrow_right);
	draw(32, arrow_down);
	Scroll_Setup(1,0,7,0);
	close();
}

void SSD1306_welcome(){
	open();
	
  setAddrWindow(44, 84, 3, 3, 0); // Horizontal Addressing Mode
	
	draw_animate(40, welcome);
	delay(1000);
	clear_screen();
	delay(1000);
	
  close();
}

void Scroll_Stop(void){
	write_command(0x2E);
}

/* End Exposed SSD1306 Functions */
