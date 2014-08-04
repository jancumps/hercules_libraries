/*
 * gioutils.h
 *
 *  Created on: 18-mei-2014
 *      Author: jan
 */

#ifndef GIOUTILS_H_
#define GIOUTILS_H_

#include "gio.h"

typedef struct {
		gioPORT_t *port; // the port the signal line is on
		uint32 bit;		 // the signal line
} gioPin_t;



typedef enum {pinRead, pinWrite} t_pinDir;
typedef enum {pinOpenDrainDisable, pinOpenDrainEnable} t_pinOpenDrainOption;
void gioutilsSetPinDirection(const gioPin_t *pin, t_pinDir dir);
void gioutilsSetPinOpenDrainEnable(const gioPin_t *pin, t_pinOpenDrainOption openDrainOption);
void gioutilsSetPin(const gioPin_t *pin, uint32 value);
uint32 gioutilsGetPin(const gioPin_t *pin);


#endif /* GIOUTILS_H_ */
