#ifndef   __SPI_HARD_H__
#define   __SPI_HARD_H__
#include "stm32f10x.h"


void spi_hard_init(void); // spi1Ӳ����ʼ��,ģʽ0  NSS ���ģʽ ,//PA4 -NSS //PA5-SCK  //PA6-MI  //PA7-MO
	

void spi_hard_CS(u8 cs_bit);
u8 spi_hard_swapByte( u8 byte);  // �����ֽ�

#endif


