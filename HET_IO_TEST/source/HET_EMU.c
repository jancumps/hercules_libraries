#include "HET_EMU.h"
#include "HET.h"
#include <stdio.h>

/** @fn void HetUART1PutChar(unsigned char Data)
*   @brief Sends a character of characters over HET emulated SCI
*
*   @param[in]  Data		The character to send
*/
void HetI2CPutData(char Data, char IntEna)
{
  unsigned int Tmp = Data;

  Tmp <<= 22;                                    // Shift in start bit (0)
  //Tmp |= (NumOfBytes<<7);
  Tmp |= (IntEna & 1) <<18;
  Tmp |= 0x802A0000;                            // Add start bit, stopdata ready, bit (1)
  while(hetRAM1->Instruction[0].Data != 0);
  hetRAM1->Instruction[0].Data = Tmp;  // Load TX buffer
}

void HetI2CPutAddr(char Addr, char RW, char NumOfBytes, char IntEna, char StopBit)
{
  unsigned int Tmp = Addr;

  Tmp <<= 23;                                    // Shift in start bit (0)
  Tmp |= (NumOfBytes<<7);
  Tmp |= (IntEna & 1) <<18;
  if(StopBit) Tmp |= 0x8000;
  if(RW) //read
	  Tmp |= 0x806B0000;                            // Add start bit, stopdata ready, bit (1)
  else //write
	  Tmp |= 0x802B0000;                            // Add start bit, stopdata ready, bit (1)
  while(hetRAM1->Instruction[0].Data != 0);
  hetRAM1->Instruction[0].Data = Tmp;  // Load TX buffer
}

/*void HetI2CPutRepAddr(char Addr, char RW, char IntEna)
{
  unsigned int Tmp = Addr;

  Tmp <<= 23;                                    // Shift in start bit (0)
  //Tmp |= (NumOfBytes<<7);
  Tmp |= (IntEna & 1) <<18;
  if(RW) //read
	  Tmp |= 0x806B8000;                            // Add start bit, stopdata ready, bit (1)
  else //write
	  Tmp |= 0x802B8000;                            // Add start bit, stopdata ready, bit (1)
  while(hetRAM1->Instruction[0].Data != 0);
  hetRAM1->Instruction[0].Data = Tmp;  // Load TX buffer
}*/



/** @fn unsigned HetUART1Printf(const char *_format, ...)
*   @brief sends data to terminal (HET emulated SCI)
*
*   @param[in]  _format - string with format argument
*   @return      length of sent string
*
*   Sends formated string to terminal on HET emulated SCI
*/
/*
unsigned HetUART1Printf(const char *_format, ...)
{
   char str[128];
   int length = -1, k = 0;

   va_list argList;
   va_start( argList, _format );

   length = vsnprintf(str, sizeof(str), _format, argList);

   va_end( argList );

   if (length > 0)
   {
      for(k=0; k<length; k++)
      {
    	  HetUART1PutChar(str[k]);
      }
   }

   return (unsigned)length;
}
*/

/** @fn char HetUART1GetChar()
*   @brief		Gets a character from the HET Emulated SCI Receive Buffer if one is available
*
*	@return		The character in the receive buffer, if one is available. If not, 0.
*/
char HetI2CGetChar(void)
{
	unsigned int HetFlag;
	HetFlag = hetREG1->FLG;
	if(HetFlag & (1<<23))
	{
		hetREG1->FLG = (1<<23); // clear this bit
		return((char)(hetRAM1->Instruction[25].Data));
	}
	else
		return 0;
}
