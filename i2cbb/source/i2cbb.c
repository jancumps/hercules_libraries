/*
 * i2cbb.c
 *
 *  Created on: 19-mei-2014
 *      Author: jan
 *
 *      http://en.wikipedia.org/wiki/I%C2%B2C
 */


#include "gioutils.h"
#include "gio.h"


// SCL and SDA pins:

const gioPin_t * _scl = NULL;
const gioPin_t * _sda = NULL;

void i2cDelayMicroSeconds( uint32 ms);

void i2c_init(const gioPin_t *scl, const gioPin_t *sda) {
	_scl = scl;
	_sda = sda;
}

void I2C_delay() {
	i2cDelayMicroSeconds(1);
}
//{ volatile int v; int i; for (i=0; i < I2CSPEED/2; i++) v; }

bool read_SCL(void){
	// Set SCL as input and return current level of line, 0 or 1
	gioutilsSetPinDirection(_scl, pinRead);
	uint32 retval = gioutilsGetPin(_scl);
	return (retval == true);
}
bool read_SDA() {
	// Set SDA as input and return current level of line, 0 or 1
	gioutilsSetPinDirection(_sda, pinRead);
	uint32 retval = gioutilsGetPin(_sda);
	return (retval == true);
}

void clear_SCL() {
	// Actively drive SCL signal low
	gioutilsSetPinDirection(_scl, pinWrite);
	gioutilsSetPin(_scl, 0);

}
void clear_SDA() {
	// Actively drive SDA signal low
	gioutilsSetPinDirection(_sda, pinWrite);
	gioutilsSetPin(_sda, 0);
}
void arbitration_lost() {

}

bool started = false; // global data

void i2c_start_cond() {
  if (started) { // if started, do a restart cond
    // set SDA to 1
    read_SDA();
    I2C_delay();
    while (read_SCL() == 0) {  // Clock stretching
      // You should add timeout to this loop
    }
    // Repeated start setup time, minimum 4.7us
    I2C_delay();
  }
  if (read_SDA() == 0) {
    arbitration_lost();
  }
  // SCL is high, set SDA from 1 to 0.
  clear_SDA();
  I2C_delay();
  clear_SCL();
  started = true;
}

void i2c_stop_cond(){
  // set SDA to 0
  clear_SDA();
  I2C_delay();
  // Clock stretching
  while (read_SCL() == 0) {
    // add timeout to this loop.
  }
  // Stop bit setup time, minimum 4us
  I2C_delay();
  // SCL is high, set SDA from 0 to 1
  if (read_SDA() == 0) {
    arbitration_lost();
  }
  I2C_delay();
  started = false;
}

// Write a bit to I2C bus
void i2c_write_bit(uint32 bit) {
  if (bit) {
    read_SDA();
  } else {
    clear_SDA();
  }
  I2C_delay();
  while (read_SCL() == 0) { // Clock stretching
    // You should add timeout to this loop
  }
  // SCL is high, now data is valid
  // If SDA is high, check that nobody else is driving SDA
  if (bit && read_SDA() == 0) {
    arbitration_lost();
  }
  I2C_delay();
  clear_SCL();
}

// Read a bit from I2C bus
bool i2c_read_bit() {
  bool bit;
  // Let the slave drive data
  read_SDA();
  I2C_delay();
  while (read_SCL() == 0) { // Clock stretching
    // You should add timeout to this loop
  }
  // SCL is high, now data is valid
  bit = read_SDA();
  I2C_delay();
  clear_SCL();
  return bit;
}

// Write a byte to I2C bus. Return 0 if ack by the slave.
bool i2c_write_byte(uint32 send_start,
		uint32 send_stop,
                    unsigned char byte) {
  unsigned bit;
  bool nack;
  if (send_start) {
    i2c_start_cond();
  }
  for (bit = 0; bit < 8; bit++) {
    i2c_write_bit((byte & 0x80) != 0);
    byte <<= 1;
  }
  nack = i2c_read_bit();
  if (send_stop) {
    i2c_stop_cond();
  }
  return nack;
}

// Read a byte from I2C bus
unsigned char i2c_read_byte(uint32 nack, uint32 send_stop) {
  unsigned char byte = 0;
  unsigned bit;
  for (bit = 0; bit < 8; bit++) {
    byte = (byte << 1) | i2c_read_bit();
  }
  i2c_write_bit(nack);
  if (send_stop) {
    i2c_stop_cond();
  }
  return byte;
}

/**
* delay for the given number of microseconds, for 80MHz processor
*/
void i2cDelayMicroSeconds(uint32 ms) {
uint32 i = 0;
for( i=0; i<ms*5; i++) {
asm(" nop");
}
}
