/*
 * i2cbb.c
 *
 *  Created on: 19-mei-2014
 *      Author: jan
 *
 */


/**
/ $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ /
/ **** I2C Driver . (c)95-97 V.Himpe . Public Domain release *** /
/ $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ /
 * http://www.esacademy.com/en/library/technical-articles-and-documents/miscellaneous/i2c-bus/miscellaneous-information/i2c-driver-in-psudocode.html
*/

#include "i2cbb.h"

// SCL and SDA pins:

const gioPin_t * _vh_scl = NULL;
const gioPin_t * _vh_sda = NULL;

// #define _i2c_wait 1 // 140 kHz
// #define _i2c_wait 25
//#define _i2c_wait 2 // 105 kHz
#define _i2c_wait 4 // 68 kHz


/**
* delay for the given number of microseconds, for 80MHz processor
*/
void vh_i2cDelayMicroSeconds(uint32 ms) {
	uint32 i = 0;
	for( i=0; i<ms*5; i++) {
		asm(" nop");
	}
}

void i2c_vh_init(const gioPin_t *scl, const gioPin_t *sda) {

	int i = 0;
	// two variables that will keep the pin assignment all the time
	_vh_scl = scl;
	_vh_sda = sda;
	// initialize both as outpins
	gioutilsSetPinDirection(_vh_scl, pinWrite);
	gioutilsSetPinDirection(_vh_sda, pinWrite);
	/*
	 * SUBroutine I2C_INIT / call this immediately after power-on /
	 *       SDA=1
	 *       SCK=0
	 *       FOR n = 0 to 3
	 *             CALL STOP
	 *       NEXT n
	 * ENDsub
	 */
	gioutilsSetPin(_vh_sda, 1);
	vh_i2cDelayMicroSeconds(_i2c_wait);
	gioutilsSetPin(_vh_scl, 0);
	vh_i2cDelayMicroSeconds(_i2c_wait);
	for(i = 0; i <= 3; i++) {
		i2c_stop();
	}
}

void i2c_start() {
	/*
	 * SUBroutine START
	 *       SCK=1 / BUGFIX !/
	 *       SDA=1 / Improvement /
	 *       SDA=0
	 *       SCK=0
	 *       SDA=1
	 * ENDsub
	 */
	gioutilsSetPin(_vh_scl, 1);
	vh_i2cDelayMicroSeconds(_i2c_wait);
	gioutilsSetPin(_vh_sda, 1);
	vh_i2cDelayMicroSeconds(_i2c_wait);
	gioutilsSetPin(_vh_sda, 0);
	vh_i2cDelayMicroSeconds(_i2c_wait);
	gioutilsSetPin(_vh_scl, 0);
	vh_i2cDelayMicroSeconds(_i2c_wait);
	gioutilsSetPin(_vh_sda, 1);
	vh_i2cDelayMicroSeconds(_i2c_wait);
}

void i2c_stop() {
	/*
	 * SUBroutine STOP
	 *       SDA=0
	 *       SCK=1
	 *       SDA=1
	 * ENDsub
	 */
	gioutilsSetPin(_vh_sda, 0);
	vh_i2cDelayMicroSeconds(_i2c_wait);
	gioutilsSetPin(_vh_scl, 1);
	vh_i2cDelayMicroSeconds(_i2c_wait);
	gioutilsSetPin(_vh_sda, 1);
	vh_i2cDelayMicroSeconds(_i2c_wait);
}

void i2c_putByte(uint8 byte) {
	/*
	 * SUBroutine PUTBYTE(BUFFER)
	 *       FOR n = 7 TO 0
	 *             SDA= BIT(n) of BUFFER
	 *             SCK=1
	 *             SCK=0
	 *       NEXT n
	 *       SDA=1
	 * ENDsub
	 */
	int i = 0;

	for (i = 7; i >= 0; i-- ) {
		uint32 bit = ((byte >> i) & 0x01);
		gioutilsSetPin(_vh_sda, bit);
		vh_i2cDelayMicroSeconds(_i2c_wait);
		gioutilsSetPin(_vh_scl, 1);
		vh_i2cDelayMicroSeconds(_i2c_wait);
		gioutilsSetPin(_vh_scl, 0);
		vh_i2cDelayMicroSeconds(_i2c_wait);
	}
	gioutilsSetPin(_vh_sda, 1);
	vh_i2cDelayMicroSeconds(_i2c_wait);
}

uint8 i2c_getByte() {
	/*
	 * SUBroutine GETBYTE
	 *       FOR n = 7 to 0
	 *             SCK=1
	 *             BIT(n) OF BUFFER = SDA
	 *             SCK=0
	 *       NEXT n
	 *       SDA=1
	 * ENDsub
	 */
	int i = 0;
	uint8 byte = 0;

	gioutilsSetPinDirection(_vh_sda, pinRead);
	for (i = 7; i >= 0; i-- ) {
		gioutilsSetPin(_vh_scl, 1);
		vh_i2cDelayMicroSeconds(_i2c_wait);
		uint32 bit = gioutilsGetPin(_vh_sda);
		vh_i2cDelayMicroSeconds(_i2c_wait);
		byte = byte | (bit << i);
		gioutilsSetPin(_vh_scl, 0);
		vh_i2cDelayMicroSeconds(_i2c_wait);
	}
	gioutilsSetPinDirection(_vh_sda, pinWrite);
	gioutilsSetPin(_vh_sda, 1);
	vh_i2cDelayMicroSeconds(_i2c_wait);
	return byte;
}

void i2c_giveAck() {
	/*
	 * SUBroutine GIVEACK
	 *       SDA=0
	 *       SCK=1
	 *       SCK=0
	 *       SDA=1
	 * ENDsub
	 */
	gioutilsSetPin(_vh_sda, 0);
	vh_i2cDelayMicroSeconds(_i2c_wait);
	gioutilsSetPin(_vh_scl, 1);
	vh_i2cDelayMicroSeconds(_i2c_wait);
	gioutilsSetPin(_vh_scl, 0);
	vh_i2cDelayMicroSeconds(_i2c_wait);
	gioutilsSetPin(_vh_sda, 1);
	vh_i2cDelayMicroSeconds(_i2c_wait);
}

void i2c_getAck() {
	/*
	 * SUBroutine GETACK
	 *       SDA=1
	 *       SCK=1
	 *       WAITFOR SDA=0
	 *       SCK=0
	 * ENDSUB
	 */
	gioutilsSetPin(_vh_sda, 1);
	vh_i2cDelayMicroSeconds(_i2c_wait);
	gioutilsSetPinDirection(_vh_sda, pinRead);
	vh_i2cDelayMicroSeconds(_i2c_wait);
	gioutilsSetPin(_vh_scl, 1);
	vh_i2cDelayMicroSeconds(_i2c_wait);
	while (gioutilsGetPin(_vh_sda)) {
		vh_i2cDelayMicroSeconds(_i2c_wait);
		// WAITFOR SDA=0
	}
	gioutilsSetPinDirection(_vh_sda, pinWrite);
/*
  	gioutilsSetPin(_vh_sda, 1); // TODO: controversial
	vh_i2cDelayMicroSeconds(_i2c_wait); // TODO: controversial
*/
	gioutilsSetPin(_vh_scl, 0);
	vh_i2cDelayMicroSeconds(_i2c_wait);
}

/*
 * / this concludes the low-level set of instructions for the I2C driver
 *   The next functions will handle the telegram formatting on a higher level /
 */

void i2c_read(uint8 address, uint8 *buffer, uint32 count, uint32 callStart, uint32 callStop) {
	/*
	 * SUBroutine READ(Device_address,Number_of_bytes)
	 *       Device_adress=Device_adress OR (0000.0001)b /This sets the READ FLAG/
	 *       CALL START
	 *       CALL PUTBYTE(Device_adress)
	 *       CALL GETACK
	 *       FOR x = 0 to Number_of_bytes
	 *             CALL GETBYTE DATA(x)=BUFFER /Copy received BYTE to DATA array /
	 *             IF X< Number_of_bytes THEN /Not ack the last byte/
	 *                   CALL GIVEACK
	 *             END IF
	 *       NEXT x
	 *       CALL STOP
	 * ENDsub
	 */
	int i = 0;
//	address = address | 0x01;
	address= address << 1;
	address = address | 0x01;
	if (callStart) {
		i2c_start();
	}
	i2c_putByte(address);
	i2c_getAck();
	for (i=0; i < count; i++) {
		buffer[i] = i2c_getByte();
		if (i < (count-1)) { // TODO check
			i2c_giveAck();
		}
	}
	if (callStop) {
		i2c_stop();
	}
}

void i2c_write(uint8 address, uint8 *buffer, uint32 count, uint32 callStart, uint32 callStop) {
	/*
	 * SUBroutine WRITE(Device_address,Number_of_bytes)
	 *       Device_adress=Device_adress AND (1111.1110)b / This clears READ flag /
	 *       CALL START
	 *       CALL PUTBYTE(Device_adress)
	 *       CALL GETACK
	 *       FOR x = 0 to Number_of_bytes
	 *             CALL PUTBYTE (DATA(x))
	 *             CALL GETACK
	 *       NEXT x
	 *       CALL STOP
	 * ENDsub
	 */
	int i = 0;
//	address = address & 0xFE;
	address = address << 1;
	if (callStart) {
		i2c_start();
	}
	i2c_putByte(address);
	i2c_getAck();
	for (i=0; i < count; i++) {
		i2c_putByte(buffer[i]);
		i2c_getAck();
	}
	if (callStop) {
		i2c_stop();
	}
}


