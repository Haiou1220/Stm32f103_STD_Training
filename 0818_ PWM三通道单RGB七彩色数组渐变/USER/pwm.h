#ifndef __PWM_H__
#define __PWM_H__

#include "stm32f10x.h"


typedef struct 
{
	u8 R;
	u8 G;
	u8 B;
}RGBcolor_t ;


#define RAINBOW_COLOR_BUF_COUNT 7

u8 time3_PWM_ch3_init(void) ; // 配置，时基tim3ARR:1000     ch3,PB0
 void time2_PWM_ch2_pa1_init(void) ;  //PA1,已经配置了时基tim2,  并设置了ARR:1000，时间：1*10^-6  *1*10^3 = 1*10^-3 = 1ms    
 void time4_PWM_ch1to3_init(void);   //完全重映射 pd12-13-14
 u16 convert_255To999(u8 color);
void show_RGB_LED_3Channel( RGBcolor_t RGBcolor); //设置 ccrX  与 时间基本单位aar 比较 
void show_RGB_LED_3Channel_colorBuf_sequence( RGBcolor_t* p_RGBcolor_t,  u8 count);

void show_RGB_LED_3Channel_colorBuf_sequence_gradient( RGBcolor_t* p_RGBcolor_t,  u8 count);

 void time4_PWM_ch1to3_Switch(u8 on_off) ; //
	 
#endif




















