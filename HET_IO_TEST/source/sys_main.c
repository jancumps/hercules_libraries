/** @file sys_main.c 
*   @brief Application main file
*   @date 9.Sep.2014
*   @version 04.01.00
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* (c) Texas Instruments 2009-2014, All rights reserved. */

/* USER CODE BEGIN (0) */
/* USER CODE END */

/* Include Files */

#include "sys_common.h"

/* USER CODE BEGIN (1) */
#include "het.h"
#include "HET_IO_TEST.h"





// #define WAIT 500
#define WAIT 0



/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */
/* USER CODE END */

void main(void)
{
/* USER CODE BEGIN (3) */
	volatile unsigned int delay;
	HETPROGRAM0_UN * pHetProgram;

	hetInit();
	uint32 counter = 0;
	while (1) {
		pHetProgram = (HETPROGRAM0_UN *)hetRAM1;
		pHetProgram -> Program0_ST.i2c_init_0.djz.data = counter;


		for(delay = 0; delay < WAIT; delay++);

		counter++;
		if (counter == 6) {
			counter = 0;
		}

	}
	/* USER CODE END */
}

/* USER CODE BEGIN (4) */
/* USER CODE END */
