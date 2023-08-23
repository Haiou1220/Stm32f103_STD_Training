#include "dma_ws2812.h"
#include "stm32f10x.h"


void TIM_SelectOutputTrigger(TIM_TypeDef* TIMx, uint16_t TIM_TRGOSource);//CR2

void TIM_UpdateRequestConfig(TIM_TypeDef* TIMx, uint16_t TIM_UpdateSource_);//CR1   TIM_UpdateSource_Global
void TIM_UpdateDisableConfig(TIM_TypeDef* TIMx, FunctionalState NewState);//CR1

 

void TIM_DMAConfig(TIM_TypeDef* TIMx, uint16_t TIM_DMABase, uint16_t TIM_DMABurstLength);//DCR
void TIM_DMACmd(TIM_TypeDef* TIMx, uint16_t TIM_DMASource, FunctionalState NewState);//DIER

void TIM_ITConfig(TIM_TypeDef* TIMx, uint16_t TIM_IT, FunctionalState NewState);//DIER

 
 
 
 
 
 
 
 
 
 
void ws2812_DMA1CH2_to_Tim3OCR3_init(void )//DMA1ch2��ʼ��_�����ַ��tim3ch3_�Ƚ�����Ĵ���
{
		   //  DMA1ʱ�� 
	 RCC_AHBPeriphClockCmd(  RCC_AHBPeriph_DMA1,   ENABLE);
	/*
	Ҫ��ʼ�µ�DMA���䣬��Ҫ�ڹر�DMAͨ��������£���DMA_CNDTRx�Ĵ���������д�봫����Ŀ��
	*/
    u32 PeripheralBaseAddr_dest = (u32) &(TIM3->CCR3);
	
	DMA_InitTypeDef  DMA_InitStruct;
	DMA_InitStruct.DMA_BufferSize  = 1 ;//ƭ�����
	
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;// ���ݷ���  
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable; //�� M2M
	
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStruct.DMA_Priority = DMA_Priority_Low;
	
	DMA_InitStruct.DMA_MemoryBaseAddr = 0;
	DMA_InitStruct.DMA_MemoryDataSize =DMA_MemoryDataSize_Byte; //         
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable; //��Դ��ַ����u8����
	
	DMA_InitStruct.DMA_PeripheralBaseAddr = PeripheralBaseAddr_dest;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;// 16λ      
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // ������
	
    DMA_Init( DMA1_Channel2, &DMA_InitStruct);
	
	//���ô�������ж�
	 NVIC_InitTypeDef NVIC_InitStruct;//�����жϹ�����
	 NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel2_IRQn;
	 NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	 NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	 NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	 NVIC_Init(& NVIC_InitStruct);
	//������������ж�
	DMA_ITConfig(DMA1_Channel2,DMA_IT_TC,ENABLE);
 
	DMA_Cmd( DMA1_Channel2,    DISABLE );//���ʹ�����ʹ��    

}





 
void ws2812_DMA1CH3_to_TIM3UP_init(u32 u8_buf_addr_Sourse ,u32 led_count)//DMA1ch2��ʼ��_�����ַ��tim3ch3_�Ƚ�����Ĵ���
{
		   //  DMA1ʱ�� 
	 RCC_AHBPeriphClockCmd(  RCC_AHBPeriph_DMA1,   ENABLE);
	/*
	Ҫ��ʼ�µ�DMA���䣬��Ҫ�ڹر�DMAͨ��������£���DMA_CNDTRx�Ĵ���������д�봫����Ŀ��
	*/
    u32 PeripheralBaseAddr_dest = (u32) &(TIM3->CCR3);
	
	DMA_InitTypeDef  DMA_InitStruct;
	DMA_InitStruct.DMA_BufferSize  = led_count*24;
	
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;// ���ݷ���  
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable; //�� M2M
	
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStruct.DMA_Priority = DMA_Priority_Low;
	
	DMA_InitStruct.DMA_MemoryBaseAddr = u8_buf_addr_Sourse;
	DMA_InitStruct.DMA_MemoryDataSize =DMA_MemoryDataSize_Byte; // 
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable; //��Դ��ַ����
	
	DMA_InitStruct.DMA_PeripheralBaseAddr = PeripheralBaseAddr_dest;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;// 16λ
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // ������
	
    DMA_Init( DMA1_Channel3, &DMA_InitStruct);
	
	//���ô�������ж�
	 NVIC_InitTypeDef NVIC_InitStruct;//�����жϹ�����
	 NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel3_IRQn;
	 NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	 NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	 NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	 NVIC_Init(& NVIC_InitStruct);
	//������������ж�
	DMA_ITConfig(DMA1_Channel3,DMA_IT_TC,ENABLE);
 
	DMA_Cmd( DMA1_Channel3,    DISABLE );//ʹ�� ���ȴ�����

}

void ws2812_DMA1CH3_for_TIM3UP__Send(u32 buf_addr ,u32  LED_count)// 
{			 
		 
		//PWM - ��ʱ�����ʺ��� �����¼���Ϊ��������
//	
//		DMA_Cmd( DMA1_Channel3,    DISABLE );// 
//	
//	    DMA1_Channel3->CNDTR =  LED_count*24;
//		DMA1_Channel3->CMAR = buf_addr;
//	
//	    TIM_Cmd(TIM3,ENABLE);
//		DMA_Cmd( DMA1_Channel3,    ENABLE );//ʹ�� ���ȴ�����
		 
}



void ws2812_DMA1CH2_for_TIM3OCR3_Send(u32 buf_addr ,u32 LED_count)// (u32)&GRB_CCRx_buf ,  (sizeof(GRB_CCRx_buf)/sizeof(GRB_CCRx_buf[0]))
{
		u32 zeroOneCode_u8buf_count = (  LED_count )*24;
		
		DMA_Cmd( DMA1_Channel2,    DISABLE );// 
		
		DMA1_Channel2->CMAR = buf_addr;
	    DMA1_Channel2->CNDTR =  zeroOneCode_u8buf_count;
		TIM_GenerateEvent(TIM3,TIM_EventSource_Update); // �ֶ����������¼� ,����Ĵ����������÷���������о�Ҫ����ʹ��
	 
	
	  TIM_Cmd(TIM3,ENABLE);
		DMA_Cmd( DMA1_Channel2,    ENABLE );//ͨ��ʹ�� ���ȴ�����
}









