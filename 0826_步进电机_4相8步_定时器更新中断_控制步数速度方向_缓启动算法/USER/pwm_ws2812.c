
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

//内存到内存//内存到内存
void dma1_ch1_mem2mem_init(void) // 
{
	//开启时钟 
	RCC_AHBPeriphClockCmd(  RCC_AHBPeriph_DMA1,   ENABLE);
	/*
	要开始新的DMA传输，需要在关闭DMA通道的情况下，在DMA_CNDTRx寄存器中重新写入传输数目。
	*/
	DMA_Cmd( DMA1_Channel1,    DISABLE);//关闭通道 再配置 // 
	
	DMA_InitTypeDef  DMA_InitStruct;
	DMA_InitStruct.DMA_BufferSize  = 1;
	
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;// 数据方向  
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
	
	//配置中断管理器
	 NVIC_InitTypeDef NVIC_InitStruct;
	 NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	 NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	 NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
	 NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
	 NVIC_Init(& NVIC_InitStruct);
	//开启传输完成中断
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);
	DMA_Cmd( DMA1_Channel1,   DISABLE );//开始发送
}

void dma1_ch1_mem2mem_u8_send(u32 Sourse ,u32 tar_addr ,u32 count_size) //DMA MEM2MEM 开启完成中断
{
	/*
	要开始新的DMA传输，需要在关闭DMA通道的情况下，在DMA_CNDTRx寄存器中重新写入传输数目。
	*/
	DMA_Cmd( DMA1_Channel1,    DISABLE);//关闭通道 再配置 // 
	
	DMA1_Channel1->CNDTR =  count_size;
	DMA1_Channel1->CPAR =  tar_addr;
	DMA1_Channel1->CMAR = Sourse;

	DMA_Cmd( DMA1_Channel1,    ENABLE );//开始发送
}

 
//内存到内存//内存到内存//内存到内存


 


 
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
////TIM_SetCompare3(TIM3,  36);//配置比较值CCR1 
//	for(u16 i = 0;i< count ; i++)
//	{	
//	  ws2812_time3_PWM_ch3_pb0_send_one_color(*p_Color24);
//	}
//	
//    ws2812_time3_PWM_ch3_pb0_send_Res_code( );// 300us res

//}


 



 

u8 ws2812_PwmTIM3CH3_init(void)  // ,时基配置  PB0,ws2812,//ws2812 1.25us  0.8M 
{	
	
	/*
	定时器 72M        
	要求1250ns=1.25us=0.8M周期,   72M/0.8M =90   需要90分频  ===》（2*45：计数到45, 1*90计数到）,长1000 ， 0.8  短250 ，0.2 ，45-1
	72Mf=2分频，36M，45分频，0.8M = 1.25us
	*/
	     u8 ret;
		//开启时钟 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3,  ENABLE);

	
	                                
		//配置时基单元time3
		TIM_TimeBaseInitTypeDef   TIM_TimeBaseInitStruct;
    	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_TimeBaseInitStruct.TIM_CounterMode =  TIM_CounterMode_Up;
	
//	 	TIM_TimeBaseInitStruct.TIM_Prescaler =  2-1; //        (分频要减一)
//	 	TIM_TimeBaseInitStruct.TIM_Period =  45-1  ; //         （装载值要大于0，cn工作）
//	
			TIM_TimeBaseInitStruct.TIM_Prescaler =  1-1; //        (分频要减一)
		    TIM_TimeBaseInitStruct.TIM_Period =  90-1  ; //         （装载值要大于0，cn工作）
		
		
		
	TIM_TimeBaseInit(  TIM3, &TIM_TimeBaseInitStruct);
	TIM_ARRPreloadConfig( TIM3, ENABLE); // 时基单元3   开启 arr 预装载
 
		//配置 PB0 复用 
		GPIO_InitTypeDef  GPIO_InitStruct;
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		
		//配置 比较输出
		TIM_OCInitTypeDef TIM_OCInitStruct;
		TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1; //向上计数 小于目标值 输出1
		TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
		TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Disable;//默认关闭
		TIM_OCInitStruct.TIM_Pulse = 0;//CCRx 比较值
		  //TIM_OCInitStruct.TIM_OCNIdleState = 高级定时器用
		   //TIM_OCInitStruct.TIM_OCIdleState = 高级定时器用 ;
		 //TIM_OCInitStruct.TIM_OutputNState =高级定时器用
		//TIM_OCInitStruct.TIM_OCNPolarity = 高级定时器用
	   TIM_OC3Init( TIM3, &TIM_OCInitStruct); 	// 配置  ccmr 模式寄存器 ，cce 使能寄存器  ，ccrx
	   TIM_OC3PreloadConfig( TIM3, TIM_OCPreload_Enable);  // 配置ccmrx_oc2pe 输出比较 开启 ccrx 预装载



	  	TIM_DMACmd( TIM3,  TIM_DMA_CC3,  ENABLE);//定时器   比较 DMA请求，，，,  
  
		 
		  TIM_CCxCmd(TIM3,TIM_Channel_3,TIM_CCx_Enable);  //通道 开启，如果是  
		  
	 
	  	 TIM_Cmd( TIM3, DISABLE );//关闭 ， 
	
	
	
		 ret =  0 ;
	
return ret;	
}

