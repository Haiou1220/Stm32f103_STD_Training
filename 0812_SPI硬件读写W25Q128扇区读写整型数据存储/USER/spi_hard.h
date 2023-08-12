#ifndef   __SPI_HARD_H__
#define   __SPI_HARD_H__
#include "stm32f10x.h"


void spi_hard_init(void); // spi1硬件初始化,模式0  NSS 软件模式 ,//PA4 -NSS //PA5-SCK  //PA6-MI  //PA7-MO
	

void spi_hard_CS(u8 cs_bit);
u8 spi_hard_swapByte( u8 byte);  // 交换字节

#endif


