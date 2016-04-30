/*
 * HX8353E.h
 *
 *  Created on: Apr 10, 2016
 *      Author: Jan & Martin
 *
 *   This is derived from the Rei VILO lcd_screen-library-suite
 *   Strictly reduced to drive a HX8353 screen with Hercules microcontrollers
 *
/// @copyright (c) Rei VILO, 2010-2013
/// @copyrightAll rights reserved
/// @n          http://embeddedcomputing.weebly.com/lcd_screen-library-suite
///
/// @n  Dual license:
/// *   For hobbyists and for personal usage: Attribution-NonCommercial-ShareAlike 3.0 Unported (CC BY-NC-SA 3.0)
/// *   For professionals or organisations or for commercial usage: All rights reserved
///
/// @n  For any enquiry about license, http://embeddedcomputing.weebly.com/contact
 *
 */

#ifndef HX8353E_HX8353E_H_
#define HX8353E_HX8353E_H_

#include <stdint.h>
#include <stdbool.h>


#define blackColour    0b0000000000000000
#define whiteColour    0b1111111111111111
#define redColour      0b1111100000000000
#define greenColour    0b0000011111100000
#define blueColour     0b0000000000011111
#define yellowColour   0b1111111111100000
#define cyanColour     0b0000011111111111
#define orangeColour   0b1111101111100000
#define magentaColour  0b1111100000001111
#define violetColour   0b1111100000011111
#define grayColour     0b0111101111101111
#define darkGrayColour 0b0011100111100111

// ported from LCD_screen_font
#define MAX_FONT_SIZE 2 // the source for font size 3 is not available

void screenInit();
void screenBegin();
void clear(uint16_t colour);
void setBacklight(bool flag);
void setOrientation(uint16_t orientation);
uint16_t screenSizeX();
uint16_t screenSizeY();
void setPenSolid(bool flag);
void rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t colour);
void circle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t colour);
void dLine(uint16_t x0, uint16_t y0, uint16_t dx, uint16_t dy, uint16_t colour);
void line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t colour);
void point(uint16_t x1, uint16_t y1, uint16_t colour);
void triangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t colour);
void invert(bool flag);

// ported from LCD_screen_font
void setFontSize(uint32_t font);
uint32_t fontMax();
uint32_t fontSizeX();
uint32_t fontSizeY();
void setFontSolid(bool flag);

void gText(uint16_t x0, uint16_t y0,
        uint8_t *s, uint32_t length,
        uint16_t textColour, uint16_t backColour,
        uint32_t ix, uint32_t iy);

void drawLogo();

typedef void (*BacklightCallback)(bool function);
void setBacklightCallback(BacklightCallback cb);


#endif /* HX8353E_HX8353E_H_ */
