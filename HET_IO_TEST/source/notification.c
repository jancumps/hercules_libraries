/** @file notification.c 
*   @brief User Notification Definition File
*   @date 9.Sep.2014
*   @version 04.01.00
*
*   This file  defines  empty  notification  routines to avoid
*   linker errors, Driver expects user to define the notification. 
*   The user needs to either remove this file and use their custom 
*   notification function or place their code sequence in this file 
*   between the provided USER CODE BEGIN and USER CODE END.
*
*/

/* Include Files */

#include "esm.h"
#include "sys_selftest.h"
#include "het.h"

/* USER CODE BEGIN (0) */
#include "HET_EMU.h"


extern unsigned int Data_Send_HET;
extern unsigned int Data_Rece_HET;
extern char I2C_ADDR;
extern unsigned char *I2C1_txptr;
extern unsigned char *I2C1_rxptr;
extern char IntEna;
extern char RW;
extern unsigned int Start_Repeat;

/* USER CODE END */
#pragma WEAK(esmGroup1Notification)
void esmGroup1Notification(uint32 channel)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (1) */
/* USER CODE END */
}

/* USER CODE BEGIN (2) */
/* USER CODE END */
#pragma WEAK(esmGroup2Notification)
void esmGroup2Notification(uint32 channel)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (3) */
/* USER CODE END */
}

/* USER CODE BEGIN (4) */
/* USER CODE END */
#pragma WEAK(memoryPort0TestFailNotification)
void memoryPort0TestFailNotification(uint32 groupSelect, uint32 dataSelect, uint32 address, uint32 data)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (5) */
/* USER CODE END */
}

/* USER CODE BEGIN (6) */
/* USER CODE END */
#pragma WEAK(memoryPort1TestFailNotification)
void memoryPort1TestFailNotification(uint32 groupSelect, uint32 dataSelect, uint32 address, uint32 data)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (7) */
/* USER CODE END */
}

/* USER CODE BEGIN (8) */
/* USER CODE END */


#pragma WEAK(pwmNotification)
void pwmNotification(hetBASE_t * hetREG,uint32 pwm, uint32 notification)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (35) */
/* USER CODE END */
}

/* USER CODE BEGIN (36) */
/* USER CODE END */
#pragma WEAK(edgeNotification)
void edgeNotification(hetBASE_t * hetREG,uint32 edge)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (37) */
/* USER CODE END */
}

/* USER CODE BEGIN (38) */
/* USER CODE END */
#pragma WEAK(hetNotification)
void hetNotification(hetBASE_t *het, uint32 offset)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (39) */
    uint32 vect = offset;
	switch (vect)
    {
    case 11: /*--------------------------->      Transmit interrupt */
    	if(Data_Send_HET<3)
    	{
    		HetI2CPutData(*I2C1_txptr++, IntEna);
    		Data_Send_HET++;
    	}
    	if(Start_Repeat==1)
    	{
    		if(Data_Send_HET<5)
    		{
    			HetI2CPutData(*I2C1_txptr++, IntEna);
    			Data_Send_HET++;
    		}
    		else if(Data_Send_HET==5)
    		{
    			RW = 1; //Read
    			IntEna = 0;//no transmit interrupt
    			HetI2CPutAddr(I2C_ADDR, RW, 2, IntEna, 1);//now, start to receive two data bytes and generate stop bit
    		}
    	}
    	break;
    case 15: /*--------------------------->      Receive interrupt */
    	*I2C1_rxptr++ = (hetRAM1->Instruction[0x2C].Data>>8) & 0xFF;  // read the data
    	Data_Rece_HET++;
        break;
   	case 18:  /*--------------------------->      No ACK interrupt */

   	case 29: /* timeout2*/
   	case 8: /* timeout6*/
   	case 2: /* timeout9*/

   	default:
        	/* timeout + phantom interrupt, clear flags and return */
       	while(1);
       	break;
    }
/* USER CODE END */
}

/* USER CODE BEGIN (40) */
/* USER CODE END */


/* USER CODE BEGIN (43) */
/* USER CODE END */


/* USER CODE BEGIN (47) */
/* USER CODE END */


/* USER CODE BEGIN (50) */
/* USER CODE END */


/* USER CODE BEGIN (53) */
/* USER CODE END */


/* USER CODE BEGIN (56) */
/* USER CODE END */

/* USER CODE BEGIN (58) */
/* USER CODE END */

/* USER CODE BEGIN (60) */
/* USER CODE END */
