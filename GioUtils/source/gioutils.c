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
 * enable / disable the open drain option of a given pin without altering other pins
 * on the same port
 *
 */
void gioutilsSetPinOpenDrainEnable(const gioPin_t *pin, t_pinOpenDrainEnable openDrainEnable) {
	uint32 uPdr = pin->port->PDR;
	if(openDrainEnable==pinOpenDrainEnable) {
		uPdr |= (1U << pin->bit);
	} else {
		uPdr &= ~(1U << pin->bit);
	}
	pin->port->PDR = uPdr;
}

/**
 * set the pull resistor direction of a given pin without altering other pins
 * on the same port
 *
 */
void gioutilsSetPinPullResistorDirection(const gioPin_t *pin, t_pinPullResistorDirection pullResistorDirection) {
	uint32 uPsl = pin->port->PSL;
	if(pullResistorDirection==pinPullResistorUp) {
		uPsl |= (1U << pin->bit);
	} else {
		uPsl &= ~(1U << pin->bit);
	}
	pin->port->PSL = uPsl;
}

/**
 * enable / disable the pull resistor of a given pin without altering other pins
 * on the same port
 *
 */
void gioutilsSetPinPullResistorEnable(const gioPin_t *pin, t_pinPullResistorEnable pullResistorEnable) {
	uint32 uPuldis = pin->port->PULDIS;
	if(pullResistorEnable==pinPullResistorEnable) {
		uPuldis |= (1U << pin->bit);
	} else {
		uPuldis &= ~(1U << pin->bit);
	}
	pin->port->PULDIS = uPuldis;
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

