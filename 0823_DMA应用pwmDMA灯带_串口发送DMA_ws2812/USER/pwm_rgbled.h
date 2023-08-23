#ifndef __PWM_H__
#define __PWM_H__

#include "stm32f10x.h"
#include "ws2812.h"





 void time2_PWM_ch2_pa1_init(void) ;  //PA1,已经配置了时基tim2,  并设置了ARR:1000，时间：1*10^-6  *1*10^3 = 1*10^-3 = 1ms    
 void time4_PwmLed_3ch_pd121314_init(void);   //完全重映射 pd12-13-14
 u16 convert_255To999(u8 color);
void show_RGB_LED_3Channel( RGB_t RGBcolor); //设置 ccrX  与 时间基本单位aar 比较 
void show_RGB_LED_3Channel_colorBuf_sequence( RGB_t* p_RGBcolor_t,  u8 count);
void show_TIM4PWM_RGBLED_3Ch_colorBuf_sequence_gradient( RGB_t* p_RGBcolor_t,  u8 count);
 void time4_PwmLed_3ch_Switch(u8 on_off) ; //
	 
#endif




















