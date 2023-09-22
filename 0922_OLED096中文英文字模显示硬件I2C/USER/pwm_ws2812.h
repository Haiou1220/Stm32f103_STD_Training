#ifndef __WS2812_PWM_H__
#define __WS2812_PWM_H__

#include "stm32f10x.h"
#include "ws2812.h"

//内存到内存
void dma1_ch1_mem2mem_init(void) ;
void dma1_ch1_mem2mem_u8_send(u32 Sourse ,u32 tar_addr ,u32 count_size);
 
//内存到内存








void ws2812_t3c3_pwm_dma1_c2_send(u32 Sourse ,u32 dest  ,u32 count);
void ws2812_pwm_dma_init(void);

void ws2812_pwm_dma_send(u32 Sourse ,u32 tar_addr ,u32 count_size) ;
u8  ws2812_pwm_dma_getflag_finsh(void) ;


///////

 

////////////
u8 ws2812_PwmTIM3CH3_init(void) ; // 配置，时基tim3ARR:1000     ch3,PB0
 

	 
#endif



