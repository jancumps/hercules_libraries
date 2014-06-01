/*
 * 44780.c
 *
 *  Created on: Mar 23, 2014
 *      Author: jan
 *      100% intellectual property the Arduino LiquidCrystal library
 */

#include "44780.h"

#include <stdio.h>

/*
 * the signal assignments
 */
uint32 _lcd_bitmode = 0;
const gioPin_t *_rs = NULL;
const gioPin_t *_enable = NULL;
/*const gioPin_t *_d0 = NULL;
const gioPin_t *_d1 = NULL;
const gioPin_t *_d2 = NULL;
const gioPin_t *_d3 = NULL;*/

uint32 _numlines=0;
uint32 _currline=0;
uint32 _displayfunction = 0;
uint32 _displaycontrol = 0;
uint32 _displaymode = 0;
const gioPin_t * _data_pins[8];



/**
 * implementation specific
 *
 */
void clear();
void display();
void lcdDelayMicroSeconds( uint32 ms);
void command(uint32 value);
void send(uint32 value, uint32 mode);
uint32 lcdWrite(uint32 value);
void write4bits(uint32 value);
void write8bits(uint32 value);
void lcdPrintSignalLine(const char *name, const gioPin_t *p);

/**
 * API:
 */
void lcdInit(const gioPin_t *rs, const gioPin_t *enable,
		const gioPin_t *d0, const gioPin_t *d1,
		const gioPin_t *d2, const gioPin_t *d3) {

	// if we call this version of lcdInit, we're in 4 bit mode
	_lcd_bitmode = LCD_4BITMODE;

	// assign the signals to a library variable
	// in a miulti-display library we should create an array of signals for each display
	// and work with handles
	_rs = rs;
	// _rw not implemented, add later if needed
	_enable = enable;

	/*_d0 = d0;
	_d1 = d1;
	_d2 = d2;
	_d3 = d3;*/
	_data_pins[0] = d0;
	_data_pins[1] = d1;
	_data_pins[2] = d2;
	_data_pins[3] = d3;

	gioutilsSetPinDirection(_rs, pinWrite);
//	gioutilsSetPinDirection(_rw, pinWrite); // optional, will only enable if needed
	gioutilsSetPinDirection(_enable, pinWrite);
	// in LiquidCristal,
	// the mode is set to fixed character matrix, rows and bitmode(4/8)
    _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;

	// the LCD display is initialised to 16/1 in case begin isn't called
	// not happening here until needed

/*
	// TODO implement lcdInit()
	printf("\n*** start lcdInit()");
	printf("\nThe following data lines are assigned:");
	printf("\n");
	printSignalLine("rs", rs);
	printf("\n");
	printSignalLine("enable", enable);
	printf("\n");
	printSignalLine("d0", d0);
	printf("\n");
	printSignalLine("d1", d1);
	printf("\n");
	printSignalLine("d2", d2);
	printf("\n");
	printSignalLine("d3", d3);
	printf("\n*** stop lcdInit()");
*/

}

void lcdBegin(uint32 cols, uint32 lines, uint32 dotsize) {
	if (lines > 1) {
		_displayfunction |= LCD_2LINE;
	}
	_numlines = lines;
	_currline = 0;

	// for some 1 line displays you can select a 10 pixel high font
	if ((dotsize != 0) && (lines == 1)) {
		_displayfunction |= LCD_5x10DOTS;
	}

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	lcdDelayMicroSeconds(50000);




	/**
	 * test code remove
	int trig = 0;

	while(1) {
		gioutilsSetPin(_rs, trig);
		gioutilsSetPin(_enable, trig);
		gioutilsSetPin(_data_pins[0], trig);
		gioutilsSetPin(_data_pins[1], trig);
		gioutilsSetPin(_data_pins[2], trig);
		gioutilsSetPin(_data_pins[3], trig);
//		lcdDelayMicroSeconds(50000);
		trig = trig ? 0 : 1;

	}
	 */

	// Now we pull both RS and R/W low to begin commands
	gioutilsSetPin(_rs, 0);
	gioutilsSetPin(_enable, 0);
	// no rw pin unless needed

	//put the LCD into 4 bit or 8 bit mode
	if (! (_displayfunction & LCD_8BITMODE)) {
		// this is according to the hitachi HD44780 datasheet
		// figure 24, pg 46

		// we start in 8bit mode, try to set 4 bit mode
		write4bits(0x03);
		lcdDelayMicroSeconds(4500); // wait min 4.1ms

		// second try
		write4bits(0x03);
		lcdDelayMicroSeconds(4500); // wait min 4.1ms

		// third go!
		write4bits(0x03);
		lcdDelayMicroSeconds(150);

		// finally, set to 4-bit interface
		write4bits(0x02);
	} else {
		// this is according to the hitachi HD44780 datasheet
		// page 45 figure 23

		// Send function set command sequence
		command(LCD_FUNCTIONSET | _displayfunction);
		lcdDelayMicroSeconds(4500);  // wait more than 4.1ms

		// second try
		command(LCD_FUNCTIONSET | _displayfunction);
		lcdDelayMicroSeconds(150);

		// third go
		command(LCD_FUNCTIONSET | _displayfunction);
	}

	// finally, set # lines, font size, etc.
	command(LCD_FUNCTIONSET | _displayfunction);

	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	display();

	// clear it off
	clear();

	// Initialize to default text direction (for romance languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	// set the entry mode
	command(LCD_ENTRYMODESET | _displaymode);

	/*
	// TODO implement lcdBegin()
	printf("\n*** start lcdBegin()");
	printf("\nThe display has %u rows and %u columns", rows, cols);
	printf("\n*** stop lcdBegin()");
	*/
}

void clear() {
	command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
	lcdDelayMicroSeconds(2000);  // this command takes a long time!
}

void display() {
	_displaycontrol |= LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void lcdSetCursor(uint32 col, uint32 row) {
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if ( row >= _numlines ) {
		row = _numlines-1;    // we count rows starting w/0
	}

	command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
/*	// TODO implement lcdSetCursor()
	printf("\n*** start lcdSetCursor()");
	printf("\nThe cursor moves to row %u and column %u", row, col);
	printf("\n*** stop lcdSetCursor()");*/
}

void lcdPrint(const char *str) {
	  while (*str)
		  lcdWrite(*str++);
/*	// TODO implement lcdPrint()
	printf("\n*** start lcdPrint()");
	printf("\nlcd print: %s", str);
	printf("\n*** stop lcdPrint()");*/
}



/**
 * end API, begin implementation
 */

/**
 * delay for the given number of microseconds, for 80MHz processor
 */
void lcdDelayMicroSeconds(uint32 ms) {
	uint32 i = 0;
	for( i=0; i<ms*5; i++) {
		asm("	nop");
	}
}

/**
 * set the direction of a given pin without altering other pins
 * on the same port
 *
 */
void command(uint32 value) {
	send(value, 0);
}

void send(uint32 value, uint32 mode) {
  gioutilsSetPin(_rs, mode);

  // rw pin removed, add later if needed

  if (_displayfunction & LCD_8BITMODE) {
    write8bits(value);
  } else {
    write4bits(value>>4);
    write4bits(value);
  }
}

uint32 lcdWrite(uint32 value) {
	send(value, 1);
	return 1; // assume sucess
}

void pulseEnable(void) {
	gioutilsSetPin(_enable, 0);
	lcdDelayMicroSeconds(1);
	gioutilsSetPin(_enable, 1);
	lcdDelayMicroSeconds(1);    // enable pulse must be >450ns
	gioutilsSetPin(_enable, 0);
	lcdDelayMicroSeconds(100);   // commands need > 37us to settle
}

void write4bits(uint32 value) {
	int i;
	for (i = 0; i < 4; i++) {
		gioutilsSetPinDirection(_data_pins[i],pinWrite);
		gioutilsSetPin(_data_pins[i], (value >> i) & 0x01);
	}

	pulseEnable();
}

void write8bits(uint32 value) {
	int i;
	for (i = 0; i < 8; i++) {
		gioutilsSetPinDirection(_data_pins[i],pinWrite);
		gioutilsSetPin(_data_pins[i], (value >> i) & 0x01);
	}

	pulseEnable();
}


/*
 * THIS IS A TEMPORARY DEBUG FUNCTION TO OUTPUT THE PIN ASSIGNMENTS
 */
void lcdPrintSignalLine(const char *name, const gioPin_t *p) {
	printf("signal: %s, port %x, bit %u", name, p->port, p->bit);

}

void lcdTestBed() {
    gioPin_t test1 = {gioPORTA,0};
	gioPin_t test2 = {gioPORTA,2};
    gioutilsSetPinDirection(&test1, pinWrite);
    gioutilsSetPinDirection(&test2, pinWrite);
    while(1) {
    	gioToggleBit(gioPORTA, 0);
    	gioToggleBit(gioPORTA, 2);
    	lcdDelayMicroSeconds(25000);
    }
}

