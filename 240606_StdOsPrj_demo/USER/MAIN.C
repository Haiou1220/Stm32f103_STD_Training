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
void my_system_init(void);
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
/*
Rom start:0x08000000 size:0x10000  因为flash:64KB字节,sram:20KB,
 flash:65536Byte = 0x10000 字节

 bootloader设置: Rom start:0x8005000 size:0x10000-0x5000 = 0xb000 

*/
/*
*/
int main(void)
{		
  my_system_init();
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
		OLED_PrintSignedVal(len, 0, count--);
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

void my_system_init(void)
{
ErrorStatus HSEStartUpStatus;
 RCC_DeInit();
#if 0  //与 RCC_DeInit()一致
//启用内部振动器
  /* Set HSION bit */
  RCC->CR |= (uint32_t)0x00000001;

//HSI为系统时钟,HSI为系统时钟状态,AHB预分频为不分频
//(APB1)为不分频,APB2为不分频,ADC预分频为2分频,
//微控制器时钟输出为无输出
  /* Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits */
  RCC->CFGR &= (uint32_t)0xF8FF0000;


//PLL使能为失能;CSS时钟安全系统使能为关闭;HSE振荡器为关闭
    /* Reset HSEON, CSSON and PLLON bits */
  RCC->CR &= (uint32_t)0xFEF6FFFF;

//外部高速时钟旁路为无旁路
  /* Reset HSEBYP bit */
  RCC->CR &= (uint32_t)0xFFFBFFFF;

//PLL输入时钟源:HSI的2分频;PLLXTPRE为HSE不分频;
//PLLMUL[3:0]:PLL 2倍频输出;USBPRE:PLL时钟1.5倍分频作为USB时钟
/* Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits */
  RCC->CFGR &= (uint32_t)0xFF80FFFF;

//flag[7-0]:通过写1来清除就绪中断标志,为什么配置是写0
//IE[15-8]:写0,中断关闭
//ready interrupt clear[20-16]:1f,写1,清除就绪标志位(PLL,HSE,HIS,LSE,LSI)
//CSSC 写1,清除CSSF安全系统中断标志位
  /* Disable all interrupts and clear pending bits  */
  RCC->CIR = 0x009F0000;
  #endif

#if 1 // 与SetSysClockTo72() 内容一致性

 /* Enable HSE */
 RCC_HSEConfig(RCC_HSE_ON);
 /* Wait till HSE is ready and if Time out is reached exit */
 HSEStartUpStatus = RCC_WaitForHSEStartUp();
 if (HSEStartUpStatus == SUCCESS)
 {

	 /* Enable Prefetch Buffer */
	 // FLASH->ACR |= FLASH_ACR_PRFTBE;
	 FLASH_HalfCycleAccessCmd(FLASH_PrefetchBuffer_Enable);
	 /* Flash 2 wait state */
	 // FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
	 // FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;
	 FLASH_SetLatency(FLASH_Latency_2);
	 /* HCLK = SYSCLK */
	 // RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
	 RCC_HCLKConfig(RCC_SYSCLK_Div1);
	 /* PCLK2 = HCLK */
	 // RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
	 RCC_PCLK2Config(RCC_HCLK_Div1);
	 /* PCLK1 = HCLK */
	 // RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;
	 RCC_PCLK1Config(RCC_HCLK_Div2);
	 /*  PLL configuration: PLLCLK = HSE * 9 = 72 MHz */
	 // RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE |
	 //                                    RCC_CFGR_PLLMULL));
	 // RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9);
	 RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
	 /* Enable PLL */
	 // RCC->CR |= RCC_CR_PLLON;
	 RCC_PLLCmd(ENABLE);
	 /* Wait till PLL is ready */
	 // while((RCC->CR & RCC_CR_PLLRDY) == 0)
	 //{
	 // }
	 while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
	 {
	 }
	 /* Select PLL as system clock source */
	 // RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
	 // RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;
	 RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	 /* Wait till PLL is used as system clock source */
	 // while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
	 //{
	 // }
	 while (RCC_GetSYSCLKSource() != (uint8_t)0x08)
	 {
	 }
 }
#endif

  //SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; 
  /* Vector Table Relocation in Internal FLASH. */                                      
 NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x00 );//bootloader设置:0x5000
  
}

