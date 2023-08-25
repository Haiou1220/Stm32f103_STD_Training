#ifndef __SPI_SOFT_H__
#define __SPI_SOFT_H__

#include "stm32f10x.h"

u8 spi_soft_init(s8 mode);  //MODE; 0 1 2 3

void spi_soft_CS(u8 bit); //CSÐÅºÅ
u8 spi_soft_swapByte(u8 Byte);


#endif

