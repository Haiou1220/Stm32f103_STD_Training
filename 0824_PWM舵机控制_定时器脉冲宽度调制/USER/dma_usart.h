#ifndef __DMA_USART_H__
#define __DMA_USART_H__
#include "stm32f10x.h"


void dma1CH4_USART1_TX_init(void);

void usart1_sendBuf_dma1CH4_start(u32 SendDataBufADDR ,u16 count);//函数 //使能串口 发送请求



#endif



