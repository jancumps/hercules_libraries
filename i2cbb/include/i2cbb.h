/*
 * i2cbb.h
 *
 *  Created on: 19-mei-2014
 *      Author: jan
 */

#ifndef I2CBB_H_
#define I2CBB_H_

#include "gioutils.h"

void i2c_init(const gioPin_t *scl, const gioPin_t *sda);
void i2c_start();
void i2c_stop();
void i2c_putByte(uint8 byte);
uint8 i2c_getByte();
void i2c_read(uint8 address, uint8 *buffer, uint32 count, uint32 callStart, uint32 callStop);
void i2c_write(uint8 address, uint8 *buffer, uint32 count, uint32 callStart, uint32 callStop);

#endif /* I2CBB_H_ */
