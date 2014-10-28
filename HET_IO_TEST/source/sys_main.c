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
unsigned char I2C1_TxData[5] = {0xA5,1,2,0x5A,0x1E};
unsigned char I2C1_RxData[20];
unsigned int Data_Send_HET = 0, Data_Rece_HET = 0;
unsigned int Data_Send_I2C = 0, Data_Rece_I2C = 0;
// unsigned int Stop_Rece_I2C = 0;
unsigned int Start_Repeat=0;
char IntEna =1;
char RW = 0; //write
char I2C_ADDR; // todo: jc 20141024 this is a hack to make the TI emukator code work, candidate for refactoring

#define I2C_MSGSIZE 3

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

/*
 * this is the original test program I made

   	volatile unsigned int delay;
	HETPROGRAM0_UN * pHetProgram;

	hetInit();
	while (1) {
		pHetProgram = (HETPROGRAM0_UN *)hetRAM1;

	}

*/



	// set the i2c address
	I2C_ADDR = 0x55;


	int i;

	char NumOfBytes =3;
	char StopBit = 1;
	volatile unsigned int wait_counter;


/**	//char Data = 0xAA;
	(*(volatile unsigned int *)(0xFFFFEA38))=0x83E70B13;
    (*(volatile unsigned int *)(0xFFFFEA3C))=0x95A4F1E0;

    *(volatile unsigned int *) 0xFFFFEB10 = 0x02020101; // SCL 0[17], SDA 0[25],

    (*(volatile unsigned int *)(0xFFFFEA38))=0x0;
    (*(volatile unsigned int *)(0xFFFFEA3C))=0x0;

	i2cInit(0,	I2C2_ADDR);/** - set i2c mode */

	/*
	 * asm(" cpsie i");
	 */



	// todo jc 20141024 this is part of TI's i2c example loop, remove
	for (i=0;i<I2C_MSGSIZE;i++)
	{
		I2C1_RxData[i] = 0xff;
	}
	I2C1_txptr = I2C1_TxData;
	I2C1_rxptr = I2C1_RxData;

	hetInit();
	hetREG1->INTENAS = 0xFFFFFFFFU;	// todo jc 20141024 check if this is needed
	//asm(" cpsie i");
	hetREG1->GCR = 0x01010001;	// todo jc 20141024 check if this is needed

	 _enable_IRQ();	// todo jc 20141024 check if this is needed


	 NumOfBytes = 1;
	 I2C1_TxData[0] = 0x20; // % charge left


	//test for master transmit mode
	HetI2CPutAddr(I2C_ADDR, RW, NumOfBytes, IntEna, StopBit);
//	while(Stop_Rece_I2C == 0); //todo: this seems to be related wuith the genuine i2c peripheral, should be ignored
//	Stop_Rece_I2C = 0; //todo: this seems to be related wuith the genuine i2c peripheral, should be ignored
	for(wait_counter=0;wait_counter<0x18;wait_counter++); //wait some time.
	//test for master receive mode
	RW = 1; //Read
	IntEna = 0;//no transmit interrupt
	HetI2CPutAddr(I2C_ADDR, RW, NumOfBytes, IntEna, StopBit);
	//I2C_MasterTransmitStart(I2C2_ADDR);
/*
	//test for repeated start mode
	while(Stop_Rece_I2C == 0);//wait until master transmit completes.
	Start_Repeat = 1;
	for(wait_counter=0;wait_counter<0x18;wait_counter++); //wait some time.
	RW = 0; //write
	IntEna = 1;//transmit interrupt
	StopBit = 0; //no stop bit for the first transfer
	NumOfBytes = 2; //write 2 data, 1 repeat address, read 2 data
	HetI2CPutAddr(I2C_ADDR, RW, NumOfBytes, IntEna, StopBit);
*/
	//i2cREG1->DXR = *I2C1_txptr++;
	asm(" cpsie f");
	while(1);




	/* USER CODE END */
}

/* USER CODE BEGIN (4) */
/* USER CODE END */
