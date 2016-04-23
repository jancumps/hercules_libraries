/*
 * HX8353E.c
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

#include "HX8353E.h"
#include "gio.h"
#include "mibspi.h"
#include "etpwm.h"
#include "HX8353E_font_terminal.h"

// ported from Screen_HX8353E

#define HX8353E_WIDTH  128
#define HX8353E_HEIGHT 128
#define HX8353E_MADCTL_MY  0x80
#define HX8353E_MADCTL_MX  0x40
#define HX8353E_MADCTL_MV  0x20
#define HX8353E_MADCTL_ML  0x10
#define HX8353E_MADCTL_RGB 0x08
#define HX8353E_MADCTL_MH  0x04
#define HX8353E_NOP     0x00
#define HX8353E_SWRESET 0x01
#define HX8353E_RDDID   0x04
#define HX8353E_RDDST   0x09
#define HX8353E_SLPIN   0x10
#define HX8353E_SLPOUT  0x11
#define HX8353E_PTLON   0x12
#define HX8353E_NORON   0x13
#define HX8353E_INVOFF  0x20
#define HX8353E_INVON   0x21
#define HX8353E_GAMSET  0x26
#define HX8353E_DISPOFF 0x28
#define HX8353E_DISPON  0x29
#define HX8353E_CASET   0x2A
#define HX8353E_RASET   0x2B
#define HX8353E_RAMWR   0x2C
#define HX8353E_RGBSET  0x2d
#define HX8353E_RAMRD   0x2E
#define HX8353E_PTLAR   0x30
#define HX8353E_MADCTL  0x36
#define HX8353E_COLMOD  0x3A
#define HX8353E_SETPWCTR 0xB1
#define HX8353E_SETDISPL 0xB2
#define HX8353E_FRMCTR3  0xB3
#define HX8353E_SETCYC   0xB4
#define HX8353E_SETBGP   0xb5
#define HX8353E_SETVCOM  0xB6
#define HX8353E_SETSTBA  0xC0
#define HX8353E_SETID    0xC3
#define HX8353E_GETHID   0xd0
#define HX8353E_SETGAMMA 0xE0


// ported from LCD_screen
uint32_t      _fontX, _fontY, _fontSize;
uint32_t      _orientation;
bool         _penSolid, _fontSolid, _flagRead, _flagStorage;
uint32_t     _screenWidth, _screenHeigth;


// ported from Screen_HX8353E
gioPORT_t *_portReset; // the port the signal line is on
uint32_t _pinReset;      // the pin the signal line is on
gioPORT_t *_portDataCommand; // the port the signal line is on
uint32_t _pinDataCommand;      // the pin the signal line is on


void delay(uint32_t ms); // not making any fans here, am I?

// ported from Screen_HX8353E
void _writeRegister(uint16_t command8, uint16_t data8);
void _writeCommand(uint16_t command8);
void _writeData(uint16_t data8);
void _writeData16(uint16_t data16);
void _writeData88(uint16_t dataHigh16, uint16_t dataLow16);
void _writeData8888(uint16_t dataHigh8, uint16_t dataLow8, uint16_t data8_3, uint16_t data8_4);
void _fastFill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t colour);
void _setPoint(uint16_t x1, uint16_t y1, uint16_t colour);
void _setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void _swapU(uint16_t *a, uint16_t *b);
void _swapI(int16_t *a, int16_t *b);

// ported from LCS_screen_font
uint32_t _getCharacter(uint32_t c, uint32_t i);

// ported from LCD_screen
void _triangleArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t colour);


/**
 * this is the functionality of the energia lib constructors
 */
void screenInit() {
    // LCD_screen::LCD_screen()
    _fontSize       = 0;
    _fontSolid      = true;
    _penSolid       = false;
    _flagRead       = false;
    _flagStorage    = false;

    // LCD_screen_font::LCD_screen_font()
    // empty

    // Screen_HX8353E::Screen_HX8353E()
    _portReset = gioPORTB;
    _pinReset = 3;
    _portDataCommand = gioPORTA;
    _pinDataCommand = 0;

    // I could move this to Begin(), because I'm doing more than initialising local variables
    gioInit();
    etpwmInit();

}

void screenBegin() {
    // Screen_HX8353E::begin()

    /*
     * this is a 8MHz speed (16MHz Arduino speed/2)
     * In HalCoGen, on data format 0, I defined Baudrate 8000 kHz
     */
    mibspiInit();
    etpwmStartTBCLK();

    gioSetBit(_portReset, _pinReset, 1);
    delay(100);
    gioSetBit(_portReset, _pinReset, 0);
    delay(50);
    gioSetBit(_portReset, _pinReset, 1);
    delay(120);
    _writeCommand(HX8353E_SWRESET);
    delay(150);
    _writeCommand(HX8353E_SLPOUT);
    delay(200);
    _writeRegister(HX8353E_GAMSET, 0x04);
    _writeCommand(HX8353E_SETPWCTR);
    _writeData88(0x0A, 0x14);
    _writeCommand(HX8353E_SETSTBA);
    _writeData88(0x0A, 0x00);
    _writeRegister(HX8353E_COLMOD, 0x05);
    delay(10);
    _writeRegister(HX8353E_MADCTL, HX8353E_MADCTL_RGB);
    _writeCommand(HX8353E_CASET);
    _writeData8888(0x00, 0x00, 0x00, 0x79);
    _writeCommand(HX8353E_RASET);
    _writeData8888(0x00, 0x00, 0x00, 0x79);
    _writeCommand(HX8353E_NORON);
    delay(10);
    _writeCommand(HX8353E_DISPON);
    delay(120);
    _writeCommand(HX8353E_RAMWR);
    setBacklight(true);
    setOrientation(0);
    _screenWidth  = HX8353E_WIDTH;
    _screenHeigth = HX8353E_HEIGHT;
    _penSolid  = false;
    _fontSolid = true;
    _flagRead  = false;
    clear(blackColour);
}

// ported from Screen_HX8353E
void setBacklight(bool flag) {
    // todo: think of something that makes sence with PWM - maybe replace flag with duty cycle?

    if (flag) {
        etpwmREG6->CMPA = 550U;
    } else {
        etpwmREG6->CMPA = 0U;
    }
}

// ported from Screen_HX8353E
void setOrientation(uint16_t orientation) {
    _orientation = orientation % 4;
    _writeCommand(HX8353E_MADCTL);
    switch (_orientation) {
    case 0:
        _writeData(HX8353E_MADCTL_MX | HX8353E_MADCTL_MY | HX8353E_MADCTL_RGB);
        break;
    case 1:
        _writeData(HX8353E_MADCTL_MY | HX8353E_MADCTL_MV | HX8353E_MADCTL_RGB);
        break;
    case 2:
        _writeData(HX8353E_MADCTL_RGB);
        break;
    case 3:
        _writeData(HX8353E_MADCTL_MX | HX8353E_MADCTL_MV | HX8353E_MADCTL_RGB);
        break;
    }
}

/**
 *  from the TMS570LS043x_demo_software
 *
 */
void delay(uint32_t ms) {
    static volatile uint32_t delayval;
    delayval = 14684 * ms;   // this is a milisecond on an non-optimised compilation, RM46 LaunchPad, 110 MHz
    while(delayval--);
}


// ported from Screen_HX8353E
void _writeRegister(uint16_t command8, uint16_t data8)
{
    _writeCommand(command8);
    _writeData(data8);
}

// ported from Screen_HX8353E
void _writeCommand(uint16_t command8) {
    gioSetBit(_portDataCommand, _pinDataCommand, 0);

    mibspiSetData(mibspiREG3, 0, &command8);
    mibspiTransfer(mibspiREG3,0 );
    while(!(mibspiIsTransferComplete(mibspiREG3,0))) {
    }
}

// ported from Screen_HX8353E
void _writeData(uint16_t data8) {
    gioSetBit(_portDataCommand, _pinDataCommand, 1);
    mibspiSetData(mibspiREG3, 0, &data8);
    mibspiTransfer(mibspiREG3,0 );
    while(!(mibspiIsTransferComplete(mibspiREG3,0))) {
    }
}

// ported from Screen_HX8353E
void _writeData16(uint16_t data16) {
    gioSetBit(_portDataCommand, _pinDataCommand, 1);
    mibspiSetData(mibspiREG3, 1, &data16);
    mibspiTransfer(mibspiREG3, 1 );
    while(!(mibspiIsTransferComplete(mibspiREG3, 1))) {
    }
}

// ported from Screen_HX8353E
void _writeData88(uint16_t dataHigh16, uint16_t dataLow16) {
    uint16_t data = (dataHigh16 << 8) | 0x00FF ;
    uint16_t data_low = dataLow16 | 0xFF00;
    data &= data_low;
    gioSetBit(_portDataCommand, _pinDataCommand, 1);
    mibspiSetData(mibspiREG3, 1, &data);
    mibspiTransfer(mibspiREG3, 1 );
    while(!(mibspiIsTransferComplete(mibspiREG3, 1))) {
    }
}

// ported from Screen_HX8353E
void _writeData8888(uint16_t dataHigh8, uint16_t dataLow8, uint16_t data8_3, uint16_t data8_4) {
    _writeData(dataHigh8);
    _writeData(dataLow8);
    _writeData(data8_3);
    _writeData(data8_4);
}

// ported from LCD_screen
void clear(uint16_t colour) {
    uint16_t oldOrientation = _orientation;
    bool oldPenSolid = _penSolid;
    setOrientation(0);
    setPenSolid(true);
    rectangle(0, 0, screenSizeX()-1, screenSizeY()-1, colour);
    setOrientation(oldOrientation);
    setPenSolid(oldPenSolid);
}

// ported from LCD_screen
void setPenSolid(bool flag) {
    _penSolid = flag;
}

// ported from LCD_screen
void point(uint16_t x1, uint16_t y1, uint16_t colour) {
    _setPoint(x1, y1, colour);
}

// ported from LCD_screen
void rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t colour) {
    if (_penSolid == false) {
        line(x1, y1, x1, y2, colour);
        line(x1, y1, x2, y1, colour);
        line(x1, y2, x2, y2, colour);
        line(x2, y1, x2, y2, colour);
    } else {
        _fastFill(x1, y1, x2, y2, colour);
    }
}

// ported from LCD_screen
uint16_t screenSizeX() {
    switch (_orientation) {
    case 0:
    case 2:
        return _screenWidth;
        //            break;
    case 1:
    case 3:
        return _screenHeigth;
        //            break;
    }
    return 0U;
}

// ported from LCD_screen
uint16_t screenSizeY() {
    switch (_orientation) {
    case 0:
    case 2:
        return _screenHeigth;
        //            break;
    case 1:
    case 3:
        return _screenWidth;
        //            break;
    }
    return 0U;
}

// ported from LCD_screen
void circle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t colour) {
    int16_t f = 1 - radius;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * radius;
    int16_t x = 0;
    int16_t y = radius;
    if (_penSolid == false) {
        point(x0, y0+radius, colour);
        point(x0, y0-radius, colour);
        point(x0+radius, y0, colour);
        point(x0-radius, y0, colour);
        while (x < y) {
            if (f >= 0) {
                y--;
                ddF_y += 2;
                f += ddF_y;
            }
            x++;
            ddF_x += 2;
            f += ddF_x;
            point(x0 + x, y0 + y, colour);
            point(x0 - x, y0 + y, colour);
            point(x0 + x, y0 - y, colour);
            point(x0 - x, y0 - y, colour);
            point(x0 + y, y0 + x, colour);
            point(x0 - y, y0 + x, colour);
            point(x0 + y, y0 - x, colour);
            point(x0 - y, y0 - x, colour);
        }
    } else {
        while (x<y) {
            if (f >= 0) {
                y--;
                ddF_y += 2;
                f += ddF_y;
            }
            x++;
            ddF_x += 2;
            f += ddF_x;
            line(x0 + x, y0 + y, x0 - x, y0 + y, colour);
            line(x0 + x, y0 - y, x0 - x, y0 - y, colour);
            line(x0 + y, y0 - x, x0 + y, y0 + x, colour);
            line(x0 - y, y0 - x, x0 - y, y0 + x, colour);
        }
        setPenSolid(true);
        rectangle(x0-x, y0-y, x0+x, y0+y, colour);
    }
}

// ported from LCD_screen
void dLine(uint16_t x0, uint16_t y0, uint16_t dx, uint16_t dy, uint16_t colour) {
    line(x0, y0, x0 + dx - 1, y0 + dy - 1, colour);
}

// ported from LCD_screen
void line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t colour) {
    if ((x1 == x2) && (y1 == y2)) {
        _setPoint(x1, y1, colour);
    } else if ((x1 == x2) || (y1 == y2)) {
        _fastFill(x1, y1, x2, y2, colour);
    } else {
        int16_t wx1 = (int16_t)x1;
        int16_t wx2 = (int16_t)x2;
        int16_t wy1 = (int16_t)y1;
        int16_t wy2 = (int16_t)y2;
        boolean flag = abs(wy2 - wy1) > abs(wx2 - wx1);
        if (flag) {
            _swapI(&wx1, &wy1);
            _swapI(&wx2, &wy2);
        }
        if (wx1 > wx2) {
            _swapI(&wx1, &wx2);
            _swapI(&wy1, &wy2);
        }
        int16_t dx = wx2 - wx1;
        int16_t dy = abs(wy2 - wy1);
        int16_t err = dx / 2;
        int16_t ystep;
        if (wy1 < wy2) ystep = 1;
        else ystep = -1;
        for (; wx1<=wx2; wx1++) {
            if (flag) _setPoint(wy1, wx1, colour);
            else _setPoint(wx1, wy1, colour);
            err -= dy;
            if (err < 0) {
                wy1 += ystep;
                err += dx;
            }
        }
    }
}

// ported from LCD_screen
void _triangleArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t colour) {
    int16_t wx1 = (int16_t)x1;
    int16_t wy1 = (int16_t)y1;
    int16_t wx2 = (int16_t)x2;
    int16_t wy2 = (int16_t)y2;
    int16_t wx3 = (int16_t)x3;
    int16_t wy3 = (int16_t)y3;
    int16_t wx4 = wx1;
    int16_t wy4 = wy1;
    int16_t wx5 = wx1;
    int16_t wy5 = wy1;
    bool changed1 = false;
    bool changed2 = false;
    int16_t dx1 = abs(wx2 - wx1);
    int16_t dy1 = abs(wy2 - wy1);
    int16_t dx2 = abs(wx3 - wx1);
    int16_t dy2 = abs(wy3 - wy1);
    int16_t signx1 = (wx2 >= wx1) ? +1 : -1;
    int16_t signx2 = (wx3 >= wx1) ? +1 : -1;
    int16_t signy1 = (wy2 >= wy1) ? +1 : -1;
    int16_t signy2 = (wy3 >= wy1) ? +1 : -1;
    if (dy1 > dx1) {
        _swapI(&dx1, &dy1);
        changed1 = true;
    }
    if (dy2 > dx2) {
        _swapI(&dx2, &dy2);
        changed2 = true;
    }
    int16_t e1 = 2 * dy1 - dx1;
    int16_t e2 = 2 * dy2 - dx2;
    uint32_t i = 0;
    for (i = 0; i <= dx1; i++) {
        line(wx4, wy4, wx5, wy5, colour);
        while (e1 >= 0) {
            if (changed1) wx4 += signx1;
            else wy4 += signy1;
            e1 = e1 - 2 * dx1;
        }
        if (changed1) wy4 += signy1;
        else wx4 += signx1;
        e1 = e1 + 2 * dy1;
        while (wy5 != wy4) {
            while (e2 >= 0) {
                if (changed2) wx5 += signx2;
                else wy5 += signy2;
                e2 = e2 - 2 * dx2;
            }
            if (changed2) wy5 += signy2;
            else wx5 += signx2;
            e2 = e2 + 2 * dy2;
        }
    }
}

// ported from LCD_screen
void triangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t colour)
{
    if (_penSolid) {
        bool b = true;
        while (b) {
            b = false;
            if ((b == false) && (y1 > y2)) {
                _swapU(&x1, &x2);
                _swapU(&y1, &y2);
                b = true;
            }
            if ((b == false) && (y2 > y3)) {
                _swapU(&x3, &x2);
                _swapU(&y3, &y2);
                b = true;
            }
        }
        if (y2 == y3) {
            _triangleArea(x1, y1, x2, y2, x3, y3, colour);
        } else if (y1 == y2) {
            _triangleArea(x3, y3, x1, y1, x2, y2, colour);
        } else {
            uint16_t x4 = (uint16_t)( (int32_t)x1 + (y2 - y1) * (x3 - x1) / (y3 - y1) );
            uint16_t y4 = y2;
            _triangleArea(x1, y1, x2, y2, x4, y4, colour);
            _triangleArea(x3, y3, x2, y2, x4, y4, colour);
        }
    } else {
        line(x1, y1, x2, y2, colour);
        line(x2, y2, x3, y3, colour);
        line(x3, y3, x1, y1, colour);
    }
}



// ported from Screen_HX8353E
void _fastFill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t colour) {
    if (x1 > x2) _swapU(&x1, &x2);
    if (y1 > y2) _swapU(&y1, &y2);
    _setWindow(x1, y1, x2, y2);
    gioSetBit(_portDataCommand, _pinDataCommand, 1);

//    uint32_t t; // todo: this can be extremely optimized using the mibspi buffer + dma
//    for (t=(uint32_t)(y2-y1+1)*(x2-x1+1); t>1; t--) {
//        mibspiSetData(mibspiREG3, 2, &colour);
//        mibspiTransfer(mibspiREG3, 2 );
//        while(!(mibspiIsTransferComplete(mibspiREG3, 2))) {
//        }
//    }
//    mibspiSetData(mibspiREG3, 1, &colour);
//    mibspiTransfer(mibspiREG3, 1 );
//    while(!(mibspiIsTransferComplete(mibspiREG3, 1))) {
//    }
    uint32_t t; // todo: this can be extremely optimized using the mibspi buffer + dma
    for (t=(uint32_t)(y2-y1+1)*(x2-x1+1); t>0; t--) {
        mibspiSetData(mibspiREG3, 1, &colour);
        mibspiTransfer(mibspiREG3, 1 );
        while(!(mibspiIsTransferComplete(mibspiREG3, 1))) {
        }
    }
}

// ported from Screen_HX8353E
void _setPoint(uint16_t x1, uint16_t y1, uint16_t colour) {
    if( /*(x1 < 0) || */(x1 >= screenSizeX()) || /*(y1 < 0) || */(y1 >= screenSizeY()) ) return;
    _setWindow(x1, y1, x1+1, y1+1);
    _writeData16(colour);
}

// ported from Screen_HX8353E
void _setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    switch (_orientation) {
    case 0:
        x0 += 2;
        y0 += 3;
        x1 += 2;
        y1 += 3;
        break;
    case 1:
        x0 += 3;
        y0 += 2;
        x1 += 3;
        y1 += 2;
        break;
    case 2:
        x0 += 2;
        y0 += 1;
        x1 += 2;
        y1 += 1;
        break;
    case 3:
        x0 += 1;
        y0 += 2;
        x1 += 1;
        y1 += 2;
        break;
    default:
        break;
    }
    _writeCommand(HX8353E_CASET);
    _writeData16(x0);
    _writeData16(x1);
    _writeCommand(HX8353E_RASET);
    _writeData16(y0);
    _writeData16(y1);
    _writeCommand(HX8353E_RAMWR);
}

// ported from LCD_screen
void _swapU(uint16_t *a, uint16_t *b) {
    uint16_t w = *a;
    *a = *b;
    *b = w;
}

// ported from LCD_screen
void _swapI(int16_t *a, int16_t *b) {
    int16_t w = *a;
    *a = *b;
    *b = w;
}

//ported fromScreen_HX8353E
void invert(boolean flag) {
    _writeCommand(flag ? HX8353E_INVON : HX8353E_INVOFF);
}

// ported from LCD_screen_font
void setFontSize(uint32_t size) {
    if (size < MAX_FONT_SIZE) _fontSize = size;
    else _fontSize = MAX_FONT_SIZE ;
}

// ported from LCD_screen_font
uint32_t fontMax() {
    return MAX_FONT_SIZE;
}

// ported from LCD_screen_font
uint32_t fontSizeX() {
    if (_fontSize == 0) return 6;
    else if (_fontSize == 1) return 8;
    else if (_fontSize == 2) return 12;
    else return 0;
}

// ported from LCD_screen_font
uint32_t fontSizeY() {
    if (_fontSize == 0) return 8;
    else if (_fontSize == 1) return 12;
    else if (_fontSize == 2) return 16;
    else return 0;
}

// ported from LCS_screen_font
void setFontSolid(bool flag) {
    _fontSolid = flag;
}

// ported from LCS_screen_font
uint32_t _getCharacter(uint32_t c, uint32_t i) {
    if (_fontSize == 0) {
        return Terminal6x8e[c][i];
    } else if (_fontSize == 1) {
        return Terminal8x12e[c][i];
    } else if (_fontSize == 2) {
        return Terminal12x16e[c][i];
    }
    else {
        return 0;
    }
}

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)

// ported from LCS_screen_font
void gText(uint16_t x0, uint16_t y0,
        uint8_t *s, uint32_t length,
        uint16_t textColour, uint16_t backColour,
        uint32_t ix, uint32_t iy) {
    uint8_t c;
    uint32_t line, line1, line2;
    uint32_t x, y;
    uint32_t i, j, k;
    if ((ix > 1) || (iy > 1) || !_fontSolid) {
        if ((_fontSize == 0) && ((ix > 1) || (iy > 1))) {
            bool oldPenSolid = _penSolid;
            setPenSolid(true);
            for (k=0; k<length; k++) {
                x = x0 + 6 * k * ix;
                y = y0;
                c = s[k]-' ';
                for ( i=0; i<6; i++ ) {
                    line = _getCharacter(c, i);
                    for ( j = 0; j<8; j++) {
                        if (bitRead(line, j)) rectangle(x+i*ix, y+j*iy, x+i*ix+ix-1, y+j*iy+iy-1, textColour);
                        else if (_fontSolid)  rectangle(x+i*ix, y+j*iy, x+i*ix+ix-1, y+j*iy+iy-1, backColour);
                    }
                }
            }
            setPenSolid(oldPenSolid);
        } else {
            if (_fontSize == 0) {
                for (k=0; k<length; k++) {
                    c = s[k]-' ';
                    for (i=0; i<6; i++) {
                        line = _getCharacter(c, i);
                        for (j=0; j<8; j++)
                            if (bitRead(line, j)) point(x0 + 6*k + i, y0 + j, textColour);
                            else if (_fontSolid)  point(x0 + 6*k + i, y0 + j, backColour);
                    }
                }
            }
            else if (_fontSize == 1) {
                for (k=0; k<length; k++) {
                    c = s[k]-' ';
                    for (i=0; i<8; i++) {
                        line = _getCharacter(c, 2*i);
                        line1 =_getCharacter(c, 2*i+1);
                        for (j=0; j<8; j++) {
                            if (bitRead(line, j)) point(x0 + 8*k + i, y0 + j, textColour);
                            else if (_fontSolid)  point(x0 + 8*k + i, y0 + j, backColour);
                            if (bitRead(line1, j))  point(x0 + 8*k + i, y0 +8 + j, textColour);
                            else if ((_fontSolid) && (j<4)) point(x0 + 8*k + i, y0 +8 + j, backColour);
                        }
                    }
                }
            }
            else if (_fontSize == 2) {
                for (k=0; k<length; k++) {
                    c = s[k]-' ';
                    for (i=0; i<12; i++) {
                        line = _getCharacter(c, 2*i);
                        line1 =_getCharacter(c, 2*i+1);
                        for (j=0; j<8; j++) {
                            if (bitRead(line, j))  point(x0 + 12*k + i, y0 + j,    textColour);
                            else if (_fontSolid)   point(x0 + 12*k + i, y0 + j,    backColour);
                            if (bitRead(line1, j)) point(x0 + 12*k + i, y0 +8 + j, textColour);
                            else if (_fontSolid)   point(x0 + 12*k + i, y0 +8 + j, backColour);
                        }
                    }
                }
            }
            else if (_fontSize == 3) {
                for (k=0; k<length; k++) {
                    c = s[k]-' ';
                    for (i=0; i<16; i++) {
                        line  =_getCharacter(c, 3*i);
                        line1 =_getCharacter(c, 3*i+1);
                        line2 =_getCharacter(c, 3*i+2);
                        for (j=0; j<8; j++) {
                            if (bitRead(line, j))  point(x0 + 16*k + i, y0 + j,     textColour);
                            else if (_fontSolid)   point(x0 + 16*k + i, y0 + j,     backColour);
                            if (bitRead(line1, j)) point(x0 + 16*k + i, y0 +8 + j,  textColour);
                            else if (_fontSolid)   point(x0 + 16*k + i, y0 +8 + j,  backColour);
                            if (bitRead(line2, j)) point(x0 + 16*k + i, y0 +16 + j, textColour);
                            else if (_fontSolid)   point(x0 + 16*k + i, y0 +16 + j, backColour);
                        }
                    }
                }
            }
        }
    } else {
        uint8_t c;
        uint32_t i, j, k;
        if (_fontSize == 0) {
            for (k=0; k<length; k++) {
                c = s[k]-' ';
                _setWindow(x0 +fontSizeX()*k, y0, x0 +fontSizeX()*(k+1)-1, y0+fontSizeY()-1);
                for (j=0; j<8; j++) {
                    for (i=0; i<6; i++) {
                        line = _getCharacter(c, i);
                        if (bitRead(line, j)) {
                            _writeData16(textColour);
                        } else {
                            _writeData16(backColour);
                        }
                    }
                }
            }
        }
        else if (_fontSize == 1) {
            for (k=0; k<length; k++) {
                c = s[k]-' ';
                _setWindow(x0 +fontSizeX()*k, y0, x0 +fontSizeX()*(k+1)-1, y0+fontSizeY()-1);
                for (j=0; j<8; j++) {
                    for (i=0; i<8; i++) {
                        line = _getCharacter(c, 2*i);
                        if (bitRead(line, j)) {
                            _writeData16(textColour);
                        } else {
                            _writeData16(backColour);
                        }
                    }
                }
                for (j=0; j<4; j++) {
                    for (i=0; i<8; i++) {
                        line = _getCharacter(c, 2*i+1);
                        if (bitRead(line, j)) {
                            _writeData16(textColour);
                        } else {
                            _writeData16(backColour);
                        }
                    }
                }
            }
        }
        else if (_fontSize == 2) {
            for (k=0; k<length; k++) {
                c = s[k]-' ';
                _setWindow(x0 +fontSizeX()*k, y0, x0 +fontSizeX()*(k+1)-1, y0+fontSizeY()-1);
                for (j=0; j<8; j++) {
                    for (i=0; i<12; i++) {
                        line = _getCharacter(c, 2*i);
                        if (bitRead(line, j)) {
                            _writeData16(textColour);
                        } else {
                            _writeData16(backColour);
                        }
                    }
                }
                for (j=0; j<8; j++) {
                    for (i=0; i<12; i++) {
                        line = _getCharacter(c, 2*i+1);
                        if (bitRead(line, j)) {
                            _writeData16(textColour);
                        } else {
                            _writeData16(backColour);
                        }
                    }
                }
            }
        }
        else if (_fontSize == 3) {
            for (k=0; k<length; k++) {
                c = s[k]-' ';
                _setWindow(x0 +fontSizeX()*k, y0, x0 +fontSizeX()*(k+1)-1, y0+fontSizeY()-1);
                for (j=0; j<8; j++) {
                    for (i=0; i<16; i++) {
                        line = _getCharacter(c, 3*i);
                        if (bitRead(line, j)) {
                            _writeData16(textColour);
                        } else {
                            _writeData16(backColour);
                        }
                    }
                }
                for (j=0; j<8; j++) {
                    for (i=0; i<16; i++) {
                        line = _getCharacter(c, 3*i+1);
                        if (bitRead(line, j)) {
                            _writeData16(textColour);
                        } else {
                            _writeData16(backColour);
                        }
                    }
                }
                for (j=0; j<8; j++) {
                    for (i=0; i<16; i++) {
                        line = _getCharacter(c, 3*i+2);
                        if (bitRead(line, j)) {
                            _writeData16(textColour);
                        } else {
                            _writeData16(backColour);
                        }
                    }
                }
            }
        }
    }
}


