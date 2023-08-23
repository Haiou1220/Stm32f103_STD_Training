#include "usart.h"
#include "stdio.h"

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
	//只开启串口    接收中断
	USART_ITConfig( USART1,   USART_IT_RXNE,  ENABLE );
	//开启串口
	USART_Cmd( USART1, ENABLE );

}




int fputc(int c,FILE* f)		//两种 现在   一是 串口发送，一是dma发送  串口解析（如果是接收可以用接收空闲作为触发？？是吗）
{
																																				  
	while ( USART_GetFlagStatus(   USART1,  USART_FLAG_TXE ) != SET) {}; //等待TXE=1发送寄存器空，表示发送完成，由硬件复位，TC与TXE复位值为1
	USART_SendData(  USART1,  c);
	
	return c;
}





