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
#include "HET_EMU.h"

unsigned char *I2C1_txptr = NULL, *I2C1_rxptr = NULL;
unsigned char I2C1_TxData[5] = {0, 0, 0, 0, 0};
unsigned char I2C1_RxData[20];
unsigned int Data_Send_HET = 0, Data_Rece_HET = 0;
unsigned int Data_Send_I2C = 0, Data_Rece_I2C = 0;
// unsigned int Stop_Rece_I2C = 0;
unsigned int Start_Repeat=0;
char IntEna =1;
char RW = 0; //write
char I2C_ADDR; // todo: jc 20141024 this is a hack to make the TI emukator code work, candidate for refactoring
char NumOfBytes = 1;


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

	// set the i2c address
	I2C_ADDR = 0x55;


	int i;

	NumOfBytes = 1;
	char StopBit = 1;
	volatile unsigned int wait_counter;



	I2C1_txptr = I2C1_TxData;
	I2C1_rxptr = I2C1_RxData;

	hetInit();
	hetREG1->INTENAS = 0xFFFFFFFFU;	// todo jc 20141024 check if this is needed
	hetREG1->GCR = 0x01010001;	// todo jc 20141024 check if this is needed

	 _enable_IRQ();	// todo jc 20141024 check if this is needed

	 IntEna =1;
	 RW = 0; //write
	 NumOfBytes = 1;
	 I2C1_TxData[0] = 0x20; // % charge left


	HetI2CPutAddr(I2C_ADDR, RW, NumOfBytes, IntEna, StopBit);
	for(wait_counter=0;wait_counter<0x18;wait_counter++); //wait some time.


	RW = 1; //Read
	IntEna = 0;//no transmit interrupt
	HetI2CPutAddr(I2C_ADDR, RW, NumOfBytes, IntEna, StopBit);

	while(1);

	/* USER CODE END */
}

/* USER CODE BEGIN (4) */
/* USER CODE END */
