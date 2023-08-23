
 #ifndef __LED_H__
#define __LED_H__

#include "stm32f10x.h"
 

#define Led0_On GPIO_WriteBit( GPIOB,GPIO_Pin_5, Bit_RESET )
#define Led0_Off GPIO_WriteBit( GPIOB,GPIO_Pin_5, Bit_SET )
#define Led1_On GPIO_WriteBit( GPIOE,GPIO_Pin_5, Bit_RESET )
#define Led1_Off GPIO_WriteBit( GPIOE,GPIO_Pin_5, Bit_SET )
#define Led1_Switch GPIO_WriteBit( GPIOE,GPIO_Pin_5, (BitAction)!GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_5) ) 


void led_init(void );//led0 led1 pb5 pe5
 

void led_blink(u32 us) ;


#endif 
