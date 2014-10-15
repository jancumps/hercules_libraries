//******************************************************************************
//  TMS470R1A256 Demo - 2 x 115,200 Baud UART using HET, Interrupt driven
//
//  Description: This program implements a 2x115,200 Baud UARTs using the HET.
//  The received character are echo'ed back. Interrupt driven program flow is
//  used. Pins HET24/HET31 are used for UART1 and pins HET0/HET2 are used for
//  UART2. Note that any HET pins can be used.
//  SYSCLK = 4x11.0592MHz = 44.2368MHz.
//
//            TMS470R1A256
//          +---------------+
//          |               |
//      +---|PLLDIS         |
//      |   |               |
//     ---  |               |
//     GND  |               |
//          |       XIN/XOUT|<--- 11.0592MHz crystal
//          |               |
//          |          HET24|---> 115,200 Baud UART1
//          |          HET31|<---
//          |               |
//          |          HET0 |---> 115,200 Baud UART2
//          |          HET2 |<---
//          |               |
//          +---------------+
//
//  Andreas Dannenberg
//  Texas Instruments, Inc
//  June 06th 2006
//  Built with IAR Embedded Workbench Version: 4.31A
//  A256 EVM
//******************************************************************************

// Include Standard C Language Header Files
#include <stdlib.h>
#include <stdio.h>

// Include TMS470 Specific Header Files
#include <intrinsic.h>
#include "iotms470r1a256.h"
#include "tms470r1a256_bit_definitions.h"
#include "std_het.h"
#include "A256_HET_UART_2X115200_H.h"

__no_init volatile HETPROGRAM0_UN e_HETPROGRAM0_UN @ 0x00800000;

//------------------------------------------------------------------------------
// Help function to load the HET program into the HET RAM using 32-bit access.
//------------------------------------------------------------------------------
void MemCopy32(unsigned long *dst, unsigned long *src, int bytes)
{
  for (int i = 0; i < (bytes + 3) / 4; i++)
    *dst++ = *src++;
}
//------------------------------------------------------------------------------
// void HetUART1PutByte(unsigned char Data)
//
// This function initializes the transmit one byte using the HET UART1.
// The transmit is performed ARM7 independent in the background by the
// HET program.
//------------------------------------------------------------------------------
void HetUART1PutByte(unsigned char Data)
{
  unsigned int Tmp = Data;

  Tmp <<= 1;                                    // Shift in start bit (0)
  Tmp |= 0x00000200;                            // Add stop bit (1)

  HET_TX1_Shift_0.memory.data_word = Tmp << 5;  // Load TX buffer
  HET_TX1_BitCtr_0.memory.data_word = 10 << 5;  // Load bit count
  HET_TX1_Start_0.memory.data_word = 0x1 << 5;  // Start TX
}
//------------------------------------------------------------------------------
// unsigned char HetUART1GetByte(void)
//
// This function reads out the last received character from the HET UART1.
//------------------------------------------------------------------------------
unsigned char HetUART1GetByte(void)
{
  unsigned int Tmp = HET_RX1_Buffer_0.memory.data_word >> 16;

  HET_RX1_Buffer_0.memory.data_word = 0;        // Clear RX buffer
  return Tmp;                                   // Return received byte
}
//------------------------------------------------------------------------------
// char HetUART1RxDataAvailable(void)
//
// This function checks if HET UART RX data is available and ready for
// read out.
//------------------------------------------------------------------------------
char HetUART1RxDataAvailable(void)
{
  unsigned int Tmp = HET_RX1_Buffer_0.memory.data_word >> 16;

  if (Tmp & 0x100)                              // Stop bit detected?
    return 1;                                   // Yes, data available
  else
    return 0;                                   // No data received
}
//------------------------------------------------------------------------------
// char HetUART1TxBufferEmpty(void)
//
// This function returns true if the HET UART1 TX buffer can be written to.
//------------------------------------------------------------------------------
char HetUART1TxBufferEmpty(void)
{
  if ((HET_TX1_BitCtr_0.memory.data_word >> 5) & 0xFFFFF)
    return 0;                                   // TX in progress
  else
    return 1;                                   // TX buffer empty
}
//------------------------------------------------------------------------------
// void HetUART2PutByte(unsigned char Data)
//
// This function initializes the transmit one byte using the HET UART2.
// The transmit is performed ARM7 independent in the background by the
// HET program.
//------------------------------------------------------------------------------
void HetUART2PutByte(unsigned char Data)
{
  unsigned int Tmp = Data;

  Tmp <<= 1;                                    // Shift in start bit (0)
  Tmp |= 0x00000200;                            // Add stop bit (1)

  HET_TX2_Shift_0.memory.data_word = Tmp << 5;  // Load TX buffer
  HET_TX2_BitCtr_0.memory.data_word = 10 << 5;  // Load bit count
  HET_TX2_Start_0.memory.data_word = 0x1 << 5;  // Start TX
}
//------------------------------------------------------------------------------
// unsigned char HetUART2GetByte(void)
//
// This function reads out the last received character from the HET UART2.
//------------------------------------------------------------------------------
unsigned char HetUART2GetByte(void)
{
  unsigned int Tmp = HET_RX2_Buffer_0.memory.data_word >> 16;

  HET_RX2_Buffer_0.memory.data_word = 0;        // Clear RX buffer
  return Tmp;                                   // Return received byte
}
//------------------------------------------------------------------------------
// char HetUART2RxDataAvailable(void)
//
// This function checks if HET UART RX data is available and ready for
// read out.
//------------------------------------------------------------------------------
char HetUART2RxDataAvailable(void)
{
  unsigned int Tmp = HET_RX2_Buffer_0.memory.data_word >> 16;

  if (Tmp & 0x100)                              // Stop bit detected?
    return 1;                                   // Yes, data available
  else
    return 0;                                   // No data received
}
//------------------------------------------------------------------------------
// char HetUART2TxBufferEmpty(void)
//
// This function returns true if the HET UART2 TX buffer can be written to.
//------------------------------------------------------------------------------
char HetUART2TxBufferEmpty(void)
{
  if ((HET_TX2_BitCtr_0.memory.data_word >> 5) & 0xFFFFF)
    return 0;                                   // TX in progress
  else
    return 1;                                   // TX buffer empty
}
//------------------------------------------------------------------------------
void main(void)
{
  PCR = 0x00;                                   // Disable peripherals
  GCR = ZPLL_MULT4;                             // Enable PLL multiply-by-4
  PCR = CLKDIV_2;                               // ICLK=SYSCLK/2
                                                // (to not violate max. ICLK)
  PCR |= PENABLE;                               // Enable peripherals
                                                // AFTER setting CLKDIV

  HETDOUT = 0x01000001;                         // Output HIGH on HET24/HET0
                                                // (RS-232 idle state)
  HETDIR  = 0x7FFFFFFB;                         // Set all HETx
                                                // but HET31/HET2 as outputs

  REQMASK = (1 << CIM_HET1);                    // Enable HET level 1 ints

  // Copy HET instructions to HET RAM
  MemCopy32((void *) &e_HETPROGRAM0_UN, (void *) HET_INIT0_PST,
            sizeof(HET_INIT0_PST));

  HETGCR = CLK_MASTER + IGNORE_SUSPEND;         // HET Master Mode,
                                                // Ignore SW BP
  HETPRY = (0x1 << 18) + (0x1 << 2) +           // Interrupts on instructions
           (0x1 << 27) + (0x1 << 11);           // 18, 2, 27, 11(43)
                                                // are high priority

  // Set HET PFR register
  // fSYSCLK = 44.2368MHz
  // fHR = fSYSCLK / 3 = 14.7456Mhz
  // fLR = fHR / 16 = 921.6kHz
  HETPFR = LRPRES_FACTOR_16 + HRPRES_FACTOR_3;

  __enable_interrupt();                         // Enable Interrupts

  HETGCR |= ON;                                 // Enable HET

  while (1)                                     // Loop forever...
  {
  }
}
//------------------------------------------------------------------------------
// HET UART1 Tx Handler
//
// This ISR is executed upon HET UART1 TX completion and can be used to transmit
// the next data byte.
//------------------------------------------------------------------------------
void HetUART1TxHandler()
{
}
//------------------------------------------------------------------------------
// HET UART1 Rx Handler
//
// This ISR is executed upon HET UART1 data receiption.
//------------------------------------------------------------------------------
void HetUART1RxHandler(void)
{
  if (HetUART1TxBufferEmpty())
    HetUART1PutByte(HetUART1GetByte());
}
//------------------------------------------------------------------------------
// HET UART2 Tx Handler
//
// This ISR is executed upon HET UART2 TX completion and can be used to transmit
// the next data byte.
//------------------------------------------------------------------------------
void HetUART2TxHandler()
{
}
//------------------------------------------------------------------------------
// HET UART2 Rx Handler
//
// This ISR is executed upon HET UART2 data receiption.
//------------------------------------------------------------------------------
void HetUART2RxHandler(void)
{
  if (HetUART2TxBufferEmpty())
    HetUART2PutByte(HetUART2GetByte());
}
//------------------------------------------------------------------------------
// HET Interrupt Level 1 Handler
//------------------------------------------------------------------------------
void HET1_irq_handler()
{
  switch ((HETOFF1 & 0xff) - 1)
  {
    case 2 :                                    // Int on HET instruction 2
      HetUART1TxHandler();
      break;
    case 11 :                                   // Int on HET instruction 11(43)
      HetUART2RxHandler();
      break;
    case 18 :                                   // Int on HET instruction 18
      HetUART1RxHandler();
      break;
    case 27 :                                   // Int on HET instruction 27
      HetUART2TxHandler();
      break;
  }
}
//------------------------------------------------------------------------------
// TMS470R1A256 Standard Interrupt Handler
//------------------------------------------------------------------------------
#pragma vector = IRQV
__irq __arm void irq_handler(void)
{
  switch ((IRQIVEC & 0xff) - 1)
  {
    case CIM_HET1 :
      HET1_irq_handler();
      break;
  }
}
