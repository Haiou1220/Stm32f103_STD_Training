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
#include "main.h"
#include "string.h"
/*Compiler definec : USE_STDPERIPH_DRIVER,STM32F10X_MD */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

#define Led0_On GPIO_WriteBit( GPIOB,GPIO_Pin_5, Bit_RESET );
#define Led0_Off GPIO_WriteBit( GPIOB,GPIO_Pin_5, Bit_SET );
#define Led1_On GPIO_WriteBit( GPIOE,GPIO_Pin_5, Bit_RESET );
#define Led1_Off GPIO_WriteBit( GPIOE,GPIO_Pin_5, Bit_SET );

#define  RS485RxEnalbe GPIO_WriteBit( GPIOD ,GPIO_Pin_7,Bit_RESET   ); //PD7 485_rn_pin
#define  RS485TxEnalbe GPIO_WriteBit( GPIOD,GPIO_Pin_7, Bit_SET    );



RS485Recv_t  RS485Recv;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void led_init(void )//led0 led1 pb5 pe5
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB,&GPIO_InitStruct);//pb5
  GPIO_Init(GPIOE,&GPIO_InitStruct);//pb5

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
	RS485RxEnalbe;//开启485接收

}

int fputc(int c,FILE* f)
{
	USART_SendData(  USART1,  c);																																	  
	while ( USART_GetFlagStatus(   USART1,  USART_FLAG_TXE ) == RESET) ; //TXE写DR硬件清零
	return c;
}

void rs485_send(u8* pBuf,u16 len) //单双工 模式 ，不能一边接收一边发送
{	
	RS485TxEnalbe;//开启发送
	for(u16 i = 0;i<len;i++)
	{
		//USART_ClearFlag(  USART2,  USART_FLAG_TC);//while USART_FLAG_TC 需要 USART_ClearFlag
	USART_SendData(  USART2,  pBuf[i] );
	while ( USART_GetFlagStatus( USART2,  USART_FLAG_TXE) == RESET ); //  TC/TXE上电默认1，TC软件序列清零；TXE写DR硬件清零
	
	//等待发送完成
	
	}
	//延时恢复接收,发送需要时间
	for(  int i = 0;i<99999;i++);
	RS485RxEnalbe;//恢复接收
}



void rs485_init(uint32_t BaudRate ) //配置usart2为rs485,pa2 tx,pa3 rx
{	
	
	//开启相关时钟
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 ,  ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE ); // //PD7 485_rn_pin
	//配置pa2 pa3 gpio复用功能
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2; // 
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init( GPIOA, &GPIO_InitStruct);
	////PD7 485_rn_pin
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init( GPIOD, &GPIO_InitStruct);
	//串口
	USART_InitTypeDef   USART_InitStruct;
	USART_InitStruct.USART_BaudRate = BaudRate ;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	//配置usart2
	USART_Init(  USART2,  &USART_InitStruct);
	//配置usart2 接收中断接收
	USART_ITConfig( USART2,USART_IT_RXNE, ENABLE);
	//配置usart2中断优先级
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority =2;
	NVIC_Init(&NVIC_InitStruct);
	//开启串口
	USART_Cmd( USART2,  ENABLE);
	//开启485接收
	RS485RxEnalbe;//485接收

}




int main(void)
{
  
	
	  
 led_init( );//led0 led1 pb5 pe5
 NVIC_PriorityGroupConfig(  NVIC_PriorityGroup_2);//配置中断分组
  usart1_init(115200 ) ;//pa9 tx,  pa10 rx
   rs485_init( 9600 ) ;//配置usart2为rs485,pa2 tx,pa3 rx
  Led1_On;	
  Led0_On;
	rs485_send((u8*)"RS485_SEND_IS_WORK",strlen("RS485_SEND_IS_WORK"));  //单双工 模式 ，不能一边接收一边发送
	printf("IN MAIN\r\n");
	
  while (1) {
 
	}
   
   
}






