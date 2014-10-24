/*
 * HET_EMU.h
 *
 *  Created on: Nov 28, 2012
 *      Author: a0866528
 */

#ifndef HET_EMU_H_
#define HET_EMU_H_
#define I2CRxReady (hetREG1->FLG & (1<<23))
void HetI2CPutData(char Data, char IntEna);
void HetI2CPutAddr(char Addr, char RW, char NumOfBytes, char IntEna, char StopBit);
//void HetI2CPutRepAddr(char Addr, char RW, char IntEna);
//void HetI2CPutText(char *text);
//unsigned HetI2UART1Printf(const char *_format, ...);
char HetI2CGetChar(void);

#endif /* HET_EMU_H_ */
