#ifndef __USART_H__
#define __USART_H__
#include "stm32f10x.h"

void usart1_init(uint32_t BaudRate); //pa9 tx,  pa10 rx �жϽ���

extern u8 gUSART1_receive_IT_u8data;
extern u8 gUSART1_receive_IT_u8falg;
void USART1_receive_ITcallback(void);


#endif









