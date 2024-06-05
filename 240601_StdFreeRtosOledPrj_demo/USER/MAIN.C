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
#include "stdio.h"
#include "ssd1306.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

/*Compiler definec : USE_STDPERIPH_DRIVER,STM32F10X_MD */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
void start_task(void *);
void oled_task( void* );
void led_task( void* );
void usart1_task( void* );

/* Private macro -------------------------------------------------------------*/
TaskHandle_t oledTask_Handler;
TaskHandle_t ledTask_Handler;
TaskHandle_t usart1Task_Handler;

/* Private variables ---------------------------------------------------------*/

#define Led0_On GPIO_WriteBit( GPIOC,GPIO_Pin_13, Bit_RESET );
#define Led0_Off GPIO_WriteBit( GPIOC,GPIO_Pin_13, Bit_SET );

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void led_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct); // PC13
}

void usart1_init(uint32_t BaudRate) //pa9 tx,  pa10 rx 中断接收
{
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1,   ENABLE);//开启外设时钟
	//配置gpio
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOA, & GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init( GPIOA, & GPIO_InitStruct);
	//配置usart1
	USART_InitTypeDef  USART_InitStruct;
	USART_InitStruct.USART_BaudRate = BaudRate;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(  USART1, &USART_InitStruct);
	//配置串口中断
	NVIC_InitTypeDef  NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(& NVIC_InitStruct);
	//开启串口中断
	USART_ITConfig( USART1,   USART_IT_RXNE,  ENABLE );
	//开启串口
	USART_Cmd( USART1, ENABLE );
}

int fputc(int c,FILE* f)
{
																																				  
	while ( USART_GetFlagStatus(   USART1,  USART_FLAG_TXE ) != SET) {}; //等待TXE=1发送寄存器空，表示发送完成，由硬件复位，TC与TXE复位值为1
	USART_SendData(  USART1,  c);
	
	return c;
}

int main(void)
{		
  NVIC_PriorityGroupConfig(  NVIC_PriorityGroup_2);//配置中断分组  
  // 创建开始任务
  xTaskCreate((TaskFunction_t)start_task,			// 任务函数
				(const char *)"start_task",			// 任务名称
				(uint16_t)512,						// 任务堆栈大小
				(void *)NULL,						// 传递给任务函数的参数
				(UBaseType_t)10,					// 任务优先级
				(TaskHandle_t *)NULL);              // 任务句柄
  vTaskStartScheduler();
  while (1);
}

void start_task(void *argv)
{
	taskENTER_CRITICAL();
	xTaskCreate(oled_task,"oled_task",512,NULL,10,&oledTask_Handler); 
	xTaskCreate(led_task,"led_task",512,NULL,10,&ledTask_Handler);
	xTaskCreate(usart1_task,"usart1_task",512,NULL,10,&usart1Task_Handler);
	taskEXIT_CRITICAL();
	vTaskDelete(NULL);
}

void oled_task(void *argv)
{
	uint32_t len = 0, count = 0;
	taskENTER_CRITICAL();
	OLED_bsp_init();
	OLED_init();
	OLED_Clear();
	taskEXIT_CRITICAL();
	// OLED_ShowChar(0,0, 'L');
	len = OLED_ShowString(0, 0, "GHH_Val:");
	// OLED_ClearLine(0, 0);

	while (1)
	{
		vTaskDelay(pdMS_TO_TICKS(1000));
		taskENTER_CRITICAL();	
		OLED_PrintSignedVal(len, 0, count++);
		taskEXIT_CRITICAL();
	}
}

void led_task(void *argv)
{
	uint32_t count = 0;
	led_init();

	while (1)
	{
		vTaskDelay(pdMS_TO_TICKS(1000));
		if((count++)%2 == 0) 
		{Led0_On;}else{Led0_Off;}
	}
}

void usart1_task(void *argv)
{
	uint32_t count = 0;
	usart1_init(115200); // pa9 tx,  pa10 rx
	while (1)
	{
		vTaskDelay(pdMS_TO_TICKS(1000));
		printf("IN usart1_task:%d\r\n",count++);
	}
}
