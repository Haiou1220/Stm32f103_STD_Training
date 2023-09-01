#include "stm32f10x.h"
#include "stdio.h"
#include "usart.h"
#include "global.h"
#include "FreeRTOS.h"
#include "task.h"
 



u8 gUSART1_receive_IT_u8data;
u8 gUSART1_receive_IT_u8falg;
void USART1_receive_ITcallback(void);

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
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 5;// 硬件 中断优先级   ,//RTOS 系统可管理的最高中断优先级 =  5, 所以硬件的优先级不能高于rtos            
	//NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2; //子优先级 rtos不使用
	NVIC_Init(& NVIC_InitStruct);
	//开启串口中断
	USART_ITConfig( USART1,   USART_IT_RXNE,  ENABLE );
	//开启串口
	USART_Cmd( USART1, ENABLE );


  

}

void USART1_receive_ITcallback_task_deal(void) //中断串口命令 任务处理 ，， 中断 恢复 led0_task
{	
    extern	TaskHandle_t     led0_task_TaskHandle;
	BaseType_t xYieldRequired ;
	
	printf(" IN: USART1_receive_ITcallback_task_deal(void) \r\n");
	
	if(gUSART1_receive_IT_u8data ==  '5') // 中断恢复任务
	{
	 
	xYieldRequired =  xTaskResumeFromISR(   led0_task_TaskHandle ) ; //中断恢复 led0_task
	
		if(xYieldRequired == pdTRUE)
		{
		
			printf("中断恢复任务成功 led0_task \r\n");
			
			portYIELD_FROM_ISR(xYieldRequired);
		}
	
	}


}



void USART1_receive_ITcallback(void)
{	
	 
	if(USART_GetITStatus( USART1,USART_IT_RXNE) == SET )// 中断接收
	{	
		gUSART1_receive_IT_u8falg = 1;
		USART_ClearITPendingBit( USART1, USART_IT_RXNE);
		
		gUSART1_receive_IT_u8data = (u8)USART_ReceiveData(USART1);
		
	 	USART_SendData(USART1,gUSART1_receive_IT_u8data  );
		 
			 
		USART1_receive_ITcallback_task_deal();//中断串口命令 任务处理
		
	}
	
}

























int fputc(int c,FILE* f)
{
																																				  
	while ( USART_GetFlagStatus(   USART1,  USART_FLAG_TXE ) != SET) {}; //等待TXE=1发送寄存器空，表示发送完成，由硬件复位，TC与TXE复位值为1
	USART_SendData(  USART1,  c);
	
	return c;
}







