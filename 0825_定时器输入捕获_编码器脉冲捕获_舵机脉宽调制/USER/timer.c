#include "timer.h"
#include "stm32f10x.h"


void  timer2_countdown_init(void)        //配置定时器2定时1ms中断
{
	//打开定时器2时钟
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2,  ENABLE);
	//配置定时器2
	TIM_TimeBaseInitTypeDef   TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_TimeBaseInitStruct.TIM_CounterMode =  TIM_CounterMode_Up;
		TIM_TimeBaseInitStruct.TIM_Period =  1000  ;    // 1*10-3 -> 1ms（装载值要大于0，cn工作）
		TIM_TimeBaseInitStruct.TIM_Prescaler =  72-1; //1M ->1*10-6(分频要减一)
	TIM_TimeBaseInit(  TIM2, &TIM_TimeBaseInitStruct);
	
	TIM_ARRPreloadConfig( TIM2, ENABLE); // 时基单元2    开启 arr 预装载
	
	//配置定时器2中断
	TIM_ITConfig(  TIM2,  TIM_IT_Update,  ENABLE);
	//定时器2中断优先级
	NVIC_InitTypeDef    NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel =  TIM2_IRQn; 
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority =  2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(  &NVIC_InitStruct);
 	//定时器2 使能
 	TIM_Cmd(  TIM2,  ENABLE);

 
}



