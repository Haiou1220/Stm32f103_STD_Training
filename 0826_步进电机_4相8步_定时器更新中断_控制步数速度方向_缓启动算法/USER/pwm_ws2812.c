
#include "pwm_rgbled.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "systick.h"
#include "global.h"
#include "ws2812.h"
  		 
//	//ws2812_
//	void ws2812_refresh(u32 number )
//	{
//		
//	
//	}
// 
//void DMA_Init(DMA_Channel_TypeDef* DMAy_Channelx, DMA_InitTypeDef* DMA_InitStruct);//TIM3 CH3 DMA1CH2	
//void DMA_Cmd(DMA_Channel_TypeDef* DMAy_Channelx, FunctionalState NewState);
//void DMA_ITConfig(DMA_Channel_TypeDef* DMAy_Channelx, uint32_t DMA_IT, FunctionalState NewState);
//void DMA_SetCurrDataCounter(DMA_Channel_TypeDef* DMAy_Channelx, uint16_t DataNumber); 
//uint16_t DMA_GetCurrDataCounter(DMA_Channel_TypeDef* DMAy_Channelx);
//FlagStatus DMA_GetFlagStatus(uint32_t DMAy_FLAG);
//void DMA_ClearFlag(uint32_t DMAy_FLAG);
//ITStatus DMA_GetITStatus(uint32_t DMAy_IT);
//void DMA_ClearITPendingBit(uint32_t DMAy_IT);

//�ڴ浽�ڴ�//�ڴ浽�ڴ�
void dma1_ch1_mem2mem_init(void) // 
{
	//����ʱ�� 
	RCC_AHBPeriphClockCmd(  RCC_AHBPeriph_DMA1,   ENABLE);
	/*
	Ҫ��ʼ�µ�DMA���䣬��Ҫ�ڹر�DMAͨ��������£���DMA_CNDTRx�Ĵ���������д�봫����Ŀ��
	*/
	DMA_Cmd( DMA1_Channel1,    DISABLE);//�ر�ͨ�� ������ // 
	
	DMA_InitTypeDef  DMA_InitStruct;
	DMA_InitStruct.DMA_BufferSize  = 1;
	
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;// ���ݷ���  
	DMA_InitStruct.DMA_M2M = DMA_M2M_Enable;
	
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStruct.DMA_Priority = DMA_Priority_Low;
	
	DMA_InitStruct.DMA_MemoryBaseAddr = 0;
	DMA_InitStruct.DMA_MemoryDataSize =DMA_MemoryDataSize_Byte;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	
	DMA_InitStruct.DMA_PeripheralBaseAddr = 0;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
	
    DMA_Init( DMA1_Channel1, &DMA_InitStruct);
	
	//�����жϹ�����
	 NVIC_InitTypeDef NVIC_InitStruct;
	 NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	 NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	 NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
	 NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
	 NVIC_Init(& NVIC_InitStruct);
	//������������ж�
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);
	DMA_Cmd( DMA1_Channel1,   DISABLE );//��ʼ����
}

void dma1_ch1_mem2mem_u8_send(u32 Sourse ,u32 tar_addr ,u32 count_size) //DMA MEM2MEM ��������ж�
{
	/*
	Ҫ��ʼ�µ�DMA���䣬��Ҫ�ڹر�DMAͨ��������£���DMA_CNDTRx�Ĵ���������д�봫����Ŀ��
	*/
	DMA_Cmd( DMA1_Channel1,    DISABLE);//�ر�ͨ�� ������ // 
	
	DMA1_Channel1->CNDTR =  count_size;
	DMA1_Channel1->CPAR =  tar_addr;
	DMA1_Channel1->CMAR = Sourse;

	DMA_Cmd( DMA1_Channel1,    ENABLE );//��ʼ����
}

 
//�ڴ浽�ڴ�//�ڴ浽�ڴ�//�ڴ浽�ڴ�


 


 
//void ws2812_time3_PWM_ch3_pb0_send_one_color(Color24bit_t Color24)
//{
//	 
//	for( u16 i = 0;i<24 ;i++)
//	{
//		 
//		
//		if(Color24.Color24bit & (0x00800000>>i))
//		{
//			 ws2812_time3_PWM_ch3_pb0_send_1_code() ;
//			
//		}else{
//		
//			 ws2812_time3_PWM_ch3_pb0_send_0_code();
//			 
//		}
//		 
//		 
//	}

//}

//void ws2812_time3_PWM_ch3_pb0_send_colorbuf(Color24bit_t* p_Color24,  u8 count)
//{
////TIM_SetCompare3(TIM3,  36);//���ñȽ�ֵCCR1 
//	for(u16 i = 0;i< count ; i++)
//	{	
//	  ws2812_time3_PWM_ch3_pb0_send_one_color(*p_Color24);
//	}
//	
//    ws2812_time3_PWM_ch3_pb0_send_Res_code( );// 300us res

//}


 



 

u8 ws2812_PwmTIM3CH3_init(void)  // ,ʱ������  PB0,ws2812,//ws2812 1.25us  0.8M 
{	
	
	/*
	��ʱ�� 72M        
	Ҫ��1250ns=1.25us=0.8M����,   72M/0.8M =90   ��Ҫ90��Ƶ  ===����2*45��������45, 1*90��������,��1000 �� 0.8  ��250 ��0.2 ��45-1
	72Mf=2��Ƶ��36M��45��Ƶ��0.8M = 1.25us
	*/
	     u8 ret;
		//����ʱ�� 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3,  ENABLE);

	
	                                
		//����ʱ����Ԫtime3
		TIM_TimeBaseInitTypeDef   TIM_TimeBaseInitStruct;
    	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_TimeBaseInitStruct.TIM_CounterMode =  TIM_CounterMode_Up;
	
//	 	TIM_TimeBaseInitStruct.TIM_Prescaler =  2-1; //        (��ƵҪ��һ)
//	 	TIM_TimeBaseInitStruct.TIM_Period =  45-1  ; //         ��װ��ֵҪ����0��cn������
//	
			TIM_TimeBaseInitStruct.TIM_Prescaler =  1-1; //        (��ƵҪ��һ)
		    TIM_TimeBaseInitStruct.TIM_Period =  90-1  ; //         ��װ��ֵҪ����0��cn������
		
		
		
	TIM_TimeBaseInit(  TIM3, &TIM_TimeBaseInitStruct);
	TIM_ARRPreloadConfig( TIM3, ENABLE); // ʱ����Ԫ3   ���� arr Ԥװ��
 
		//���� PB0 ���� 
		GPIO_InitTypeDef  GPIO_InitStruct;
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		
		//���� �Ƚ����
		TIM_OCInitTypeDef TIM_OCInitStruct;
		TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1; //���ϼ��� С��Ŀ��ֵ ���1
		TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
		TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Disable;//Ĭ�Ϲر�
		TIM_OCInitStruct.TIM_Pulse = 0;//CCRx �Ƚ�ֵ
		  //TIM_OCInitStruct.TIM_OCNIdleState = �߼���ʱ����
		   //TIM_OCInitStruct.TIM_OCIdleState = �߼���ʱ���� ;
		 //TIM_OCInitStruct.TIM_OutputNState =�߼���ʱ����
		//TIM_OCInitStruct.TIM_OCNPolarity = �߼���ʱ����
	   TIM_OC3Init( TIM3, &TIM_OCInitStruct); 	// ����  ccmr ģʽ�Ĵ��� ��cce ʹ�ܼĴ���  ��ccrx
	   TIM_OC3PreloadConfig( TIM3, TIM_OCPreload_Enable);  // ����ccmrx_oc2pe ����Ƚ� ���� ccrx Ԥװ��



	  	TIM_DMACmd( TIM3,  TIM_DMA_CC3,  ENABLE);//��ʱ��   �Ƚ� DMA���󣬣���,  
  
		 
		  TIM_CCxCmd(TIM3,TIM_Channel_3,TIM_CCx_Enable);  //ͨ�� �����������  
		  
	 
	  	 TIM_Cmd( TIM3, DISABLE );//�ر� �� 
	
	
	
		 ret =  0 ;
	
return ret;	
}

