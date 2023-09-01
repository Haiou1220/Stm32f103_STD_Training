#include "stm32f10x.h"
#include "stdio.h"
#include "usart.h"
#include "global.h"
#include "FreeRTOS.h"
#include "task.h"
 



u8 gUSART1_receive_IT_u8data;
u8 gUSART1_receive_IT_u8falg;
void USART1_receive_ITcallback(void);

void usart1_init(uint32_t BaudRate) //pa9 tx,  pa10 rx �жϽ���
{

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1,   ENABLE);//��������ʱ��
	//����gpio
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOA, & GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init( GPIOA, & GPIO_InitStruct);
	//����usart1
	USART_InitTypeDef  USART_InitStruct;
	USART_InitStruct.USART_BaudRate = BaudRate;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(  USART1, &USART_InitStruct);
	//���ô����ж�
	NVIC_InitTypeDef  NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 5;// Ӳ�� �ж����ȼ�   ,//RTOS ϵͳ�ɹ��������ж����ȼ� =  5, ����Ӳ�������ȼ����ܸ���rtos            
	//NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2; //�����ȼ� rtos��ʹ��
	NVIC_Init(& NVIC_InitStruct);
	//���������ж�
	USART_ITConfig( USART1,   USART_IT_RXNE,  ENABLE );
	//��������
	USART_Cmd( USART1, ENABLE );


  

}

void USART1_receive_ITcallback_task_deal(void) //�жϴ������� ������ ���� �ж� �ָ� led0_task
{	
    extern	TaskHandle_t     led0_task_TaskHandle;
	BaseType_t xYieldRequired ;
	
	printf(" IN: USART1_receive_ITcallback_task_deal(void) \r\n");
	
	if(gUSART1_receive_IT_u8data ==  '5') // �жϻָ�����
	{
	 
	xYieldRequired =  xTaskResumeFromISR(   led0_task_TaskHandle ) ; //�жϻָ� led0_task
	
		if(xYieldRequired == pdTRUE)
		{
		
			printf("�жϻָ�����ɹ� led0_task \r\n");
			
			portYIELD_FROM_ISR(xYieldRequired);
		}
	
	}


}



void USART1_receive_ITcallback(void)
{	
	 
	if(USART_GetITStatus( USART1,USART_IT_RXNE) == SET )// �жϽ���
	{	
		gUSART1_receive_IT_u8falg = 1;
		USART_ClearITPendingBit( USART1, USART_IT_RXNE);
		
		gUSART1_receive_IT_u8data = (u8)USART_ReceiveData(USART1);
		
	 	USART_SendData(USART1,gUSART1_receive_IT_u8data  );
		 
			 
		USART1_receive_ITcallback_task_deal();//�жϴ������� ������
		
	}
	
}

























int fputc(int c,FILE* f)
{
																																				  
	while ( USART_GetFlagStatus(   USART1,  USART_FLAG_TXE ) != SET) {}; //�ȴ�TXE=1���ͼĴ����գ���ʾ������ɣ���Ӳ����λ��TC��TXE��λֵΪ1
	USART_SendData(  USART1,  c);
	
	return c;
}







