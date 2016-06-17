/*
 * bitmaputils.h
 *
 *  Created on: 16 mei 2016
 *      Author: jancu
 */

#ifndef BITMAPUTILS_BITMAPUTILS_H_
#define BITMAPUTILS_BITMAPUTILS_H_

#include <stdint.h>


 typedef struct {
     const uint16_t *data;
     uint32_t width;
     uint32_t height;
     } bitmap_t;




void drawBitmap(bitmap_t *bmp, uint32_t x, uint32_t y);
void setBitmapInOneThrow(bitmap_t *bmp);

#endif /* BITMAPUTILS_BITMAPUTILS_H_ */
