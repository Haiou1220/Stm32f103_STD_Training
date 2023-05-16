/**
  ******************************************************************************
  * @file    GPIO/IOToggle/main.c 
  * @author  MCD Application Team
  * @version V3.6.0
  * @date    20-September-2021
  * @brief   Main program body.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2011 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
/*Compiler definec : USE_STDPERIPH_DRIVER,STM32F10X_MD */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
unsigned int  g_timerCount = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */     
       
  /* GPIOD Periph clock enable */
 
	RCC_APB2PeriphClockCmd(  RCC_APB2Periph_GPIOC ,   ENABLE);//使能pc13时钟
	
	GPIO_InitTypeDef GPIO_InitStruct;	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(   GPIOC,   & GPIO_InitStruct);//初始化pc13
	GPIO_SetBits(GPIOC,GPIO_Pin_13);//灭灯

	RCC_APB1PeriphClockCmd(  RCC_APB1Periph_TIM2, ENABLE );   //开启定时器外设时钟
	TIM_TimeBaseInitTypeDef   TIM_TimeBaseInitStruct;//初始化定时器
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 1000 ;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 72-1; // 1M ->1us ->1ms   
	TIM_TimeBaseInit(   TIM2, &TIM_TimeBaseInitStruct  );//配置定时器2	 
	 TIM_ClearFlag(  TIM2,   TIM_FLAG_Update);//清除中断标志物//TIM_ClearITPendingBit
	 TIM_ITConfig(  TIM2,TIM_IT_Update,ENABLE) ;//配置定时器2计数中断
	

	NVIC_PriorityGroupConfig(  NVIC_PriorityGroup_2);//配置中断组
	NVIC_InitTypeDef NVIC_InitType  ;	
	NVIC_InitType.NVIC_IRQChannel = TIM2_IRQn; //定时器2中断优先级
	NVIC_InitType.NVIC_IRQChannelPreemptionPriority =  2;
	NVIC_InitType.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitType.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init (&NVIC_InitType);//配置中断控制器

	TIM_Cmd( TIM2,  ENABLE);//使能定时器2
	
	
  /* Configure PD0 and PD2 in output pushpull mode */
 

  /* To achieve GPIO toggling maximum frequency, the following  sequence is mandatory. 
     You can monitor PD0 or PD2 on the scope to measure the output signal. 
     If you need to fine tune this frequency, you can add more GPIO set/reset 
     cycles to minimize more the infinite loop timing.
     This code needs to be compiled with high speed optimization option.  */
  while (1) {


	  
	if(g_timerCount >=2000) //1ms*1000 = 1s
	{
	   GPIO_WriteBit(  GPIOC,   GPIO_Pin_13,(BitAction)!GPIO_ReadOutputDataBit(  GPIOC,   GPIO_Pin_13) );
		g_timerCount = 0 ;

	}

  }
   
   
}




