#ifndef __SYSTICK_H__
#define __SYSTICK_H__

#include "stm32f10x.h"


     
	void sysTick_init(void);//³õÊ¼»¯systick	
	void  sysTickDelay_ms(uint32_t delay_count); //9M ->9*1000 000  

	u8  sysTickDelay_us_Block(uint32_t delay_count); //9M ->:   (9*1000 000 -> 1000 ms ,1ms= 9000)   (9*1000 000 -> 1000 000 us ,1us = 9)

	void  sysTickDelay_us_NonBlock(uint32_t delay_count); //9M ->:   (9*1000 000 / 1000 ms = 9000)   (9*1000 000 -> 1000 000 us ,1us = 9)
	u8 getFlagSysTickDelayOut(void);

	
	


#endif

