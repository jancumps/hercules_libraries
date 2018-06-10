/*
 * bitmaputils.c
 *
 *  Created on: 16 mei 2016
 *      Author: jancu
 */

#include "bitmaputils.h"

#include "sys_common.h"
#include "stddef.h"
#include "mibspi.h"
#include "sys_dma.h"


#include "HX8353E.h"

/* buffer memory in special location for DMA / MibSPI */
#define D_SIZE      128

#pragma SET_DATA_SECTION(".sharedRAM")
#pragma DATA_ALIGN(TXDATA, 8); /* Note: It is very important to keep the data alignment on 64-bit for the DMA */
uint16_t TXDATA[D_SIZE/2];         /* transmit buffer in sys ram */
#pragma SET_DATA_SECTION()
g_dmaCTRL g_dmaCTRLPKT;    /* dma control packet configuration stack */



void loadBitmapInDMABuffer(bitmap_t *bmp, uint32_t row, uint32_t offset);
//void mibspiEnableInternalLoopback(mibspiBASE_t *mibspi);
void dmaConfigCtrlPacket(uint32 sadd,uint32 dadd,uint32 dsize);
void mibspiDmaConfig(mibspiBASE_t *mibspi,uint32 channel, uint32 txchannel);

void drawBitmap(bitmap_t *bmp, uint32_t x, uint32_t y) {
	uint32_t xx, yy;

    for (yy = 0; yy < bmp->height; yy++) {
        for (xx = 0; xx < bmp->width; xx++) {
            point(xx, yy, (bmp->data)[(yy)*bmp->height + (xx) ]);
        }
    }
}


void initDMA() {
       dmaReqAssign(0, 15 );
       /* - configuring dma control packets   */
       dmaConfigCtrlPacket((uint32)(&TXDATA),(uint32)(&(mibspiRAM3->tx[0].data) + 4),(D_SIZE)); // +  because we're already consuming part of the buffer
       /* upto 32 control packets are supported. */

       /* - setting dma control packets */
       dmaSetCtrlPacket(DMA_CH0,g_dmaCTRLPKT);

       /* - setting the dma channel to trigger on h/w request */
       dmaSetChEnable(DMA_CH0, DMA_HW);

       /* - configuring the mibspi dma , channel 0 , tx line -0 , rxline -1     */
       /* - refer to the device data sheet dma request source for mibspi tx/rx  */
       mibspiDmaConfig(mibspiREG3,0,0);

       // first set the read mode
       TXDATA[0] = 0x03;
       // - enabling dma module
       dmaEnable();


}


void loadBitmapInDMABuffer(bitmap_t *bmp, uint32_t row, uint32_t offset) {
//  uint32_t i;
//  for(i=0; i < D_SIZE; i++) { // todo clean up use of D_SIZE and bmp.x, bmp.y, etc...
//      TXDATA[i] = (bmp->data)[(row)*bmp->width + (i)];
//  }

//  void* destination = &TXDATA;
//  const void * source = (const void *)(bmp->data + (bmp->width) * row); //
//  size_t howmanybytes = 2 * D_SIZE;



    const void * addr = (bmp->data + (bmp->width) * row) + offset;

    memcpy(&TXDATA, addr , (2 * D_SIZE)/2);
//  memcpy((void *)(&TXDATA), (const void *)(bmp->data[row]), 2 * D_SIZE);

}



void flashBitmapLineDMA(bitmap_t *bmp, uint32_t line) {

    _setWindow(0, line, bmp->width/2, line );
    loadBitmapInDMABuffer(bmp, line, 0);
    initDMA();
    _writeData64DMA();

    _setWindow(bmp->width/2, line, bmp->width, line );
    loadBitmapInDMABuffer(bmp, line, D_SIZE / 2);
    initDMA();
    _writeData64DMA();



}

void flashBitmapLine(bitmap_t *bmp, uint32_t line) {
    _setWindow(0, line, bmp->width, line );
    loadBitmapInDMABuffer(bmp, line, 0);
    _writeData64(&TXDATA[0]);
    loadBitmapInDMABuffer(bmp, line, D_SIZE / 2);
    _writeData64(&TXDATA[0]);


}

// ported from Screen_HX8353E
void setBitmapInOneThrow(bitmap_t *bmp, bool bDMA) {
    uint32_t yy;



    for (yy = 0; yy < bmp->height; yy++) {
        if (! bDMA) {
            flashBitmapLine(bmp, yy);  // uncomment if non-MDA chosen
        } else {

            flashBitmapLineDMA(bmp, yy); // uncomment if DMA chosen
        }
    }
}

void mibspiDmaConfig(mibspiBASE_t *mibspi,uint32 channel, uint32 txchannel)
{
    uint32 bufid  = 0;
    uint32 icount = 0;

    /* setting transmit and receive channels */
    mibspi->DMACTRL[channel] |= ((txchannel) << 16);

    /* enabling transmit and receive dma */
    mibspi->DMACTRL[channel] |=  0x8000C000; // todo: disable receive

    /* setting Initial Count of DMA transfers and the buffer utilized for DMA transfer */
    mibspi->DMACTRL[channel] |=  (icount << 8) |(bufid<<24);

}

/** void dmaConfigCtrlPacket(uint32 sadd,uint32 dadd,uint32 dsize)
*
*   configuring dma control packet stack
*
*       sadd  > source address
*       dadd  > destination  address
*       dsize > data size
*
*   @ note : after configuring the stack the control packet needs to be set by calling dmaSetCtrlPacket()
*/
void dmaConfigCtrlPacket(uint32 sadd,uint32 dadd,uint32 dsize)
{
      g_dmaCTRLPKT.SADD      = sadd;              /* source address             */
      g_dmaCTRLPKT.DADD      = dadd;              /* destination  address       */
      g_dmaCTRLPKT.CHCTRL    = 0;                 /* channel control            */
      g_dmaCTRLPKT.FRCNT     = 1;                 /* frame count                */
      g_dmaCTRLPKT.ELCNT     = dsize;             /* element count              */
      g_dmaCTRLPKT.ELDOFFSET = 4;                 /* element destination offset */
      g_dmaCTRLPKT.ELSOFFSET = 0;                 /* element destination offset */
      g_dmaCTRLPKT.FRDOFFSET = 0;                 /* frame destination offset   */
      g_dmaCTRLPKT.FRSOFFSET = 0;                 /* frame destination offset   */
      g_dmaCTRLPKT.PORTASGN  = 4;                 /* port b                     */
      g_dmaCTRLPKT.RDSIZE    = 0;     /* read size                  */
      g_dmaCTRLPKT.WRSIZE    = ACCESS_16_BIT;     /* write size                 */
      g_dmaCTRLPKT.TTYPE     = FRAME_TRANSFER ;   /* transfer type              */
      g_dmaCTRLPKT.ADDMODERD = ADDR_INC1;         /* address mode read          */
      g_dmaCTRLPKT.ADDMODEWR = ADDR_OFFSET;       /* address mode write         */
      g_dmaCTRLPKT.AUTOINIT  = AUTOINIT_ON;       /* autoinit                   */
}



