/*
 * bitmaputils.c
 *
 *  Created on: 16 mei 2016
 *      Author: jancu
 */

#include "bitmaputils.h"

#include "sys_common.h"
#include "stddef.h"
#include "mibspi.h"
#include "sys_dma.h"


#include "HX8353E.h"

/* buffer memory in special location for DMA / MibSPI */
#define D_SIZE      128

uint16_t TXDATA[D_SIZE];         /* transmit buffer in sys ram */

void loadBitmapInDMABuffer(bitmap_t *bmp, uint32_t row);


void drawBitmap(bitmap_t *bmp, uint32_t x, uint32_t y) {
	uint32_t xx, yy;

    for (yy = 0; yy < bmp->height; yy++) {
        for (xx = 0; xx < bmp->width; xx++) {
            point(xx, yy, (bmp->data)[(yy)*bmp->height + (xx) ]);
        }
    }
}


void loadBitmapInDMABuffer(bitmap_t *bmp, uint32_t row) {
    memcpy(&TXDATA, (const void *)(bmp->data + (bmp->width) * row), 2 * D_SIZE);
//	memcpy((void *)(&TXDATA), (const void *)(bmp->data[row]), 2 * D_SIZE);

}

void flashBitmapLine(bitmap_t *bmp, uint32_t line) {
	// todo: turn this into a DMA operation
//	uint32_t xx;
 	loadBitmapInDMABuffer(bmp, line);
 	_writeData64(&TXDATA[0]);
 	_writeData64(&TXDATA[63]);


}

// ported from Screen_HX8353E
void setBitmapInOneThrow(bitmap_t *bmp) {
	uint32_t yy;

    for (yy = 0; yy < bmp->height; yy++) {
        _setWindow(0, yy, bmp->width, yy );
        flashBitmapLine(bmp, yy);
    }
}







