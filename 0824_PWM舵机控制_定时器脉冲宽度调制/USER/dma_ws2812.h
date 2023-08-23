#ifndef __DMA_WS2812_H__
#define __DMA_WS2812_H__
#include "stm32f10x.h"

 

void ws2812_DMA1CH2_to_Tim3OCR3_init(  void );//DMA1ch2初始化_外设地址是tim3ch3_比较输出寄存器
	


void ws2812_DMA1CH3_to_TIM3UP_init(u32 u8_buf_addr_Sourse ,u32 BYTE_count) ;//DMA1ch2初始化_外设地址是tim3ch3_比较输出寄存器

 

void ws2812_DMA1CH3_for_TIM3UP__Send(u32 buf_addr ,u32  LED_count);// 

void ws2812_DMA1CH2_for_TIM3OCR3_Send(u32 Color_buf_addr ,u32 LED_count);// 
	

#endif






