#include "dma_usart.h"
#include "stm32f10x.h"
#include "global.h"


/*

//USART1_TX   DMA1  ͨ��4

//USART1_RX   DMA1  ͨ��5


*/





void dma1CH4_USART1_TX_init(void)
{
		if( !(USART1->CR1 | 0x2000 ))// USART enable λ
		{
			ErrorInfo("Error:in FUN dma1CH4_USART1_TX_init ,usart NO  init " );
			return;
		}
;
		USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);//����1������������
	//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
		
		DMA_InitTypeDef  DMA_InitStruct;
		DMA_InitStruct.DMA_BufferSize = 1;  
		DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;//������Ŀ�ĵأ��������ݵ����ڷ��ͼĴ���
		DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
		DMA_InitStruct.DMA_MemoryBaseAddr = 0;
		DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //Դ8bit
		DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
		DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)&USART1->DR;
		DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;// Ŀ��8bit
		DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;
		 
		DMA_Init(DMA1_Channel4, &DMA_InitStruct) ;
		
		//�����ж�
		NVIC_InitTypeDef  NVIC_InitStruct;
		NVIC_InitStruct.NVIC_IRQChannel =  DMA1_Channel4_IRQn;
		NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
		NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
		NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
        NVIC_Init(&NVIC_InitStruct);
		
		DMA_ITConfig( DMA1_Channel4,  DMA_IT_TC, ENABLE);//�����ж�
		DMA_Cmd( DMA1_Channel4 ,DISABLE); //�رշ����ٿ��� ���� //���� ͨ�� �ȴ�����
}

void usart1_sendBuf_dma1CH4_start(u32 SendDataBufADDR ,u16 count)//���� //ʹ�ܴ��� ��������
{
	 
	DMA_Cmd(DMA1_Channel4,DISABLE); //�ر�������
	
	DMA1_Channel4->CMAR = SendDataBufADDR;
	DMA1_Channel4->CNDTR = count;
	
	DMA_Cmd(DMA1_Channel4,ENABLE);//����ͨ�� �ȴ����� USART_Cmd(USART1,ENABLE);    //ʹ�ܴ��� ��������
	
	

}

//void USART_ITConfig(USART_TypeDef* USARTx, uint16_t USART_IT, FunctionalState NewState);
  



