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
void i2c_start_cond();
void i2c_stop_cond();
void i2c_write_bit(uint32 bit);
uint32 i2c_read_bit();
uint32 i2c_write_byte(uint32 send_start, uint32 send_stop, unsigned char byte);
unsigned char i2c_read_byte(uint32 nack, uint32 send_stop);

#endif /* I2CBB_H_ */
