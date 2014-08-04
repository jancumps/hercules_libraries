/*
 * gioutils.c
 *
 *  Created on: 18-mei-2014
 *      Author: jan
 */


#include "gioutils.h"

/**
 * set the direction of a given pin without altering other pins
 * on the same port
 *
 */
void gioutilsSetPinDirection(const gioPin_t *pin, t_pinDir dir) {
	uint32 uDir = pin->port->DIR;
	if(dir==pinWrite) {
		uDir |= (1U << pin->bit);
	} else {
		uDir &= ~(1U << pin->bit);
	}
	gioSetDirection(pin->port, uDir);
}

/**
 * set the open drain option of a given pin without altering other pins
 * on the same port
 *
 */
void gioutilsSetPinOpenDrainEnable(const gioPin_t *pin, t_pinOpenDrainOption openDrainOption) {
	uint32 uPdr = pin->port->PDR;
	if(openDrainOption==pinOpenDrainEnable) {
		uPdr |= (1U << pin->bit);
	} else {
		uPdr &= ~(1U << pin->bit);
	}
	pin->port->PDR = uPdr;
}


/**
 * set the value of a given pin
 *
 */
void gioutilsSetPin(const gioPin_t *pin, uint32 value) {
	gioSetBit(pin->port, pin->bit, value);
}

/**
 * get the value of a given pin
 *
 */
uint32 gioutilsGetPin(const gioPin_t *pin) {
	return gioGetBit(pin->port, pin->bit);
}

