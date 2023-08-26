#include "led.h"
#include "stm32f10x.h"
#include "global.h"
#include "systick.h" 

void led_init(void )//led0 led1 pb5 pe5
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB,&GPIO_InitStruct);//pb5
  GPIO_Init(GPIOE,&GPIO_InitStruct);// 

}

void led_blink(u32 us) 
{	
	static u32 temp = 0;
	if (temp != us) { temp = us; SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; sysTickDelay_us_NonBlock(us);  }      //¸üÐÂÁËus
			
		
	 if(getFlagSysTickDelayOut() )
	{
	Led1_Switch; 
	}
}


